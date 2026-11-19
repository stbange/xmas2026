/* SPDX-License-Identifier: MIT */
/* Growable array of fixed-size elements. */
#ifndef TB_CORE_DYNARR_H
#define TB_CORE_DYNARR_H

#include "core/types.h"

typedef struct tb_dynarr {
    const tb_allocator *alloc;
    uint8_t *data;
    size_t len;
    size_t cap;
    size_t elem_size;
} tb_dynarr;

void tb_dynarr_init(tb_dynarr *d, const tb_allocator *alloc, size_t elem_size);
void tb_dynarr_free(tb_dynarr *d);
/* Ensures capacity for at least `cap` elements. Returns false on allocation failure. */
bool tb_dynarr_reserve(tb_dynarr *d, size_t cap);
/* Appends a copy of *elem (or a zeroed element if elem is NULL). */
bool tb_dynarr_push(tb_dynarr *d, const void *elem);
/* Copies the last element to out (if not NULL) and removes it. */
bool tb_dynarr_pop(tb_dynarr *d, void *out);
void *tb_dynarr_at(const tb_dynarr *d, size_t index);
/* O(1) removal that moves the last element into the hole. */
bool tb_dynarr_swap_remove(tb_dynarr *d, size_t index);
void tb_dynarr_clear(tb_dynarr *d);

#endif
