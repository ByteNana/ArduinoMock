#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#define MALLOC_CAP_SPIRAM (1 << 10)
#define MALLOC_CAP_8BIT (1 << 2)
#define MALLOC_CAP_32BIT (1 << 1)
#define MALLOC_CAP_DEFAULT (1 << 3)

inline void* ps_malloc(size_t size) { return malloc(size); }
inline void* heap_caps_malloc(size_t size, uint32_t) { return malloc(size); }
inline void* heap_caps_calloc(size_t n, size_t size, uint32_t) { return calloc(n, size); }
inline void* heap_caps_realloc(void* ptr, size_t size, uint32_t) { return realloc(ptr, size); }
inline void heap_caps_free(void* ptr) { free(ptr); }
inline size_t heap_caps_get_free_size(uint32_t) { return 100000; }
inline bool psramFound(void) { return false; }