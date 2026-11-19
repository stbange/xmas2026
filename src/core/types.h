/* SPDX-License-Identifier: MIT */
#ifndef TB_CORE_TYPES_H
#define TB_CORE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TB_ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define TB_MIN(a, b)      ((a) < (b) ? (a) : (b))
#define TB_MAX(a, b)      ((a) > (b) ? (a) : (b))

/* Allocator interface: everything that allocates takes one, so tests can
 * inject failing or counting allocators. realloc(ptr, 0) frees. */
typedef struct tb_allocator {
    void *(*realloc_fn)(void *user, void *ptr, size_t old_size, size_t new_size);
    void *user;
} tb_allocator;

/* The process heap (malloc/realloc/free). */
const tb_allocator *tb_heap_allocator(void);

void *tb_alloc(const tb_allocator *a, size_t size);
void *tb_realloc(const tb_allocator *a, void *ptr, size_t old_size, size_t new_size);
void tb_free(const tb_allocator *a, void *ptr, size_t size);

#endif
