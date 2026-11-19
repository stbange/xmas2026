/* SPDX-License-Identifier: MIT */
#include "support/test_alloc.h"

#include <stdlib.h>

static void *test_realloc(void *user, void *ptr, size_t old_size, size_t new_size)
{
    tb_test_alloc *t = user;
    if (new_size == 0) {
        free(ptr);
        t->live_bytes -= old_size;
        return NULL;
    }
    if (t->fail_after == 0) {
        return NULL;
    }
    if (t->fail_after > 0) {
        t->fail_after--;
    }
    void *p = realloc(ptr, new_size);
    if (p) {
        t->live_bytes = t->live_bytes - old_size + new_size;
        t->allocations++;
    }
    return p;
}

void tb_test_alloc_init(tb_test_alloc *t, long fail_after)
{
    t->iface.realloc_fn = test_realloc;
    t->iface.user = t;
    t->fail_after = fail_after;
    t->live_bytes = 0;
    t->allocations = 0;
}
