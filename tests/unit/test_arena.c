/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/arena.h"

void setUp(void) {}
void tearDown(void) {}

static _Alignas(64) uint8_t g_mem[256];

static void test_arena_alloc_aligns_and_tracks_usage(void)
{
    tb_arena a;
    tb_arena_init(&a, g_mem, sizeof(g_mem));
    uint8_t *p1 = tb_arena_alloc(&a, 3, 1);
    uint8_t *p2 = tb_arena_alloc(&a, 8, 8);
    TEST_ASSERT_EQUAL_PTR(g_mem, p1);
    TEST_ASSERT_EQUAL_UINT(0, (uintptr_t)p2 % 8);
    TEST_ASSERT_EQUAL_UINT(16, a.used);
    TEST_ASSERT_EQUAL_UINT(16, a.high_water);
}

static void test_arena_rejects_bad_alignment(void)
{
    tb_arena a;
    tb_arena_init(&a, g_mem, sizeof(g_mem));
    TEST_ASSERT_NULL(tb_arena_alloc(&a, 4, 0));
    TEST_ASSERT_NULL(tb_arena_alloc(&a, 4, 3));
}

static void test_arena_out_of_memory(void)
{
    tb_arena a;
    tb_arena_init(&a, g_mem, 32);
    TEST_ASSERT_NOT_NULL(tb_arena_alloc(&a, 30, 1));
    TEST_ASSERT_NULL(tb_arena_alloc(&a, 4, 1));
    /* Alignment padding alone exceeds the arena. */
    TEST_ASSERT_NULL(tb_arena_alloc(&a, 0, 64));
    TEST_ASSERT_EQUAL_UINT(30, a.used);
}

static void test_arena_null_memory_has_no_space(void)
{
    tb_arena a;
    tb_arena_init(&a, NULL, 100);
    TEST_ASSERT_EQUAL_UINT(0, a.size);
    TEST_ASSERT_NULL(tb_arena_alloc(&a, 1, 1));
}

static void test_arena_calloc_zeroes(void)
{
    tb_arena a;
    tb_arena_init(&a, g_mem, sizeof(g_mem));
    g_mem[0] = 0xAA;
    uint8_t *p = tb_arena_calloc(&a, 4, 1);
    TEST_ASSERT_EQUAL_UINT8(0, p[0]);
    TEST_ASSERT_NULL(tb_arena_calloc(&a, 1000, 1));
}

static void test_arena_mark_rewind_reset(void)
{
    tb_arena a;
    tb_arena_init(&a, g_mem, sizeof(g_mem));
    tb_arena_alloc(&a, 10, 1);
    size_t m = tb_arena_mark(&a);
    tb_arena_alloc(&a, 20, 1);
    tb_arena_rewind(&a, m);
    TEST_ASSERT_EQUAL_UINT(10, a.used);
    tb_arena_rewind(&a, 200); /* beyond used: ignored */
    TEST_ASSERT_EQUAL_UINT(10, a.used);
    tb_arena_reset(&a);
    TEST_ASSERT_EQUAL_UINT(0, a.used);
    TEST_ASSERT_EQUAL_UINT(30, a.high_water);
}

static void test_pool_alloc_free_cycle(void)
{
    tb_pool p;
    TEST_ASSERT_TRUE(tb_pool_init(&p, g_mem, 64, 16));
    TEST_ASSERT_EQUAL_UINT(4, p.block_count);
    void *b[4];
    for (int i = 0; i < 4; i++) {
        b[i] = tb_pool_alloc(&p);
        TEST_ASSERT_NOT_NULL(b[i]);
    }
    TEST_ASSERT_NULL(tb_pool_alloc(&p));
    TEST_ASSERT_TRUE(tb_pool_free(&p, b[2]));
    TEST_ASSERT_EQUAL_PTR(b[2], tb_pool_alloc(&p));
    TEST_ASSERT_EQUAL_UINT(0, p.free_count);
}

static void test_pool_small_blocks_rounded_up(void)
{
    tb_pool p;
    TEST_ASSERT_TRUE(tb_pool_init(&p, g_mem, 64, 1));
    TEST_ASSERT_EQUAL_UINT(sizeof(void *), p.block_size);
}

static void test_pool_init_failures(void)
{
    tb_pool p;
    TEST_ASSERT_FALSE(tb_pool_init(&p, NULL, 64, 8));
    TEST_ASSERT_FALSE(tb_pool_init(&p, g_mem, 64, 0));
    TEST_ASSERT_FALSE(tb_pool_init(&p, g_mem, 4, 8));
    /* Misaligned start that leaves no room. */
    TEST_ASSERT_FALSE(tb_pool_init(&p, g_mem + 1, 2, 8));
}

static void test_pool_free_rejects_foreign_pointers(void)
{
    tb_pool p;
    TEST_ASSERT_TRUE(tb_pool_init(&p, g_mem + 64, 64, 16));
    void *b = tb_pool_alloc(&p);
    TEST_ASSERT_FALSE(tb_pool_free(&p, NULL));
    TEST_ASSERT_FALSE(tb_pool_free(&p, g_mem));       /* below the pool */
    TEST_ASSERT_FALSE(tb_pool_free(&p, g_mem + 200)); /* above the pool */
    TEST_ASSERT_FALSE(tb_pool_free(&p, (uint8_t *)b + 1));
    TEST_ASSERT_TRUE(tb_pool_free(&p, b));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_arena_alloc_aligns_and_tracks_usage);
    RUN_TEST(test_arena_rejects_bad_alignment);
    RUN_TEST(test_arena_out_of_memory);
    RUN_TEST(test_arena_null_memory_has_no_space);
    RUN_TEST(test_arena_calloc_zeroes);
    RUN_TEST(test_arena_mark_rewind_reset);
    RUN_TEST(test_pool_alloc_free_cycle);
    RUN_TEST(test_pool_small_blocks_rounded_up);
    RUN_TEST(test_pool_init_failures);
    RUN_TEST(test_pool_free_rejects_foreign_pointers);
    return UNITY_END();
}
