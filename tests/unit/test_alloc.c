/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/types.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_heap_alloc_realloc_free(void)
{
    const tb_allocator *h = tb_heap_allocator();
    char *p = tb_alloc(h, 16);
    TEST_ASSERT_NOT_NULL(p);
    strcpy(p, "snow");
    p = tb_realloc(h, p, 16, 64);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("snow", p);
    tb_free(h, p, 64);
}

static void test_zero_size_alloc_returns_null(void)
{
    TEST_ASSERT_NULL(tb_alloc(tb_heap_allocator(), 0));
}

static void test_free_null_is_noop(void)
{
    tb_free(tb_heap_allocator(), NULL, 0);
    TEST_PASS();
}

static void test_realloc_to_zero_frees(void)
{
    const tb_allocator *h = tb_heap_allocator();
    void *p = tb_alloc(h, 8);
    TEST_ASSERT_NULL(tb_realloc(h, p, 8, 0));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_heap_alloc_realloc_free);
    RUN_TEST(test_zero_size_alloc_returns_null);
    RUN_TEST(test_free_null_is_noop);
    RUN_TEST(test_realloc_to_zero_frees);
    return UNITY_END();
}
