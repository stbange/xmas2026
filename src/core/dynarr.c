/* SPDX-License-Identifier: MIT */
#include "core/dynarr.h"

#include <string.h>

void tb_dynarr_init(tb_dynarr *d, const tb_allocator *alloc, size_t elem_size)
{
    d->alloc = alloc ? alloc : tb_heap_allocator();
    d->data = NULL;
    d->len = 0;
    d->cap = 0;
    d->elem_size = elem_size;
}

void tb_dynarr_free(tb_dynarr *d)
{
    tb_free(d->alloc, d->data, d->cap * d->elem_size);
    d->data = NULL;
    d->len = 0;
    d->cap = 0;
}

bool tb_dynarr_reserve(tb_dynarr *d, size_t cap)
{
    if (cap <= d->cap) {
        return true;
    }
    if (d->elem_size == 0 || cap > SIZE_MAX / d->elem_size) {
        return false;
    }
    void *p = tb_realloc(d->alloc, d->data, d->cap * d->elem_size, cap * d->elem_size);
    if (!p) {
        return false;
    }
    d->data = p;
    d->cap = cap;
    return true;
}

bool tb_dynarr_push(tb_dynarr *d, const void *elem)
{
    if (d->len == d->cap && !tb_dynarr_reserve(d, d->cap ? d->cap * 2 : 8)) {
        return false;
    }
    uint8_t *dst = d->data + d->len * d->elem_size;
    if (elem) {
        memcpy(dst, elem, d->elem_size);
    } else {
        memset(dst, 0, d->elem_size);
    }
    d->len++;
    return true;
}

bool tb_dynarr_pop(tb_dynarr *d, void *out)
{
    if (d->len == 0) {
        return false;
    }
    d->len--;
    if (out) {
        memcpy(out, d->data + d->len * d->elem_size, d->elem_size);
    }
    return true;
}

void *tb_dynarr_at(const tb_dynarr *d, size_t index)
{
    return index < d->len ? d->data + index * d->elem_size : NULL;
}

bool tb_dynarr_swap_remove(tb_dynarr *d, size_t index)
{
    if (index >= d->len) {
        return false;
    }
    d->len--;
    if (index != d->len) {
        memcpy(d->data + index * d->elem_size, d->data + d->len * d->elem_size, d->elem_size);
    }
    return true;
}

void tb_dynarr_clear(tb_dynarr *d)
{
    d->len = 0;
}
