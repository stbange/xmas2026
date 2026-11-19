/* SPDX-License-Identifier: MIT */
#include "core/arena.h"

#include <string.h>

void tb_arena_init(tb_arena *a, void *memory, size_t size)
{
    a->base = memory;
    a->size = memory ? size : 0;
    a->used = 0;
    a->high_water = 0;
}

static bool is_pow2(size_t v)
{
    return v != 0 && (v & (v - 1)) == 0;
}

void *tb_arena_alloc(tb_arena *a, size_t size, size_t align)
{
    if (!is_pow2(align)) {
        return NULL;
    }
    uintptr_t base = (uintptr_t)a->base;
    uintptr_t cur = base + a->used;
    uintptr_t aligned = (cur + (align - 1)) & ~(uintptr_t)(align - 1);
    size_t offset = (size_t)(aligned - base);
    if (offset > a->size || size > a->size - offset) {
        return NULL;
    }
    a->used = offset + size;
    if (a->used > a->high_water) {
        a->high_water = a->used;
    }
    return a->base + offset;
}

void *tb_arena_calloc(tb_arena *a, size_t size, size_t align)
{
    void *p = tb_arena_alloc(a, size, align);
    if (p) {
        memset(p, 0, size);
    }
    return p;
}

size_t tb_arena_mark(const tb_arena *a)
{
    return a->used;
}

void tb_arena_rewind(tb_arena *a, size_t mark)
{
    if (mark <= a->used) {
        a->used = mark;
    }
}

void tb_arena_reset(tb_arena *a)
{
    a->used = 0;
}

bool tb_pool_init(tb_pool *p, void *memory, size_t memory_size, size_t block_size)
{
    const size_t ptr_align = sizeof(void *);
    memset(p, 0, sizeof(*p));
    if (!memory || block_size == 0) {
        return false;
    }
    uintptr_t start = ((uintptr_t)memory + (ptr_align - 1)) & ~(uintptr_t)(ptr_align - 1);
    size_t skipped = (size_t)(start - (uintptr_t)memory);
    if (block_size < sizeof(void *)) {
        block_size = sizeof(void *);
    }
    block_size = (block_size + ptr_align - 1) & ~(ptr_align - 1);
    if (skipped >= memory_size || (memory_size - skipped) / block_size == 0) {
        return false;
    }
    p->base = (uint8_t *)memory + skipped;
    p->block_size = block_size;
    p->block_count = (memory_size - skipped) / block_size;
    p->free_count = p->block_count;
    p->free_list = NULL;
    for (size_t i = p->block_count; i-- > 0;) {
        void *block = p->base + i * block_size;
        memcpy(block, &p->free_list, sizeof(void *));
        p->free_list = block;
    }
    return true;
}

void *tb_pool_alloc(tb_pool *p)
{
    void *block = p->free_list;
    if (!block) {
        return NULL;
    }
    memcpy(&p->free_list, block, sizeof(void *));
    p->free_count--;
    return block;
}

bool tb_pool_free(tb_pool *p, void *block)
{
    uintptr_t b = (uintptr_t)block;
    uintptr_t lo = (uintptr_t)p->base;
    uintptr_t hi = lo + p->block_count * p->block_size;
    if (!block || b < lo || b >= hi || (b - lo) % p->block_size != 0) {
        return false;
    }
    memcpy(block, &p->free_list, sizeof(void *));
    p->free_list = block;
    p->free_count++;
    return true;
}
