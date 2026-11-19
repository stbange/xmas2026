/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/hashmap.h"
#include "support/test_alloc.h"

void setUp(void) {}
void tearDown(void) {}

static void test_put_get_replace(void)
{
    tb_hashmap m;
    tb_hashmap_init(&m, NULL);
    uint64_t v = 0;
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, 5, &v)); /* empty map */
    TEST_ASSERT_TRUE(tb_hashmap_put(&m, 5, 50));
    TEST_ASSERT_TRUE(tb_hashmap_put(&m, 5, 55));
    TEST_ASSERT_TRUE(tb_hashmap_get(&m, 5, &v));
    TEST_ASSERT_EQUAL_UINT64(55, v);
    TEST_ASSERT_TRUE(tb_hashmap_get(&m, 5, NULL));
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, 6, &v));
    TEST_ASSERT_EQUAL_UINT(1, m.count);
    tb_hashmap_free(&m);
}

static void test_reserved_keys_rejected(void)
{
    tb_hashmap m;
    tb_hashmap_init(&m, NULL);
    TEST_ASSERT_FALSE(tb_hashmap_put(&m, 0, 1));
    TEST_ASSERT_FALSE(tb_hashmap_put(&m, UINT64_MAX, 1));
    tb_hashmap_put(&m, 1, 1);
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, 0, NULL));
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, UINT64_MAX, NULL));
    TEST_ASSERT_FALSE(tb_hashmap_remove(&m, 0));
    TEST_ASSERT_FALSE(tb_hashmap_remove(&m, UINT64_MAX));
    tb_hashmap_free(&m);
}

static void test_many_keys_grow_and_iterate(void)
{
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, -1);
    tb_hashmap m;
    tb_hashmap_init(&m, &ta.iface);
    for (uint64_t k = 1; k <= 1000; k++) {
        TEST_ASSERT_TRUE(tb_hashmap_put(&m, k, k * 3));
    }
    TEST_ASSERT_EQUAL_UINT(1000, m.count);
    uint64_t sum = 0, key, value;
    size_t it = 0, n = 0;
    while (tb_hashmap_next(&m, &it, &key, &value)) {
        TEST_ASSERT_EQUAL_UINT64(key * 3, value);
        sum += key;
        n++;
    }
    TEST_ASSERT_EQUAL_UINT(1000, n);
    TEST_ASSERT_EQUAL_UINT64(500500, sum);
    tb_hashmap_free(&m);
    TEST_ASSERT_EQUAL_UINT(0, ta.live_bytes);
}

static void test_remove_and_reuse_tombstones(void)
{
    tb_hashmap m;
    tb_hashmap_init(&m, NULL);
    for (uint64_t k = 1; k <= 10; k++) {
        tb_hashmap_put(&m, k, k);
    }
    TEST_ASSERT_TRUE(tb_hashmap_remove(&m, 3));
    TEST_ASSERT_FALSE(tb_hashmap_remove(&m, 3));
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, 3, NULL));
    TEST_ASSERT_EQUAL_UINT(1, m.tombstones);
    /* Churn: insert/remove many keys so tombstones are reused and purged. */
    for (uint64_t k = 100; k < 400; k++) {
        TEST_ASSERT_TRUE(tb_hashmap_put(&m, k, k));
        TEST_ASSERT_TRUE(tb_hashmap_remove(&m, k));
    }
    TEST_ASSERT_TRUE(tb_hashmap_put(&m, 3, 33));
    uint64_t v;
    TEST_ASSERT_TRUE(tb_hashmap_get(&m, 3, &v));
    TEST_ASSERT_EQUAL_UINT64(33, v);
    for (uint64_t k = 1; k <= 10; k++) {
        TEST_ASSERT_TRUE(tb_hashmap_get(&m, k, NULL));
    }
    TEST_ASSERT_EQUAL_UINT(10, m.count);
    tb_hashmap_free(&m);
}

static void test_clear_and_empty_ops(void)
{
    tb_hashmap m;
    tb_hashmap_init(&m, NULL);
    tb_hashmap_clear(&m); /* no slots yet */
    TEST_ASSERT_FALSE(tb_hashmap_remove(&m, 1));
    tb_hashmap_put(&m, 1, 1);
    tb_hashmap_remove(&m, 1);
    tb_hashmap_clear(&m);
    TEST_ASSERT_EQUAL_UINT(0, m.count);
    TEST_ASSERT_EQUAL_UINT(0, m.tombstones);
    TEST_ASSERT_FALSE(tb_hashmap_get(&m, 1, NULL));
    tb_hashmap_free(&m);
}

static void test_allocation_failure(void)
{
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, 0);
    tb_hashmap m;
    tb_hashmap_init(&m, &ta.iface);
    TEST_ASSERT_FALSE(tb_hashmap_put(&m, 1, 1));
    TEST_ASSERT_EQUAL_UINT(0, m.count);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_put_get_replace);
    RUN_TEST(test_reserved_keys_rejected);
    RUN_TEST(test_many_keys_grow_and_iterate);
    RUN_TEST(test_remove_and_reuse_tombstones);
    RUN_TEST(test_clear_and_empty_ops);
    RUN_TEST(test_allocation_failure);
    return UNITY_END();
}
