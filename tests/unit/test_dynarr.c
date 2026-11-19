/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/dynarr.h"
#include "support/test_alloc.h"

void setUp(void) {}
void tearDown(void) {}

static void test_push_grows_and_keeps_values(void)
{
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, -1);
    tb_dynarr d;
    tb_dynarr_init(&d, &ta.iface, sizeof(int));
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT_TRUE(tb_dynarr_push(&d, &i));
    }
    TEST_ASSERT_EQUAL_UINT(100, d.len);
    TEST_ASSERT_EQUAL_INT(42, *(int *)tb_dynarr_at(&d, 42));
    TEST_ASSERT_NULL(tb_dynarr_at(&d, 100));
    tb_dynarr_free(&d);
    TEST_ASSERT_EQUAL_UINT(0, ta.live_bytes);
}

static void test_push_null_zero_fills(void)
{
    tb_dynarr d;
    tb_dynarr_init(&d, NULL, sizeof(int));
    int v = 7;
    tb_dynarr_push(&d, &v);
    tb_dynarr_pop(&d, NULL);
    TEST_ASSERT_TRUE(tb_dynarr_push(&d, NULL));
    TEST_ASSERT_EQUAL_INT(0, *(int *)tb_dynarr_at(&d, 0));
    tb_dynarr_free(&d);
}

static void test_pop_and_swap_remove(void)
{
    tb_dynarr d;
    tb_dynarr_init(&d, NULL, sizeof(int));
    int out = -1;
    TEST_ASSERT_FALSE(tb_dynarr_pop(&d, &out));
    for (int i = 0; i < 4; i++) {
        tb_dynarr_push(&d, &i);
    }
    TEST_ASSERT_TRUE(tb_dynarr_pop(&d, &out));
    TEST_ASSERT_EQUAL_INT(3, out);
    TEST_ASSERT_TRUE(tb_dynarr_swap_remove(&d, 0)); /* moves 2 into slot 0 */
    TEST_ASSERT_EQUAL_INT(2, *(int *)tb_dynarr_at(&d, 0));
    TEST_ASSERT_TRUE(tb_dynarr_swap_remove(&d, 1)); /* last element: no move */
    TEST_ASSERT_EQUAL_UINT(1, d.len);
    TEST_ASSERT_FALSE(tb_dynarr_swap_remove(&d, 5));
    tb_dynarr_clear(&d);
    TEST_ASSERT_EQUAL_UINT(0, d.len);
    tb_dynarr_free(&d);
}

static void test_allocation_failure_is_reported(void)
{
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, 0);
    tb_dynarr d;
    tb_dynarr_init(&d, &ta.iface, sizeof(int));
    int v = 1;
    TEST_ASSERT_FALSE(tb_dynarr_push(&d, &v));
    TEST_ASSERT_EQUAL_UINT(0, d.len);
}

static void test_reserve_edge_cases(void)
{
    tb_dynarr d;
    tb_dynarr_init(&d, NULL, sizeof(int));
    TEST_ASSERT_TRUE(tb_dynarr_reserve(&d, 16));
    TEST_ASSERT_TRUE(tb_dynarr_reserve(&d, 8)); /* already big enough */
    TEST_ASSERT_FALSE(tb_dynarr_reserve(&d, SIZE_MAX));
    tb_dynarr_free(&d);
    tb_dynarr zero;
    tb_dynarr_init(&zero, NULL, 0);
    TEST_ASSERT_FALSE(tb_dynarr_reserve(&zero, 4));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_push_grows_and_keeps_values);
    RUN_TEST(test_push_null_zero_fills);
    RUN_TEST(test_pop_and_swap_remove);
    RUN_TEST(test_allocation_failure_is_reported);
    RUN_TEST(test_reserve_edge_cases);
    return UNITY_END();
}
