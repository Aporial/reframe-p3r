#include "platform.hpp"
#include "menu_text.hpp"
#include "native_menu.hpp"
#include "renderer.hpp"
#include <MinHook.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace reframe {
namespace {
// Persona 3 Reload, Steam build 22672075. The controller and Blueprint draw
// actor share the same game-allocated row objects through TSharedPtr copies.
constexpr unsigned row_size = 0xb0;
constexpr int rendering_scale_id = 0x23;
constexpr int rendering_scale_max_index = 6;
constexpr unsigned save_rendering_scale = 0x1388720;
constexpr unsigned graphics_runtime = 0x122a8e0;
constexpr unsigned apply_rendering_scale = 0x1230e60;
constexpr int technology_id = 0x100;
constexpr unsigned rendering_scale_vtable = 0x4320c30;
constexpr unsigned shared_row_vtable = 0x4081bc8;
constexpr unsigned graphics_factory = 0x139b850;
constexpr unsigned build_draw_param = 0x13b56f0;
constexpr unsigned menu_tick = 0x13bc540;
constexpr unsigned list_step = 0x138bd00;
constexpr unsigned draw_cursor_changed = 0x16c2770;
constexpr unsigned controller_groups = 0x300;
constexpr unsigned controller_rows = 0x310;
constexpr unsigned controller_navigation = 0x3c0;
constexpr unsigned controller_active_navigation = 0x3d0;
constexpr unsigned controller_draw_actor = 0x430;
constexpr unsigned graphics_navigation = 0x78;
constexpr unsigned state_in_controller = 0x288;
constexpr int original_graphics_rows = 6;
constexpr int max_graphics_rows = original_graphics_rows + 4;
constexpr int sharpness_max_index = sharpness_max / sharpness_step;

template<class T> struct GameArray {
    T* data;
    int count;
    int capacity;
};
using GameString = GameArray<wchar_t>;
struct SharedRow;
struct SharedControl {
    void* vtable;
    int shared;
    int weak;
    void* row;
};
struct SharedRow {
    void* row;
    SharedControl* control;
};
using Rows = GameArray<SharedRow>;
using Groups = GameArray<Rows>;
using Factory = void (*)(Groups*, void*, void*);
using DrawParam = void* (*)(void*, SharedRow*);
using MenuTick = void (*)(std::byte*, int, float);
using ListStep = void (*)(void*, bool);

enum class RowKind { Technology, Quality, Model, Sharpness, Scale };
constexpr size_t Index(RowKind kind) { return static_cast<size_t>(kind); }

constexpr std::array<bool, 4> VisibleAdditions(Technology technology) {
    return {true, technology != Technology::Taa, technology == Technology::Dlss, technology != Technology::Taa};
}

static_assert(VisibleAdditions(Technology::Taa) == std::array{true, false, false, false});
static_assert(VisibleAdditions(Technology::Dlss) == std::array{true, true, true, true});
static_assert(VisibleAdditions(Technology::Fsr) == std::array{true, true, false, true});
static_assert(VisibleAdditions(Technology::Xess) == std::array{true, true, false, true});
static_assert(static_cast<int>(Model::Auto) == 0 && static_cast<int>(Model::K) == 1 &&
              static_cast<int>(Model::M) == 2 && static_cast<int>(Model::L) == 3);

struct MenuPage {
    Settings saved;
    Settings draft;
    std::array<bool, 4> available{};
    Size output;
    const MenuText* text = nullptr;
    void* controller = nullptr;
    int group_index = 0;
    std::array<void*, 5> rows{};
    // Technology is owned by the game list. Its destructor releases this
    // page's extra native references to its optional rows.
    std::array<SharedRow, 4> additions{};
    std::array<RenderRecommendation, static_cast<size_t>(Quality::Count)> resolutions{};
    ULONGLONG next_language_check = 0;
    bool layout_dirty = false;
    bool scale_dirty = false;
};

struct MenuRow {
    std::shared_ptr<MenuPage> page;
    RowKind kind;
    std::array<int, sharpness_max_index + 1> values{};
};

static_assert(sizeof(GameString) == 0x10 && sizeof(SharedRow) == 0x10 && sizeof(SharedControl) == 0x18);
std::byte* image = nullptr;
Factory original_factory = nullptr;
DrawParam original_draw_param = nullptr;
MenuTick original_menu_tick = nullptr;
ListStep original_list_step = nullptr;
std::array<void*, 23> row_functions{};
std::array<void*, 23> scale_functions{};
std::unordered_map<void*, MenuRow> rows;
std::unordered_map<void*, std::weak_ptr<MenuPage>> pages;

template<class Fn> Fn At(unsigned rva) {
    return reinterpret_cast<Fn>(image + rva);
}

template<class T> T& Field(void* object, unsigned offset) {
    return *reinterpret_cast<T*>(static_cast<std::byte*>(object) + offset);
}

template<class T> bool Valid(const GameArray<T>& array, int limit) {
    return array.data && array.count > 0 && array.capacity >= array.count && array.capacity <= limit;
}

void* GameAllocate(size_t size) {
    return At<void* (*)(size_t)>(0x109ad50)(size);
}

void GameFree(void* memory) {
    if (memory) At<void (*)(void*)>(0x174d4b0)(memory);
}

void ReleaseRow(SharedRow entry) {
    auto* control = entry.control;
    if (!control || --control->shared) return;
    auto** vtable = static_cast<void**>(control->vtable);
    reinterpret_cast<void (*)(SharedControl*)>(vtable[0])(control);
    if (!--control->weak)
        reinterpret_cast<void (*)(SharedControl*, unsigned)>(vtable[1])(control, 1);
}

void AssignText(GameString& target, std::wstring_view text) {
    const auto length = static_cast<int>(text.size()) + 1;
    GameString source{const_cast<wchar_t*>(text.data()), length, length};
    At<void (*)(GameString*, const GameString*)>(0x89b1b0)(&target, &source);
}

GameString CopyText(std::wstring_view text) {
    GameString result{};
    AssignText(result, text);
    return result;
}

constexpr std::wstring_view vendor_names[] = {L"", L"NVIDIA DLSS", L"AMD FSR", L"Intel XeSS"};
constexpr Quality quality_order[] = {Quality::Native, Quality::UltraQualityPlus, Quality::UltraQuality,
                                     Quality::Quality, Quality::Balanced, Quality::Performance};

bool CanEdit(const MenuRow& row) {
    const auto technology = row.page->draft.technology;
    if (row.kind == RowKind::Scale) return technology == Technology::Taa;
    return VisibleAdditions(technology)[Index(row.kind)];
}

void RefreshChoice(void* row, int choice) {
    Field<int>(row, 0x60) = std::clamp(choice, 0, Field<int>(row, 0x74) - 1);
    At<void (*)(void*)>(0x13a30f0)(row); // Native list value -> FString.
}

void RebuildRow(MenuPage& page, RowKind kind) {
    auto* row = page.rows[Index(kind)];
    if (!row) return;
    auto& meta = rows.at(row);
    auto& options = Field<GameArray<GameString>>(row, 0xa0);
    for (int i = 0; i < options.count; ++i) {
        GameFree(options.data[i].data);
        options.data[i] = {};
    }
    options.count = 0;
    const auto add = [&](int value, std::wstring_view label) {
        meta.values[options.count] = value;
        options.data[options.count++] = CopyText(label);
    };
    int selected = 0;
    const auto& text = *page.text;
    if (kind == RowKind::Technology) {
        add(0, text.original);
        for (size_t i = 1; i < page.available.size(); ++i)
            if (page.available[i]) add(static_cast<int>(i), vendor_names[i]);
        selected = static_cast<int>(page.draft.technology);
    } else if (kind == RowKind::Quality) {
        const std::wstring_view labels[] = {page.draft.technology == Technology::Dlss ? L"DLAA" : text.native,
            text.quality_value, text.balanced, text.performance, text.ultra, text.ultra_plus};
        for (const auto quality : quality_order) {
            const auto index = static_cast<size_t>(quality);
            if (quality == Quality::Native || page.resolutions[index].optimal.width) add(static_cast<int>(quality), labels[index]);
        }
        selected = static_cast<int>(page.draft.quality);
    } else if (kind == RowKind::Model) {
        for (size_t i = 0; i < text.model_names.size(); ++i) add(static_cast<int>(i), text.model_names[i]);
        selected = static_cast<int>(page.draft.model);
        AssignText(Field<GameString>(row, 0x20), text.model_help[selected]);
    } else {
        for (int percent = 0; percent <= sharpness_max; percent += sharpness_step)
            add(percent, FormatSharpnessValue(percent, text));
        selected = page.draft.sharpness;
    }
    Field<int>(row, 0x74) = options.count;
    const auto choice = std::find(meta.values.begin(), meta.values.begin() + options.count, selected);
    RefreshChoice(row, choice == meta.values.begin() + options.count ? 0 : static_cast<int>(choice - meta.values.begin()));
}

void RebuildQuality(MenuPage& page) {
    page.resolutions = {};
    if (page.draft.technology != Technology::Taa && page.output.width && page.output.height) {
        for (const auto quality : quality_order)
            page.resolutions[static_cast<size_t>(quality)] =
                RecommendMenuQuality(page.draft.technology, quality, page.output);
        if (!page.resolutions[static_cast<size_t>(page.draft.quality)].optimal.width)
            page.draft.quality = Quality::Native;
    }
    RebuildRow(page, RowKind::Quality);
}

const MenuText& GameLanguage();

void RefreshLanguage(MenuPage& page, const MenuText& text) {
    if (page.text == &text) return;
    page.text = &text;
    const wchar_t* titles[]{text.technology, text.quality, text.model, text.sharpness};
    const wchar_t* descriptions[]{text.technology_help, text.quality_help,
        text.model_help[static_cast<size_t>(page.draft.model)], text.sharpness_help};
    for (size_t i = 0; i < std::size(titles); ++i) {
        if (auto* row = page.rows[i]) {
            AssignText(Field<GameString>(row, 0x10), titles[i]);
            AssignText(Field<GameString>(row, 0x20), descriptions[i]);
            RebuildRow(page, static_cast<RowKind>(i));
        }
    }
}

void InitializePage(MenuPage& page) {
    const auto selection = ReadMenuGraphics();
    page.saved = page.draft = selection.current;
    page.available = selection.available;
    page.output = selection.output;
    RefreshLanguage(page, GameLanguage());
    RebuildRow(page, RowKind::Technology);
    RebuildQuality(page);
    RebuildRow(page, RowKind::Model);
    RebuildRow(page, RowKind::Sharpness);
    page.layout_dirty = page.scale_dirty = true;
}

void InitializeChoice(void* row) {
    auto& meta = rows.at(row);
    if (meta.kind == RowKind::Technology) InitializePage(*meta.page);
}

bool HasChanges(void* row) {
    const auto& meta = rows.at(row);
    return meta.page->saved != meta.page->draft;
}

void CancelChoice(void* row) {
    if (!HasChanges(row)) return;
    InitializePage(*rows.at(row).page);
    Log("Native graphics menu canceled pending changes.");
}

void ApplyOriginalRenderingScale(const MenuPage& page) {
    auto* scale = page.rows[Index(RowKind::Scale)];
    if (!scale || Field<int>(scale, 0x8) != rendering_scale_id) return;
    const int index = Field<int>(scale, 0x60);
    if (index < 0 || index > rendering_scale_max_index) return;

    // The native apply slot persists the index. The setter used by the stock
    // slider applies it to the renderer after the SDK override has been released.
    if (Field<int>(scale, 0x5c) != index) At<void (*)(void*)>(save_rendering_scale)(scale);
    if (auto* runtime = At<void* (*)()>(graphics_runtime)()) {
        At<void (*)(void*, int, bool)>(apply_rendering_scale)(runtime, index, true);
        Log("Reapplied native Rendering Scale index %d after switching to Original TAA.", index);
    }
}

void ApplyChoice(void* row) {
    if (!HasChanges(row)) return;
    auto& page = *rows.at(row).page;
    if (!ApplyMenuGraphics(page.draft)) {
        page.draft.technology = Technology::Taa;
        ApplyMenuGraphics(page.draft);
    }
    if (page.saved.technology != Technology::Taa && page.draft.technology == Technology::Taa)
        ApplyOriginalRenderingScale(page);
    Log("Native graphics menu applied technology %u, quality %u, model %u, Sharpness %d%%.",
        static_cast<unsigned>(page.draft.technology), static_cast<unsigned>(page.draft.quality),
        static_cast<unsigned>(page.draft.model), page.draft.sharpness);
    InitializePage(page);
}

void SetChoice(void* row, int index) {
    auto& meta = rows.at(row);
    if (!CanEdit(meta) || index < 0 || index >= Field<int>(row, 0x74)) return;
    RefreshChoice(row, index);
    auto& page = *meta.page;
    const int selected = meta.values[index];
    switch (meta.kind) {
    case RowKind::Technology:
        if (page.draft.technology == static_cast<Technology>(selected)) break;
        page.draft.technology = static_cast<Technology>(selected);
        RebuildQuality(page);
        page.layout_dirty = page.scale_dirty = true;
        break;
    case RowKind::Quality:
        if (page.draft.quality == static_cast<Quality>(selected)) break;
        page.draft.quality = static_cast<Quality>(selected);
        page.scale_dirty = true;
        break;
    case RowKind::Model:
        page.draft.model = static_cast<Model>(selected);
        AssignText(Field<GameString>(row, 0x20), page.text->model_help[selected]);
        break;
    case RowKind::Sharpness: page.draft.sharpness = selected; break;
    case RowKind::Scale: break;
    }
    Log("Native graphics menu staged row %zu, value %d.", Index(meta.kind), selected);
}

bool MoveChoice(void* row, bool right) {
    const auto& meta = rows.at(row);
    // A handled list step must not fall through to the game's generic
    // number editor, even when the selection is at one end.
    if (!CanEdit(meta)) return true;
    const int previous = Field<int>(row, 0x60);
    const int next = std::clamp(previous + (right ? 1 : -1), 0, Field<int>(row, 0x74) - 1);
    if (next != previous) {
        SetChoice(row, next);
        At<void (*)(int)>(0x1028730)(0);
    }
    return true;
}

void SlideChoice(void* row, float position) {
    const auto& meta = rows.at(row);
    if (meta.kind != RowKind::Sharpness) {
        At<void (*)(void*, float)>(0x1395610)(row, position);
        return;
    }
    if (!CanEdit(meta) || !std::isfinite(position)) return;
    const auto index = static_cast<int>(std::clamp(position, 0.0f, 1.0f) * sharpness_max_index + 0.5f);
    if (index != Field<int>(row, 0x60)) {
        SetChoice(row, index);
        At<void (*)(int)>(0x1028730)(0);
    }
}

bool Editable(void* row) {
    return CanEdit(rows.at(row));
}

void* Destroy(void* row, unsigned flags) {
    const auto entry = rows.find(row);
    if (entry != rows.end()) {
        auto page = entry->second.page;
        const auto kind = entry->second.kind;
        page->rows[Index(kind)] = nullptr;
        rows.erase(entry);
        if (kind == RowKind::Technology) {
            const auto active = pages.find(page->controller);
            if (active != pages.end() && active->second.lock() == page) pages.erase(active);
            const auto held = std::exchange(page->additions, {});
            for (size_t i = 1; i < held.size(); ++i) ReleaseRow(held[i]);
        }
    }
    using Delete = void* (*)(void*, unsigned);
    return At<Delete*>(rendering_scale_vtable)[0](row, flags);
}

template<unsigned Slot> bool MoveScale(void* row, bool right, void* controller) {
    using Move = bool (*)(void*, bool, void*);
    if (!Editable(row)) return true;
    return At<Move*>(rendering_scale_vtable)[Slot / 8](row, right, controller);
}

void SlideScale(void* row, float position) {
    if (Editable(row)) At<void (*)(void*, float)>(0x1395610)(row, position);
}

void SelectScale(void* row, int index) {
    if (Editable(row)) At<void (*)(void*, int)>(0x1392ab0)(row, index);
}

void ApplyScaleRow(void* row) {
    if (Editable(row)) {
        At<void (*)(void*)>(save_rendering_scale)(row);
        return;
    }
    // Discard a stock slider edit staged before the user selected an SDK.
    // The native setter refreshes the row value without committing it.
    At<void (*)(void*, int)>(0x1392ab0)(row, Field<int>(row, 0x5c));
}

const MenuText& GameLanguage() {
    GameString language{};
    At<GameString* (*)(GameString*)>(0x31bb880)(&language); // GetCurrentLanguage
    const auto& text = LocalizedMenu(language.data && language.count > 0 ?
        std::wstring_view(language.data, language.count - 1) : std::wstring_view());
    GameFree(language.data);
    return text;
}

SharedRow MakeRow(const std::shared_ptr<MenuPage>& page, RowKind kind, const wchar_t* title, const wchar_t* description) {
    const int capacity = kind == RowKind::Sharpness ? sharpness_max_index + 1 : 6;
    void* row = GameAllocate(row_size);
    auto* control = static_cast<SharedControl*>(GameAllocate(sizeof(SharedControl)));
    auto* options = static_cast<GameString*>(GameAllocate(capacity * sizeof(GameString)));
    if (!row || !control || !options) {
        GameFree(row);
        GameFree(control);
        GameFree(options);
        return {};
    }
    std::memset(row, 0, row_size);
    GameString name = CopyText(title);
    GameString help = CopyText(description);
    // The constructor copies and frees these two owning FString arguments.
    At<void* (*)(void*, int, GameString*, GameString*, int, uint8_t)>(0x1385b20)(
        row, technology_id + static_cast<int>(Index(kind)), &name, &help, -1, 4);
    Field<GameArray<GameString>>(row, 0xa0) = {options, 0, capacity};
    Field<void**>(row, 0) = row_functions.data() + 1;
    rows.emplace(row, MenuRow{page, kind});
    page->rows[Index(kind)] = row;
    *control = {image + shared_row_vtable, 1, 1, row};
    return {row, control};
}

struct GraphicsRows {
    std::array<SharedRow, max_graphics_rows> entries{};
    int count = 0;
};

constexpr GraphicsRows VisibleGraphicsRows(const Rows& current, const MenuPage& page) {
    GraphicsRows result;
    const auto visible = VisibleAdditions(page.draft.technology);
    const auto* scale = page.rows[Index(RowKind::Scale)];
    bool inserted = false;
    int stock = 0;
    for (int i = 0; i < current.count; ++i) {
        const auto entry = current.data[i];
        if (!entry.row) return {};
        if (std::any_of(page.additions.begin(), page.additions.end(),
                        [&](const auto& addition) { return addition.row == entry.row; })) continue;
        if (++stock > original_graphics_rows) return {};
        result.entries[result.count++] = entry;
        if (entry.row != scale) continue;
        if (inserted) return {};
        for (size_t slot = 0; slot < visible.size(); ++slot)
            if (visible[slot] && page.additions[slot].row) result.entries[result.count++] = page.additions[slot];
        inserted = true;
    }
    return inserted && stock == original_graphics_rows ? result : GraphicsRows{};
}

constexpr bool StableGraphicsOrder(Technology technology) {
    int stock[original_graphics_rows]{};
    int added[4]{};
    SharedRow previous[]{{&stock[0], nullptr}, {&stock[1], nullptr}, {&stock[2], nullptr}, {&stock[3], nullptr},
                         {&stock[4], nullptr}, {&stock[5], nullptr}, {&added[0], nullptr}, {&added[1], nullptr},
                         {&added[2], nullptr}, {&added[3], nullptr}};
    MenuPage page{};
    page.draft.technology = technology;
    page.rows[Index(RowKind::Scale)] = &stock[1];
    for (size_t i = 0; i < page.additions.size(); ++i) page.additions[i].row = &added[i];

    auto current = VisibleGraphicsRows({previous, max_graphics_rows, max_graphics_rows}, page);
    int index = 0;
    for (int i = 0; i < original_graphics_rows; ++i) {
        if (current.entries[index++].row != &stock[i]) return false;
        if (i != 1) continue;
        const auto visible = VisibleAdditions(technology);
        for (size_t j = 0; j < visible.size(); ++j)
            if (visible[j] && current.entries[index++].row != &added[j]) return false;
    }
    if (index != current.count) return false;
    const auto repeat = VisibleGraphicsRows({current.entries.data(), current.count, current.count}, page);
    if (repeat.count != current.count) return false;
    for (int i = 0; i < repeat.count; ++i)
        if (repeat.entries[i].row != current.entries[i].row) return false;
    return true;
}

static_assert(StableGraphicsOrder(Technology::Taa) && StableGraphicsOrder(Technology::Dlss) &&
              StableGraphicsOrder(Technology::Fsr) && StableGraphicsOrder(Technology::Xess));

bool SameRows(const Rows& a, const Rows& b) {
    return a.count == b.count && std::equal(a.data, a.data + a.count, b.data,
        [](const auto& left, const auto& right) { return left.row == right.row && left.control == right.control; });
}

void CopyRows(Rows& target, const Rows& source) {
    // The game's TArray assignment owns the reference changes for both lists.
    At<void (*)(Rows*, const Rows*)>(0xb70bf0)(&target, &source);
}

std::byte* GraphicsNavigation(void* controller) {
    auto* navigation = Field<std::byte*>(controller, controller_navigation);
    return navigation ? navigation + graphics_navigation : nullptr;
}

void SetRowCount(void* navigation, int count) {
    // Preserve the game's horizontal key bindings and visible-row limit.
    At<void (*)(void*, int)>(0x10bed80)(navigation, count);
}

constexpr int VisibleTop(int previous, int focus, int count, int visible) {
    const int top = std::clamp(previous, 0, count - visible);
    if (focus < top) return focus;
    if (focus >= top + visible) return focus - visible + 1;
    return top;
}

static_assert(VisibleTop(2, 2, 7, 7) == 0); // The final two rows were hidden.
static_assert(VisibleTop(0, 8, 9, 7) == 2); // A stock row moved down.
static_assert(VisibleTop(2, 1, 9, 7) == 1); // Keep a selection visible above the old top.

void RefreshActiveRows(MenuPage& page, Rows& active, const Rows& desired, std::byte* navigation) {
    constexpr unsigned rows_on_screen = 0x8;
    constexpr unsigned relative_row = 0x24;
    constexpr unsigned scroll_top = 0x28;
    auto* controller = page.controller;
    auto* draw = Field<void*>(controller, controller_draw_actor);
    const auto* draw_rows = draw ? &Field<Rows>(draw, 0x330) : nullptr;
    const bool draw_has_active_rows = draw_rows && Valid(*draw_rows, 64) && SameRows(*draw_rows, active);
    const int previous_top = Field<int>(navigation, scroll_top);
    const int previous_row = std::clamp(Field<int>(controller, 0x338), 0, active.count - 1);
    const auto* selected = active.data[previous_row].row;
    const auto find_row = [&](const void* row) {
        return std::find_if(desired.data, desired.data + desired.count,
                            [&](const auto& entry) { return entry.row == row; });
    };
    auto* next = find_row(selected);
    if (next == desired.data + desired.count) next = find_row(page.additions[0].row);
    const int focus = static_cast<int>(next - desired.data);
    CopyRows(active, desired);
    SetRowCount(navigation, active.count);
    const int visible = std::clamp(Field<int>(navigation, rows_on_screen), 1, active.count);
    const int top = VisibleTop(previous_top, focus, active.count, visible);
    const int relative = focus - top;
    Field<int>(navigation, scroll_top) = top;
    Field<int>(navigation, relative_row) = relative;
    Field<int>(controller, 0x330) = relative;
    Field<int>(controller, 0x334) = top;
    Field<int>(controller, 0x338) = focus;
    if (draw_has_active_rows) {
        At<void (*)(void*, const Rows*)>(0x13b8080)(draw, &active);
        At<void (*)(void*, int, int, int)>(0x13b7a50)(draw, relative, top, Field<uint8_t>(controller, 0x39d));
    }
}

bool RefreshLayout(MenuPage& page, Rows& graphics) {
    auto desired_rows = VisibleGraphicsRows(graphics, page);
    auto* navigation = GraphicsNavigation(page.controller);
    if (!desired_rows.count || !navigation) return false;
    const Rows desired{desired_rows.entries.data(), desired_rows.count, desired_rows.count};
    page.layout_dirty = false;
    if (SameRows(graphics, desired)) return true;
    auto& active = Field<Rows>(page.controller, controller_rows);
    const bool showing_graphics = Field<void*>(page.controller, controller_active_navigation) == navigation &&
                                  Valid(active, 64) && SameRows(active, graphics);
    // The new order borrows pointers from the arrays it replaces. Retain
    // those rows even if the native assignment releases its old array first.
    for (int i = 0; i < desired.count; ++i)
        if (auto* control = desired.data[i].control) ++control->shared;
    CopyRows(graphics, desired);
    if (showing_graphics) RefreshActiveRows(page, active, desired, navigation);
    else SetRowCount(navigation, graphics.count);
    for (int i = 0; i < desired.count; ++i) ReleaseRow(desired.data[i]);
    Log("Native graphics menu contains %d visible rows.", graphics.count);
    return true;
}

void RefreshScaleDisplay(MenuPage& page, const Rows& graphics) {
    auto* controller = page.controller;
    auto* navigation = GraphicsNavigation(controller);
    if (!navigation || Field<void*>(controller, controller_active_navigation) != navigation) return;
    const auto& active = Field<Rows>(controller, controller_rows);
    auto* draw = Field<void*>(controller, controller_draw_actor);
    if (!draw || !Valid(active, 64) || !SameRows(active, graphics)) return;
    const auto& draw_rows = Field<Rows>(draw, 0x330);
    if (!Valid(draw_rows, 64) || !SameRows(draw_rows, active)) return;
    // The native controller uses this Blueprint event to refresh at the same cursor and scroll position.
    page.scale_dirty = false;
    At<bool (*)(void*, int, int)>(draw_cursor_changed)(draw, Field<int>(controller, 0x330),
                                                      Field<int>(controller, 0x334));
}

void UpdatePage(void* controller, bool check_language = false) {
    const auto item = pages.find(controller);
    if (item == pages.end()) return;
    const auto page = item->second.lock();
    if (!page) return;
    const auto& groups = Field<Groups>(controller, controller_groups);
    if (!Valid(groups, 64) || page->group_index >= groups.count) return;
    auto& graphics = groups.data[page->group_index];
    if (!Valid(graphics, 64) || graphics.count > max_graphics_rows ||
        std::none_of(graphics.data, graphics.data + graphics.count,
                     [&](const auto& entry) { return entry.row == page->additions[0].row; })) return;
    if (page->layout_dirty && !RefreshLayout(*page, graphics)) return;
    if (page->scale_dirty) RefreshScaleDisplay(*page, graphics);
    if (check_language && GetTickCount64() >= page->next_language_check) {
        page->next_language_check = GetTickCount64() + 500;
        RefreshLanguage(*page, GameLanguage());
    }
}

void TickMenu(std::byte* state, int phase, float delta) {
    auto* controller = state - state_in_controller;
    UpdatePage(controller, true);
    original_menu_tick(state, phase, delta);
    UpdatePage(controller);
}

void StepList(void* controller, bool right) {
    original_list_step(controller, right);
    // Native keyboard, pad and mouse arrows converge here. The engine has
    // released its borrowed list slot before we can replace either array.
    UpdatePage(controller);
}

void GraphicsFactory(Groups* groups, void* definitions, void* controller) {
    const int previous = groups->count;
    original_factory(groups, definitions, controller);
    if (!controller || previous < 0 || groups != &Field<Groups>(controller, controller_groups) ||
        !Valid(*groups, 64) || groups->count != previous + 1) return;
    auto& graphics = groups->data[previous];
    if (!Valid(graphics, 64) || graphics.count != original_graphics_rows) return;
    auto* scale = graphics.data[1].row;
    if (!scale || Field<int>(scale, 0x8) != rendering_scale_id ||
        Field<void*>(scale, 0) != image + rendering_scale_vtable) return;
    if (!GraphicsNavigation(controller)) return;
    const auto selection = ReadMenuGraphics();
    const auto& text = GameLanguage();
    auto page = std::make_shared<MenuPage>();
    page->saved = page->draft = selection.current;
    page->available = selection.available;
    page->output = selection.output;
    page->text = &text;
    page->controller = controller;
    page->group_index = previous;
    const auto choice = MakeRow(page, RowKind::Technology, text.technology, text.technology_help);
    if (!choice.row) return;
    page->additions[Index(RowKind::Technology)] = choice;
    page->additions[Index(RowKind::Quality)] = MakeRow(page, RowKind::Quality, text.quality, text.quality_help);
    page->additions[Index(RowKind::Model)] = MakeRow(page, RowKind::Model, text.model,
        text.model_help[static_cast<size_t>(page->draft.model)]);
    page->additions[Index(RowKind::Sharpness)] = MakeRow(page, RowKind::Sharpness, text.sharpness, text.sharpness_help);
    page->rows[Index(RowKind::Scale)] = scale;
    InitializeChoice(choice.row);
    if (RefreshLayout(*page, graphics)) {
        rows.emplace(scale, MenuRow{page, RowKind::Scale});
        Field<void**>(scale, 0) = scale_functions.data() + 1;
        pages.insert_or_assign(controller, page);
    }
    ReleaseRow(choice); // The graphics list now holds Technology's native reference.
}

constexpr double MenuScalePercent(Quality quality, Size render, Size output) {
    return quality == Quality::Native ? 100.0 : RenderScalePercent(render, output);
}

static_assert(MenuScalePercent(Quality::Native, {1440, 810}, {1920, 1080}) == 100);
static_assert(MenuScalePercent(Quality::Quality, {1280, 720}, {1920, 1080}) > 66.66);
static_assert(MenuScalePercent(Quality::Quality, {1280, 720}, {1920, 1080}) < 66.67);
static_assert(MenuScalePercent(Quality::Balanced, {}, {1920, 1080}) == 0);

std::wstring FormatScale(double percent, std::wstring_view language) {
    if (percent <= 0) return L"—";
    wchar_t value[16]{};
    const auto* format = percent == static_cast<int>(percent) ? L"%.0f%%" : L"%.2f%%";
    std::swprintf(value, std::size(value), format, percent);
    if (language != L"en" && language != L"ja" && language != L"ko" && !language.starts_with(L"zh")) {
        if (auto* separator = std::wcschr(value, L'.')) *separator = L',';
    }
    return value;
}

void* BuildDrawParam(void* output, SharedRow* source) {
    const auto entry = source ? rows.find(source->row) : rows.end();
    if (entry == rows.end()) return original_draw_param(output, source);
    const auto meta = entry->second;
    auto* result = original_draw_param(output, source);
    if (meta.kind != RowKind::Scale) {
        // Rendering Scale's native widget subtype. The draw data already
        // carries the current index, last index, and formatted value.
        Field<uint8_t>(result, 0x2e) = 4;
        return result;
    }
    if (CanEdit(meta)) return result;
    Field<uint8_t>(result, 0x2d) = 0;
    Field<int>(result, 0x28) = 0;
    Field<int>(result, 0x30) = 0;
    auto& page = *meta.page;
    const Size output_size = ReadMenuGraphics().output;
    if (output_size != page.output) {
        page.output = output_size;
        RebuildQuality(page);
    }
    const auto quality = page.draft.quality;
    const auto target_size = page.resolutions[static_cast<size_t>(quality)].optimal;
    const auto target = FormatScale(MenuScalePercent(quality, target_size, output_size), page.text->language);
    AssignText(Field<GameString>(result, 0x38), target);
    AssignText(Field<GameString>(result, 0x18), std::wstring(page.text->scale_target) + target);
    return result;
}

bool SupportedImage() {
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp != 0x69ce3439 ||
        nt->OptionalHeader.SizeOfImage != 0x16376000) return false;
    const unsigned char factory_prefix[]{0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10};
    const unsigned char draw_prefix[]{0x48,0x89,0x5c,0x24,0x18,0x55,0x56,0x57,0x41,0x54};
    const unsigned char tick_prefix[]{0x40,0x55,0x56,0x41,0x56,0x48,0x8d,0x6c,0x24,0xb9};
    const unsigned char step_prefix[]{0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
    const unsigned char cursor_prefix[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x30};
    return std::memcmp(image + graphics_factory, factory_prefix, sizeof(factory_prefix)) == 0 &&
           std::memcmp(image + build_draw_param, draw_prefix, sizeof(draw_prefix)) == 0 &&
           std::memcmp(image + menu_tick, tick_prefix, sizeof(tick_prefix)) == 0 &&
           std::memcmp(image + list_step, step_prefix, sizeof(step_prefix)) == 0 &&
           std::memcmp(image + draw_cursor_changed, cursor_prefix, sizeof(cursor_prefix)) == 0;
}
}

void InitializeNativeMenu() {
    static bool attempted = false;
    if (attempted) return;
    attempted = true;
    image = reinterpret_cast<std::byte*>(GetModuleHandleW(nullptr));
    if (!image || !SupportedImage()) {
        Log("Native graphics menu unavailable for this executable.");
        return;
    }
    // Preserve the relocated native vtable and its preceding entry.
    std::memcpy(row_functions.data(), image + rendering_scale_vtable - sizeof(void*), sizeof(row_functions));
    scale_functions = row_functions;
    auto** slots = row_functions.data() + 1;
    slots[0x00 / 8] = reinterpret_cast<void*>(&Destroy);
    slots[0x08 / 8] = reinterpret_cast<void*>(&SetChoice);
    slots[0x10 / 8] = reinterpret_cast<void*>(&HasChanges);
    slots[0x18 / 8] = reinterpret_cast<void*>(&InitializeChoice);
    slots[0x20 / 8] = reinterpret_cast<void*>(&CancelChoice);
    slots[0x28 / 8] = reinterpret_cast<void*>(&ApplyChoice);
    slots[0x38 / 8] = reinterpret_cast<void*>(&Editable);
    slots[0x50 / 8] = reinterpret_cast<void*>(&SlideChoice);
    slots[0x70 / 8] = reinterpret_cast<void*>(&MoveChoice);
    slots[0x80 / 8] = reinterpret_cast<void*>(&MoveChoice);
    slots[0x88 / 8] = reinterpret_cast<void*>(&SetChoice);

    auto** scale = scale_functions.data() + 1;
    scale[0x00 / 8] = reinterpret_cast<void*>(&Destroy);
    scale[0x28 / 8] = reinterpret_cast<void*>(&ApplyScaleRow);
    scale[0x38 / 8] = reinterpret_cast<void*>(&Editable);
    scale[0x50 / 8] = reinterpret_cast<void*>(&SlideScale);
    scale[0x70 / 8] = reinterpret_cast<void*>(&MoveScale<0x70>);
    scale[0x80 / 8] = reinterpret_cast<void*>(&MoveScale<0x80>);
    scale[0x88 / 8] = reinterpret_cast<void*>(&SelectScale);

    const struct { void* address; void* detour; void** original; } hooks[]{
        {At<void*>(build_draw_param), reinterpret_cast<void*>(&BuildDrawParam), reinterpret_cast<void**>(&original_draw_param)},
        {At<void*>(menu_tick), reinterpret_cast<void*>(&TickMenu), reinterpret_cast<void**>(&original_menu_tick)},
        {At<void*>(list_step), reinterpret_cast<void*>(&StepList), reinterpret_cast<void**>(&original_list_step)},
        {At<void*>(graphics_factory), reinterpret_cast<void*>(&GraphicsFactory), reinterpret_cast<void**>(&original_factory)},
    };
    size_t created = 0;
    bool queued = true;
    for (const auto& hook : hooks) {
        if (MH_CreateHook(hook.address, hook.detour, hook.original) != MH_OK) break;
        ++created;
        if (MH_QueueEnableHook(hook.address) != MH_OK) {
            queued = false;
            break;
        }
    }
    if (!queued || created != std::size(hooks) || MH_ApplyQueued() != MH_OK) {
        while (created) MH_RemoveHook(hooks[--created].address);
        Log("Native graphics menu hooks could not be installed.");
        return;
    }
    Log("Native graphics menu hooks installed for Steam build 22672075.");
}
}
