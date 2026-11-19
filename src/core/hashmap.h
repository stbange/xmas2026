/* SPDX-License-Identifier: MIT */
/* Open-addressing hash map from uint64 keys to uint64 values (linear
 * probing, tombstones, grows at 70 % load). Key 0 is reserved. */
#ifndef TB_CORE_HASHMAP_H
#define TB_CORE_HASHMAP_H

#include "core/types.h"

typedef struct tb_hashmap_slot {
    uint64_t key; /* 0 = empty, UINT64_MAX = tombstone */
    uint64_t value;
} tb_hashmap_slot;

typedef struct tb_hashmap {
    const tb_allocator *alloc;
    tb_hashmap_slot *slots;
    size_t cap; /* power of two */
    size_t count;
    size_t tombstones;
} tb_hashmap;

void tb_hashmap_init(tb_hashmap *m, const tb_allocator *alloc);
void tb_hashmap_free(tb_hashmap *m);
/* Inserts or replaces. Returns false for reserved keys (0, UINT64_MAX) or OOM. */
bool tb_hashmap_put(tb_hashmap *m, uint64_t key, uint64_t value);
bool tb_hashmap_get(const tb_hashmap *m, uint64_t key, uint64_t *value);
bool tb_hashmap_remove(tb_hashmap *m, uint64_t key);
void tb_hashmap_clear(tb_hashmap *m);

/* Iteration: start with *it = 0; returns false when done. */
bool tb_hashmap_next(const tb_hashmap *m, size_t *it, uint64_t *key, uint64_t *value);

#endif
