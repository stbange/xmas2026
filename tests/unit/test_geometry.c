/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/geometry.h"

void setUp(void) {}
void tearDown(void) {}

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;

#define EPS 0.01f
static const tb_vec3 PMIN = {-0.3f, -0.3f, 0.0f};
static const tb_vec3 PMAX = {0.3f, 0.3f, 1.8f};

static void test_box_hits_wall_with_normal(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_trace tr = tb_trace_box(&g_geo, tb_v3(15, 0, 0.1f), tb_v3(25, 0, 0.1f), PMIN, PMAX, TB_CONTENTS_SOLID);
    TEST_ASSERT_TRUE(tr.fraction < 1.0f);
    TEST_ASSERT_FLOAT_WITHIN(EPS, 19.7f, tr.end.x);
    TEST_ASSERT_FLOAT_WITHIN(EPS, -1.0f, tr.normal.x);
    TEST_ASSERT_FALSE(tr.start_solid);
}

static void test_free_move_and_parallel_slide(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_trace tr =
        tb_trace_box(&g_geo, tb_v3(0, -10, 0.1f), tb_v3(0, -12, 0.1f), PMIN, PMAX, TB_CONTENTS_SOLID);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tr.fraction);
    /* Moving parallel to the floor while touching it is not blocked. */
    tr = tb_trace_box(&g_geo, tb_v3(0, -10, 0.0005f), tb_v3(3, -10, 0.0005f), PMIN, PMAX, TB_CONTENTS_SOLID);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tr.fraction);
}

static void test_start_and_all_solid(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_trace tr = tb_trace_box(&g_geo, tb_v3(-5, 6, 1), tb_v3(-5, 6, 2), PMIN, PMAX, TB_CONTENTS_SOLID);
    TEST_ASSERT_TRUE(tr.start_solid);
    TEST_ASSERT_TRUE(tr.all_solid);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tr.fraction);
    /* Start inside the pillar, end outside: start_solid but not all_solid. */
    tr = tb_trace_box(&g_geo, tb_v3(-5, 6, 1), tb_v3(-5, 9, 1), PMIN, PMAX, TB_CONTENTS_SOLID);
    TEST_ASSERT_TRUE(tr.start_solid);
    TEST_ASSERT_FALSE(tr.all_solid);
}

static void test_ramp_surface(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    /* Ramp surface z = (y - 10) / 2: at y = 13 it is 1.5 m high. */
    tb_trace tr = tb_trace_ray(&g_geo, tb_v3(0, 13, 5), tb_v3(0, 13, -0.5f), TB_CONTENTS_SOLID);
    TEST_ASSERT_FLOAT_WITHIN(EPS, 1.5f, tr.end.z);
    TEST_ASSERT_TRUE(tr.normal.z > 0.85f && tr.normal.z < 0.95f);
}

static void test_contents_mask(void)
{
    tb_brush store[2];
    tb_geometry g;
    tb_geometry_init(&g, store, 2);
    TEST_ASSERT_TRUE(tb_geometry_add_box(&g, tb_v3(1, -1, -1), tb_v3(2, 1, 1), TB_CONTENTS_PLAYERCLIP));
    tb_trace tr = tb_trace_ray(&g, tb_v3(0, 0, 0), tb_v3(5, 0, 0), TB_CONTENTS_SOLID);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tr.fraction); /* shots pass player clips */
    tr = tb_trace_ray(&g, tb_v3(0, 0, 0), tb_v3(5, 0, 0), TB_CONTENTS_PLAYERCLIP);
    TEST_ASSERT_TRUE(tr.fraction < 1.0f);
}

static void test_storage_limits(void)
{
    tb_brush store[1];
    tb_geometry g;
    tb_geometry_init(&g, store, 1);
    TEST_ASSERT_TRUE(tb_geometry_add_box(&g, tb_v3(0, 0, 0), tb_v3(1, 1, 1), TB_CONTENTS_SOLID));
    TEST_ASSERT_FALSE(tb_geometry_add_box(&g, tb_v3(0, 0, 0), tb_v3(1, 1, 1), TB_CONTENTS_SOLID));
    tb_plane p = {{0, 0, 1}, 1};
    TEST_ASSERT_FALSE(tb_geometry_add_brush(&g, &p, 1, tb_v3(0, 0, 0), tb_v3(1, 1, 1), TB_CONTENTS_SOLID));
    TEST_ASSERT_FALSE(tb_geometry_add_brush(&g, &p, 0, tb_v3(0, 0, 0), tb_v3(1, 1, 1), TB_CONTENTS_SOLID));
    tb_plane many[TB_BRUSH_MAX_PLANES];
    TEST_ASSERT_FALSE(tb_geometry_add_brush(&g, many, TB_BRUSH_MAX_PLANES, tb_v3(0, 0, 0), tb_v3(1, 1, 1),
                                            TB_CONTENTS_SOLID));
    tb_geometry none;
    tb_geometry_init(&none, NULL, 5);
    TEST_ASSERT_EQUAL_INT(0, none.cap);
}

static void test_ray_box_and_overlap(void)
{
    tb_vec3 lo = tb_v3(1, -1, -1), hi = tb_v3(2, 1, 1);
    TEST_ASSERT_FLOAT_WITHIN(EPS, 0.25f, tb_ray_box(tb_v3(0, 0, 0), tb_v3(4, 0, 0), lo, hi));
    TEST_ASSERT_FLOAT_WITHIN(EPS, 0.5f,
                             tb_ray_box(tb_v3(4, 0, 0), tb_v3(0, 0, 0), lo, hi)); /* enters at x = 2 */
    TEST_ASSERT_TRUE(tb_ray_box(tb_v3(0, 5, 0), tb_v3(4, 5, 0), lo, hi) > 1.0f);  /* parallel, outside */
    TEST_ASSERT_TRUE(tb_ray_box(tb_v3(0, 0, 0), tb_v3(4, 4, 0), lo, hi) <= 1.0f); /* diagonal clips corner */
    TEST_ASSERT_TRUE(tb_ray_box(tb_v3(0, 0, 0), tb_v3(0.5f, 0, 0), lo, hi) > 1.0f); /* too short */
    TEST_ASSERT_TRUE(tb_ray_box(tb_v3(0, -3, 0), tb_v3(4, 3, 5), lo, hi) > 1.0f);   /* misses above */
    TEST_ASSERT_TRUE(tb_boxes_overlap(lo, hi, tb_v3(1.5f, 0, 0), tb_v3(3, 2, 2)));
    TEST_ASSERT_FALSE(tb_boxes_overlap(lo, hi, tb_v3(3, 0, 0), tb_v3(4, 1, 1)));
    TEST_ASSERT_FALSE(tb_boxes_overlap(lo, hi, tb_v3(1, 2, 0), tb_v3(2, 3, 1)));
    TEST_ASSERT_FALSE(tb_boxes_overlap(lo, hi, tb_v3(1, 0, 2), tb_v3(2, 1, 3)));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_box_hits_wall_with_normal);
    RUN_TEST(test_free_move_and_parallel_slide);
    RUN_TEST(test_start_and_all_solid);
    RUN_TEST(test_ramp_surface);
    RUN_TEST(test_contents_mask);
    RUN_TEST(test_storage_limits);
    RUN_TEST(test_ray_box_and_overlap);
    return UNITY_END();
}
