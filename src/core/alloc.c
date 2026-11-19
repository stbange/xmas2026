/* SPDX-License-Identifier: MIT */
#include "core/types.h"

#include <stdlib.h>

static void *heap_realloc(void *user, void *ptr, size_t old_size, size_t new_size)
{
    (void)user;
    (void)old_size;
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, new_size);
}

static const tb_allocator g_heap = {heap_realloc, NULL};

const tb_allocator *tb_heap_allocator(void)
{
    return &g_heap;
}

void *tb_alloc(const tb_allocator *a, size_t size)
{
    return size ? a->realloc_fn(a->user, NULL, 0, size) : NULL;
}

void *tb_realloc(const tb_allocator *a, void *ptr, size_t old_size, size_t new_size)
{
    return a->realloc_fn(a->user, ptr, old_size, new_size);
}

void tb_free(const tb_allocator *a, void *ptr, size_t size)
{
    if (ptr) {
        a->realloc_fn(a->user, ptr, size, 0);
    }
}
