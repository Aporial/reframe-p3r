#pragma once

#include <array>
#include <string_view>

namespace reframe {
struct MenuText {
    std::wstring_view language;
    const wchar_t* technology;
    const wchar_t* technology_help;
    const wchar_t* original;
    const wchar_t* quality;
    const wchar_t* quality_help;
    const wchar_t* native;
    const wchar_t* ultra_plus;
    const wchar_t* ultra;
    const wchar_t* quality_value;
    const wchar_t* balanced;
    const wchar_t* performance;
    const wchar_t* model;
    std::array<const wchar_t*, 4> model_names;
    std::array<const wchar_t*, 4> model_help;
    const wchar_t* scale_target;
};

inline constexpr MenuText menu_text[] = {
    {L"en", L"Anti-Aliasing Technology", L"Use the game's TAA or temporal anti-aliasing and upscaling with DLSS, FSR or XeSS.",
     L"Original TAA", L"Quality Mode", L"Native AA uses full resolution. Lower quality modes render fewer pixels for higher performance.",
     L"Native AA", L"Ultra Quality Plus", L"Ultra Quality", L"Quality", L"Balanced", L"Performance",
     L"DLSS Model",
     {L"Auto (Recommended)", L"DLSS 4 — Standard (K)", L"DLSS 4.5 — Optimized (M)", L"DLSS 4.5 — Detail (L)"},
     {L"Uses the recommended model for this quality mode.",
      L"The standard model for DLAA, Quality and Balanced. Lower GPU load than M and L.",
      L"Tuned for Performance. Higher GPU load on RTX 20 and 30.",
      L"Tuned for 4K Ultra Performance. Higher GPU load on RTX 20 and 30."},
     L"SDK target: "},
    {L"ru", L"Технология сглаживания", L"Используйте TAA игры или временное сглаживание и масштабирование DLSS, FSR или XeSS.",
     L"Оригинальный TAA", L"Режим качества", L"Нативное сглаживание использует полное разрешение. Другие режимы снижают разрешение рендера для повышения FPS.",
     L"Нативное сглаживание", L"Ультракачество+", L"Ультракачество", L"Качество", L"Баланс", L"Производительность",
     L"Модель DLSS",
     {L"Автоматически (рекомендуется)", L"DLSS 4 — стандартная (K)", L"DLSS 4.5 — оптимизированная (M)", L"DLSS 4.5 — детальная (L)"},
     {L"Выбирает рекомендуемую модель для текущего режима качества.",
      L"Стандарт для DLAA, качества и баланса. Нагрузка ниже, чем у M и L.",
      L"Для режима производительности. На RTX 20 и 30 повышает нагрузку.",
      L"Для ультрапроизводительности в 4K. На RTX 20 и 30 повышает нагрузку."},
     L"Целевой масштаб SDK: "},
    {L"ja", L"アンチエイリアス方式", L"ゲーム標準のTAA、またはDLSS、FSR、XeSSによる補正とアップスケーリングを選びます。",
     L"標準TAA", L"品質モード", L"ネイティブAAは等倍で描画します。品質を下げると描画負荷を軽減できます。",
     L"ネイティブAA", L"最高品質+", L"最高品質", L"品質", L"バランス", L"パフォーマンス",
     L"DLSSモデル",
     {L"自動（推奨）", L"DLSS 4 — 標準 (K)", L"DLSS 4.5 — 最適化 (M)", L"DLSS 4.5 — 精細 (L)"},
     {L"品質モードに応じた推奨モデルを使います。",
      L"DLAA、品質、バランス向けの標準モデルです。MやLより負荷を抑えます。",
      L"パフォーマンス向けです。RTX 20・30シリーズでは負荷が増えます。",
      L"4Kのウルトラパフォーマンス向けです。RTX 20・30シリーズでは負荷が増えます。"},
     L"SDK の目標描画倍率："},
    {L"ko", L"안티앨리어싱 방식", L"게임의 기본 TAA 또는 DLSS, FSR, XeSS의 안티앨리어싱과 업스케일링을 선택합니다.",
     L"기본 TAA", L"품질 모드", L"네이티브 AA는 원래 해상도를 사용합니다. 낮은 품질 모드는 렌더링 부하를 줄입니다.",
     L"네이티브 AA", L"울트라 품질+", L"울트라 품질", L"품질", L"균형", L"성능",
     L"DLSS 모델",
     {L"자동 (권장)", L"DLSS 4 — 표준 (K)", L"DLSS 4.5 — 최적화 (M)", L"DLSS 4.5 — 디테일 (L)"},
     {L"현재 품질 모드의 권장 모델을 사용합니다.",
      L"DLAA, 품질, 균형 모드의 표준 모델입니다. M과 L보다 부하가 낮습니다.",
      L"성능 모드에 최적화되었습니다. RTX 20 및 30 시리즈에서 부하가 늘어납니다.",
      L"4K 울트라 성능에 최적화되었습니다. RTX 20 및 30 시리즈에서 부하가 늘어납니다."},
     L"SDK 목표 렌더링 배율: "},
    {L"zh-Hans", L"抗锯齿技术", L"选择游戏原生 TAA，或使用 DLSS、FSR、XeSS 的抗锯齿和超分辨率。",
     L"原生 TAA", L"画质模式", L"原生抗锯齿使用完整分辨率。较低的画质模式可以提高性能。",
     L"原生抗锯齿", L"超高画质+", L"超高画质", L"画质", L"平衡", L"性能",
     L"DLSS 模型",
     {L"自动（推荐）", L"DLSS 4 — 标准 (K)", L"DLSS 4.5 — 优化 (M)", L"DLSS 4.5 — 细节 (L)"},
     {L"使用当前画质模式的推荐模型。",
      L"适用于 DLAA、画质和平衡模式，比 M 和 L 的显卡负载低。",
      L"针对性能模式优化。在 RTX 20 和 30 系列上负载更高。",
      L"针对 4K 超级性能模式优化。在 RTX 20 和 30 系列上负载更高。"},
     L"SDK 目标渲染比例："},
    {L"zh-Hant", L"反鋸齒技術", L"選擇遊戲原生 TAA，或使用 DLSS、FSR、XeSS 的反鋸齒與超解析度。",
     L"原生 TAA", L"畫質模式", L"原生反鋸齒使用完整解析度。較低的畫質模式可提升效能。",
     L"原生反鋸齒", L"超高畫質+", L"超高畫質", L"畫質", L"平衡", L"效能",
     L"DLSS 模型",
     {L"自動（建議）", L"DLSS 4 — 標準 (K)", L"DLSS 4.5 — 最佳化 (M)", L"DLSS 4.5 — 細節 (L)"},
     {L"使用目前畫質模式的建議模型。",
      L"適用於 DLAA、畫質和平衡模式，比 M 和 L 的顯示卡負載低。",
      L"針對效能模式最佳化。在 RTX 20 和 30 系列上負載較高。",
      L"針對 4K 超高效能模式最佳化。在 RTX 20 和 30 系列上負載較高。"},
     L"SDK 目標渲染比例："},
    {L"fr", L"Anticrénelage", L"Utilisez le TAA du jeu ou l’anticrénelage et la mise à l’échelle de DLSS, FSR ou XeSS.",
     L"TAA d’origine", L"Mode de qualité", L"L’AA natif utilise la pleine résolution. Les autres modes peuvent améliorer les performances.",
     L"AA natif", L"Ultra qualité +", L"Ultra qualité", L"Qualité", L"Équilibré", L"Performances",
     L"Modèle DLSS",
     {L"Auto (recommandé)", L"DLSS 4 — Standard (K)", L"DLSS 4.5 — Optimisé (M)", L"DLSS 4.5 — Détail (L)"},
     {L"Utilise le modèle recommandé pour la qualité choisie.",
      L"Modèle standard pour DLAA, Qualité et Équilibré. Charge GPU inférieure à M et L.",
      L"Optimisé pour le mode Performances. Charge GPU accrue sur RTX 20 et 30.",
      L"Optimisé pour Ultra performances en 4K. Charge GPU accrue sur RTX 20 et 30."},
     L"Échelle cible du SDK : "},
    {L"it", L"Antialiasing", L"Usa il TAA del gioco oppure l’antialiasing e l’upscaling di DLSS, FSR o XeSS.",
     L"TAA originale", L"Modalità qualità", L"L’AA nativo usa la risoluzione completa. Le altre modalità possono migliorare le prestazioni.",
     L"AA nativo", L"Qualità ultra+", L"Qualità ultra", L"Qualità", L"Bilanciato", L"Prestazioni",
     L"Modello DLSS",
     {L"Auto (consigliato)", L"DLSS 4 — Standard (K)", L"DLSS 4.5 — Ottimizzato (M)", L"DLSS 4.5 — Dettaglio (L)"},
     {L"Usa il modello consigliato per la modalità selezionata.",
      L"Standard per DLAA, Qualità e Bilanciato. Carico GPU inferiore a M e L.",
      L"Ottimizzato per Prestazioni. Carico GPU maggiore su RTX serie 20 e 30.",
      L"Ottimizzato per Ultra prestazioni in 4K. Carico GPU maggiore su RTX serie 20 e 30."},
     L"Scala obiettivo dell’SDK: "},
    {L"de", L"Kantenglättung", L"Verwendet das TAA des Spiels oder Kantenglättung und Hochskalierung mit DLSS, FSR oder XeSS.",
     L"Original-TAA", L"Qualitätsmodus", L"Native Kantenglättung nutzt die volle Auflösung. Andere Modi können die Leistung verbessern.",
     L"Natives AA", L"Ultraqualität+", L"Ultraqualität", L"Qualität", L"Ausgewogen", L"Leistung",
     L"DLSS-Modell",
     {L"Auto (empfohlen)", L"DLSS 4 — Standard (K)", L"DLSS 4.5 — Optimiert (M)", L"DLSS 4.5 — Details (L)"},
     {L"Verwendet das für den Qualitätsmodus empfohlene Modell.",
      L"Standard für DLAA, Qualität und Ausgewogen. Geringere GPU-Last als M und L.",
      L"Für Leistung optimiert. Höhere GPU-Last auf RTX 20 und 30.",
      L"Für Ultra-Leistung in 4K optimiert. Höhere GPU-Last auf RTX 20 und 30."},
     L"SDK-Zielskalierung: "},
    {L"es", L"Suavizado de bordes", L"Usa el TAA del juego o el suavizado y escalado de DLSS, FSR o XeSS.",
     L"TAA original", L"Modo de calidad", L"El AA nativo usa la resolución completa. Los otros modos pueden mejorar el rendimiento.",
     L"AA nativo", L"Calidad ultra+", L"Calidad ultra", L"Calidad", L"Equilibrado", L"Rendimiento",
     L"Modelo DLSS",
     {L"Auto (recomendado)", L"DLSS 4 — Estándar (K)", L"DLSS 4.5 — Optimizado (M)", L"DLSS 4.5 — Detalle (L)"},
     {L"Usa el modelo recomendado para el modo de calidad.",
      L"Estándar para DLAA, Calidad y Equilibrado. Menor carga gráfica que M y L.",
      L"Optimizado para Rendimiento. Mayor carga gráfica en RTX 20 y 30.",
      L"Optimizado para Rendimiento ultra en 4K. Mayor carga gráfica en RTX 20 y 30."},
     L"Escala objetivo del SDK: "},
    {L"pt", L"Antisserrilhamento", L"Use o TAA do jogo ou o antisserrilhamento e a super-resolução do DLSS, FSR ou XeSS.",
     L"TAA original", L"Modo de qualidade", L"O AA nativo usa a resolução completa. Os outros modos podem melhorar o desempenho.",
     L"AA nativo", L"Qualidade ultra+", L"Qualidade ultra", L"Qualidade", L"Equilibrado", L"Desempenho",
     L"Modelo DLSS",
     {L"Auto (recomendado)", L"DLSS 4 — Padrão (K)", L"DLSS 4.5 — Otimizado (M)", L"DLSS 4.5 — Detalhe (L)"},
     {L"Usa o modelo recomendado para o modo de qualidade.",
      L"Padrão para DLAA, Qualidade e Equilibrado. Menor carga na GPU que M e L.",
      L"Otimizado para Desempenho. Maior carga na GPU nas séries RTX 20 e 30.",
      L"Otimizado para Ultra desempenho em 4K. Maior carga nas séries RTX 20 e 30."},
     L"Escala alvo do SDK: "},
    {L"pl", L"Technologia wygładzania", L"Użyj TAA gry albo wygładzania i skalowania DLSS, FSR lub XeSS.",
     L"Oryginalne TAA", L"Tryb jakości", L"Natywne AA działa w pełnej rozdzielczości. Pozostałe tryby mogą zwiększyć wydajność.",
     L"Natywne AA", L"Jakość ultra+", L"Jakość ultra", L"Jakość", L"Zrównoważony", L"Wydajność",
     L"Model DLSS",
     {L"Auto (zalecane)", L"DLSS 4 — Standard (K)", L"DLSS 4.5 — Optymalny (M)", L"DLSS 4.5 — Szczegóły (L)"},
     {L"Używa modelu zalecanego dla wybranego trybu jakości.",
      L"Standard dla DLAA, Jakości i trybu zrównoważonego. Mniej obciąża GPU niż M i L.",
      L"Zoptymalizowany pod kątem wydajności. Większe obciążenie kart RTX 20 i 30.",
      L"Dla ultrawydajności w 4K. Większe obciążenie kart RTX 20 i 30."},
     L"Docelowa skala SDK: "},
    {L"tr", L"Kenar yumuşatma", L"Oyunun TAA yöntemini veya DLSS, FSR ya da XeSS ile kenar yumuşatma ve ölçeklendirmeyi kullan.",
     L"Orijinal TAA", L"Kalite modu", L"Yerel AA tam çözünürlüğü kullanır. Diğer modlar performansı artırabilir.",
     L"Yerel AA", L"Ultra kalite+", L"Ultra kalite", L"Kalite", L"Dengeli", L"Performans",
     L"DLSS modeli",
     {L"Otomatik (önerilen)", L"DLSS 4 — Standart (K)", L"DLSS 4.5 — Optimize (M)", L"DLSS 4.5 — Ayrıntı (L)"},
     {L"Seçili kalite modu için önerilen modeli kullanır.",
      L"DLAA, Kalite ve Dengeli modları için standart. M ve L'den daha az GPU yükü.",
      L"Performans için optimize edildi. RTX 20 ve 30 serisinde GPU yükü artar.",
      L"4K Ultra Performans için optimize edildi. RTX 20 ve 30 serisinde GPU yükü artar."},
     L"SDK hedef ölçeği: "},
};

constexpr wchar_t LanguageChar(wchar_t c) {
    if (c >= L'A' && c <= L'Z') return c + (L'a' - L'A');
    return c == L'_' ? L'-' : c;
}

constexpr bool MatchesLanguage(std::wstring_view language, std::wstring_view prefix) {
    if (language.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i)
        if (LanguageChar(language[i]) != LanguageChar(prefix[i])) return false;
    return language.size() == prefix.size() || LanguageChar(language[prefix.size()]) == L'-';
}

constexpr const MenuText& LocalizedMenu(std::wstring_view language) {
    if (MatchesLanguage(language, L"zh") &&
        !MatchesLanguage(language, L"zh-Hans") && !MatchesLanguage(language, L"zh-Hant")) {
        const bool traditional = MatchesLanguage(language, L"zh-TW") || MatchesLanguage(language, L"zh-HK") ||
                                 MatchesLanguage(language, L"zh-MO") || MatchesLanguage(language, L"zh-CHT");
        language = traditional ? L"zh-Hant" : L"zh-Hans";
    }
    for (const auto& entry : menu_text) {
        if (MatchesLanguage(language, entry.language)) return entry;
    }
    return menu_text[0];
}

constexpr bool ValidMenuText(const wchar_t* text) {
    if (!text || !*text) return false;
    for (size_t i = 0; text[i]; ++i) {
        const auto c = static_cast<unsigned>(text[i]);
        if (c == 0xfffd || c > 0x10ffff || (c >= 0xdc00 && c <= 0xdfff)) return false;
        if (c >= 0xd800 && c <= 0xdbff) {
            const auto low = static_cast<unsigned>(text[++i]);
            if (low < 0xdc00 || low > 0xdfff) return false;
        }
    }
    return true;
}

constexpr bool CompleteMenuTranslations() {
    constexpr std::wstring_view supported[] = {
        L"en-US", L"ru-RU", L"ja-JP", L"ko-KR", L"zh-CN", L"zh-TW", L"fr-FR",
        L"it-IT", L"de-DE", L"es-ES", L"pt-BR", L"pl-PL", L"tr-TR"
    };
    if constexpr (std::size(supported) != std::size(menu_text)) return false;
    for (size_t i = 0; i < std::size(supported); ++i) {
        const auto& text = menu_text[i];
        if (&LocalizedMenu(supported[i]) != &text) return false;
        const wchar_t* fields[]{text.technology, text.technology_help, text.original, text.quality,
            text.quality_help, text.native, text.ultra_plus, text.ultra, text.quality_value, text.balanced,
            text.performance, text.model, text.scale_target};
        for (auto* field : fields)
            if (!ValidMenuText(field)) return false;
        constexpr std::wstring_view suffixes[]{L"", L"(K)", L"(M)", L"(L)"};
        for (size_t model = 0; model < text.model_names.size(); ++model) {
            if (!ValidMenuText(text.model_names[model]) || !ValidMenuText(text.model_help[model]) ||
                !std::wstring_view(text.model_names[model]).ends_with(suffixes[model])) return false;
        }
    }
    return true;
}

static_assert(CompleteMenuTranslations());
static_assert(LocalizedMenu(L"zh_HANT_HK").language == L"zh-Hant");
static_assert(LocalizedMenu(L"zh-Hans-CN").language == L"zh-Hans");
static_assert(LocalizedMenu(L"pt_BR").language == L"pt");
static_assert(LocalizedMenu(L"es").language == L"es");
static_assert(LocalizedMenu(L"unknown").language == L"en");
static_assert(LocalizedMenu(L"").language == L"en");
}
