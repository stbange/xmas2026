/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/hash.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_fnv1a_known_vectors(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x811c9dc5u, tb_fnv1a32("", 0));
    TEST_ASSERT_EQUAL_HEX32(0xe40c292cu, tb_fnv1a32("a", 1));
    TEST_ASSERT_EQUAL_HEX64(0xcbf29ce484222325ull, tb_fnv1a64("", 0));
    TEST_ASSERT_EQUAL_HEX64(0xaf63dc4c8601ec8cull, tb_fnv1a64("a", 1));
    TEST_ASSERT_EQUAL_HEX64(tb_fnv1a64("foobar", 6), tb_hash_str("foobar"));
}

static void test_fnv1a64_update_is_incremental(void)
{
    uint64_t h = tb_fnv1a64("foo", 3);
    TEST_ASSERT_EQUAL_HEX64(tb_fnv1a64("foobar", 6), tb_fnv1a64_update(h, "bar", 3));
}

static void test_crc32_known_vector(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xCBF43926u, tb_crc32("123456789", 9));
    TEST_ASSERT_EQUAL_HEX32(0u, tb_crc32("", 0));
    uint32_t part = tb_crc32("12345", 5);
    TEST_ASSERT_EQUAL_HEX32(0xCBF43926u, tb_crc32_update(part, "6789", 4));
}

static void test_hash_u64_mixes(void)
{
    TEST_ASSERT_NOT_EQUAL(tb_hash_u64(1), tb_hash_u64(2));
    TEST_ASSERT_EQUAL_HEX64(tb_hash_u64(12345), tb_hash_u64(12345));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_fnv1a_known_vectors);
    RUN_TEST(test_fnv1a64_update_is_incremental);
    RUN_TEST(test_crc32_known_vector);
    RUN_TEST(test_hash_u64_mixes);
    return UNITY_END();
}
