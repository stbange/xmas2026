/* SPDX-License-Identifier: MIT */
#include "core/hashmap.h"

#include "core/hash.h"

#include <string.h>

#define EMPTY 0u
#define TOMB  UINT64_MAX

void tb_hashmap_init(tb_hashmap *m, const tb_allocator *alloc)
{
    memset(m, 0, sizeof(*m));
    m->alloc = alloc ? alloc : tb_heap_allocator();
}

void tb_hashmap_free(tb_hashmap *m)
{
    tb_free(m->alloc, m->slots, m->cap * sizeof(tb_hashmap_slot));
    m->slots = NULL;
    m->cap = 0;
    m->count = 0;
    m->tombstones = 0;
}

static bool rehash(tb_hashmap *m, size_t new_cap)
{
    tb_hashmap_slot *slots = tb_alloc(m->alloc, new_cap * sizeof(tb_hashmap_slot));
    if (!slots) {
        return false;
    }
    memset(slots, 0, new_cap * sizeof(tb_hashmap_slot));
    for (size_t i = 0; i < m->cap; i++) {
        uint64_t k = m->slots[i].key;
        if (k != EMPTY && k != TOMB) {
            size_t j = (size_t)tb_hash_u64(k) & (new_cap - 1);
            while (slots[j].key != EMPTY) {
                j = (j + 1) & (new_cap - 1);
            }
            slots[j] = m->slots[i];
        }
    }
    tb_free(m->alloc, m->slots, m->cap * sizeof(tb_hashmap_slot));
    m->slots = slots;
    m->cap = new_cap;
    m->tombstones = 0;
    return true;
}

static size_t find(const tb_hashmap *m, uint64_t key, bool *found)
{
    size_t mask = m->cap - 1;
    size_t i = (size_t)tb_hash_u64(key) & mask;
    size_t first_tomb = SIZE_MAX;
    for (;;) {
        uint64_t k = m->slots[i].key;
        if (k == key) {
            *found = true;
            return i;
        }
        if (k == EMPTY) {
            *found = false;
            return first_tomb != SIZE_MAX ? first_tomb : i;
        }
        if (k == TOMB && first_tomb == SIZE_MAX) {
            first_tomb = i;
        }
        i = (i + 1) & mask;
    }
}

bool tb_hashmap_put(tb_hashmap *m, uint64_t key, uint64_t value)
{
    if (key == EMPTY || key == TOMB) {
        return false;
    }
    if ((m->count + m->tombstones + 1) * 10 > m->cap * 7) {
        size_t new_cap = m->cap ? m->cap : 16;
        while ((m->count + 1) * 10 > new_cap * 7) {
            new_cap *= 2;
        }
        if (!rehash(m, new_cap)) {
            return false;
        }
    }
    bool found;
    size_t i = find(m, key, &found);
    if (!found) {
        if (m->slots[i].key == TOMB) {
            m->tombstones--;
        }
        m->slots[i].key = key;
        m->count++;
    }
    m->slots[i].value = value;
    return true;
}

bool tb_hashmap_get(const tb_hashmap *m, uint64_t key, uint64_t *value)
{
    if (m->cap == 0 || key == EMPTY || key == TOMB) {
        return false;
    }
    bool found;
    size_t i = find(m, key, &found);
    if (found && value) {
        *value = m->slots[i].value;
    }
    return found;
}

bool tb_hashmap_remove(tb_hashmap *m, uint64_t key)
{
    if (m->cap == 0 || key == EMPTY || key == TOMB) {
        return false;
    }
    bool found;
    size_t i = find(m, key, &found);
    if (!found) {
        return false;
    }
    m->slots[i].key = TOMB;
    m->count--;
    m->tombstones++;
    return true;
}

void tb_hashmap_clear(tb_hashmap *m)
{
    if (m->slots) {
        memset(m->slots, 0, m->cap * sizeof(tb_hashmap_slot));
    }
    m->count = 0;
    m->tombstones = 0;
}

bool tb_hashmap_next(const tb_hashmap *m, size_t *it, uint64_t *key, uint64_t *value)
{
    while (*it < m->cap) {
        const tb_hashmap_slot *s = &m->slots[(*it)++];
        if (s->key != EMPTY && s->key != TOMB) {
            *key = s->key;
            *value = s->value;
            return true;
        }
    }
    return false;
}
