/* SPDX-License-Identifier: MIT */
/* Test allocator: counts live bytes and can fail after N allocations. */
#ifndef TB_TEST_ALLOC_H
#define TB_TEST_ALLOC_H

#include "core/types.h"

typedef struct tb_test_alloc {
    tb_allocator iface;
    long fail_after; /* < 0: never fail; 0: fail next (re)allocation */
    size_t live_bytes;
    size_t allocations;
} tb_test_alloc;

void tb_test_alloc_init(tb_test_alloc *t, long fail_after);

#endif
