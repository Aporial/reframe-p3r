#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct D3d12Slots {
    size_t compute_pipeline;
    size_t create_list;
    size_t heap;
    size_t root_signature;
    size_t cbv;
    size_t srv;
    size_t uav;
    size_t copy;
    size_t copy_simple;
    size_t committed;
    size_t placed;
    size_t command_signature;
    size_t create_library;
    size_t pipeline_stream;
    size_t create_list1;
    size_t committed1;
    size_t committed2;
    size_t placed1;
    size_t library_compute;
    size_t library_stream;
    size_t reset;
    size_t clear;
    size_t dispatch;
    size_t pipeline;
    size_t heaps;
    size_t compute_root;
    size_t graphics_root;
    size_t compute_table;
    size_t graphics_table;
    size_t compute_cbv;
    size_t indirect;
    size_t factory_device;
    size_t sdk_factory;
    size_t create_queue;
    size_t execute_lists;
} D3d12Slots;

extern const D3d12Slots d3d12_slots;

#ifdef __cplusplus
}
#endif
