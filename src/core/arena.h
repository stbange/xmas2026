/* SPDX-License-Identifier: MIT */
/* Linear arena (per-frame scratch memory) and fixed-size block pool. Both
 * work on caller-provided memory and never allocate. */
#ifndef TB_CORE_ARENA_H
#define TB_CORE_ARENA_H

#include "core/types.h"

typedef struct tb_arena {
    uint8_t *base;
    size_t size;
    size_t used;
    size_t high_water;
} tb_arena;

void tb_arena_init(tb_arena *a, void *memory, size_t size);
/* align must be a power of two; returns NULL when out of memory or invalid. */
void *tb_arena_alloc(tb_arena *a, size_t size, size_t align);
/* Like tb_arena_alloc but zero-fills. */
void *tb_arena_calloc(tb_arena *a, size_t size, size_t align);
size_t tb_arena_mark(const tb_arena *a);
void tb_arena_rewind(tb_arena *a, size_t mark);
void tb_arena_reset(tb_arena *a);

typedef struct tb_pool {
    uint8_t *base;
    size_t block_size;
    size_t block_count;
    size_t free_count;
    void *free_list;
} tb_pool;

/* block_size is rounded up to hold a pointer and keep pointer alignment.
 * Returns false if the memory cannot hold at least one block. */
bool tb_pool_init(tb_pool *p, void *memory, size_t memory_size, size_t block_size);
void *tb_pool_alloc(tb_pool *p);
/* Returns false for pointers that do not belong to the pool. */
bool tb_pool_free(tb_pool *p, void *block);

#endif
