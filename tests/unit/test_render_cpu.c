/* SPDX-License-Identifier: MIT */
/* CPU-side rendering: view layout, palette/LUT, meshes, camera, scene. */
#include "unity.h"

#include "render/camera.h"
#include "render/mesh.h"
#include "render/palette.h"
#include "render/scene.h"
#include "render/view.h"
#include "support/test_alloc.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ---- view ---- */

/* @scenario retro-rendering: Default preset at 1080p */
static void test_default_preset_at_1080p(void)
{
    tb_view_layout l = tb_view_layout_for(1920, 1080, TB_RES_CLASSIC);
    TEST_ASSERT_EQUAL_INT(480, l.internal_w);
    TEST_ASSERT_EQUAL_INT(270, l.internal_h);
    TEST_ASSERT_EQUAL_INT(4, l.scale);
    TEST_ASSERT_EQUAL_INT(1920, l.viewport_w);
    TEST_ASSERT_EQUAL_INT(0, l.viewport_x);
    TEST_ASSERT_EQUAL_INT(960, l.ui_w);
}

static void test_presets_and_odd_windows(void)
{
    TEST_ASSERT_EQUAL_INT(180, tb_res_preset_lines(TB_RES_CRUNCHY));
    TEST_ASSERT_EQUAL_INT(360, tb_res_preset_lines(TB_RES_CRISP));
    TEST_ASSERT_EQUAL_INT(270, tb_res_preset_lines((tb_res_preset)9));
    tb_view_layout l = tb_view_layout_for(3840, 2160, TB_RES_CRUNCHY);
    TEST_ASSERT_EQUAL_INT(320, l.internal_w);
    TEST_ASSERT_EQUAL_INT(12, l.scale);
    l = tb_view_layout_for(1000, 1000, TB_RES_CLASSIC); /* square: letterboxed */
    TEST_ASSERT_EQUAL_INT(270, l.internal_w);
    TEST_ASSERT_EQUAL_INT(3, l.scale);
    TEST_ASSERT_EQUAL_INT((1000 - 810) / 2, l.viewport_x);
    l = tb_view_layout_for(200, 100, TB_RES_CRISP); /* window smaller than the preset */
    TEST_ASSERT_EQUAL_INT(100, l.internal_h);
    TEST_ASSERT_EQUAL_INT(1, l.scale);
    l = tb_view_layout_for(0, -5, TB_RES_CLASSIC);
    TEST_ASSERT_EQUAL_INT(1, l.internal_h);
    TEST_ASSERT_EQUAL_INT(1, l.internal_w);
    l = tb_view_layout_for(1, 1000, TB_RES_CLASSIC); /* very tall */
    TEST_ASSERT_EQUAL_INT(1, l.internal_w);
}

static void test_split_screen(void)
{
    int x, y, w, h;
    tb_view_split(1, 0, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(800, w);
    tb_view_split(2, 1, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(400, x);
    TEST_ASSERT_EQUAL_INT(400, w);
    tb_view_split(2, 0, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(0, x);
    tb_view_split(3, 0, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(300, y);
    TEST_ASSERT_EQUAL_INT(800, w);
    tb_view_split(3, 1, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(0, x);
    TEST_ASSERT_EQUAL_INT(400, w);
    tb_view_split(3, 2, 800, 600, &x, &y, &w, &h);
    TEST_ASSERT_EQUAL_INT(400, x);
    for (int i = 0; i < 4; i++) {
        tb_view_split(4, i, 800, 600, &x, &y, &w, &h);
        TEST_ASSERT_EQUAL_INT(i % 2 ? 400 : 0, x);
        TEST_ASSERT_EQUAL_INT(i < 2 ? 300 : 0, y);
    }
}

/* ---- palette ---- */

static void test_palette_and_lut(void)
{
    tb_palette p;
    tb_palette_default(&p);
    TEST_ASSERT_EQUAL_INT(256, p.count);
    tb_rgb red = {250, 5, 5};
    tb_rgb n = p.colors[tb_palette_nearest(&p, red)];
    TEST_ASSERT_EQUAL_UINT8(255, n.r);
    TEST_ASSERT_EQUAL_UINT8(0, n.g);
    static tb_rgb lut[TB_LUT_SIZE * TB_LUT_SIZE * TB_LUT_SIZE];
    tb_palette_build_lut(&p, lut);
    tb_rgb white = lut[TB_LUT_SIZE * TB_LUT_SIZE * TB_LUT_SIZE - 1];
    TEST_ASSERT_EQUAL_UINT8(255, white.r);
    TEST_ASSERT_EQUAL_UINT8(255, white.b);
    tb_palette empty = {{{0, 0, 0}}, 0}; /* no palette: identity mapping */
    tb_palette_build_lut(&empty, lut);
    TEST_ASSERT_EQUAL_UINT8(255, lut[TB_LUT_SIZE - 1].r);
}

static void test_bayer_matrix(void)
{
    float sum = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            float t = tb_bayer4(x, y);
            TEST_ASSERT_TRUE(t >= -0.5f && t < 0.5f);
            sum += t;
        }
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, 0.0f, sum);
    TEST_ASSERT_EQUAL_FLOAT(tb_bayer4(1, 2), tb_bayer4(5, 6)); /* tiles */
}

/* ---- meshes ---- */

static const uint8_t WHITE[4] = {255, 255, 255, 255};

static void test_box_brush_has_twelve_triangles(void)
{
    tb_brush b;
    tb_geometry g;
    tb_geometry_init(&g, &b, 1);
    tb_geometry_add_box(&g, tb_v3(0, 0, 0), tb_v3(1, 2, 3), TB_CONTENTS_SOLID);
    tb_mesh m;
    tb_mesh_init(&m, NULL);
    TEST_ASSERT_TRUE(tb_mesh_add_brush(&m, &b, WHITE));
    TEST_ASSERT_EQUAL_UINT(12, tb_mesh_triangle_count(&m));
    /* Every triangle faces outward (winding matches the normal). */
    for (size_t t = 0; t < tb_mesh_triangle_count(&m); t++) {
        tb_vertex *v = tb_dynarr_at(&m.vertices, t * 3);
        tb_vec3 a = tb_v3(v[0].pos[0], v[0].pos[1], v[0].pos[2]);
        tb_vec3 b2 = tb_v3(v[1].pos[0], v[1].pos[1], v[1].pos[2]);
        tb_vec3 c = tb_v3(v[2].pos[0], v[2].pos[1], v[2].pos[2]);
        tb_vec3 n = tb_v3_cross(tb_v3_sub(b2, a), tb_v3_sub(c, a));
        TEST_ASSERT_TRUE(tb_v3_dot(n, tb_v3(v[0].normal[0], v[0].normal[1], v[0].normal[2])) > 0);
    }
    tb_mesh_free(&m);
}

static void test_arena_mesh_and_colours(void)
{
    tb_brush store[TB_TEST_ARENA_BRUSHES];
    tb_geometry g;
    tb_geometry_build_test_arena(&g, store);
    const uint8_t f[4] = {1, 0, 0, 255}, w[4] = {2, 0, 0, 255}, c[4] = {3, 0, 0, 255};
    tb_mesh m;
    tb_mesh_init(&m, NULL);
    TEST_ASSERT_TRUE(tb_mesh_add_geometry(&m, &g, f, w, c));
    /* 11 boxes x 12 triangles + the ramp (5 faces: slope, 2 sides, back, bottom). */
    TEST_ASSERT_TRUE(tb_mesh_triangle_count(&m) >= 11 * 12 + 5);
    int floors = 0, walls = 0, ceilings = 0;
    for (size_t i = 0; i < m.vertices.len; i++) {
        tb_vertex *v = tb_dynarr_at(&m.vertices, i);
        floors += v->color[0] == 1;
        walls += v->color[0] == 2;
        ceilings += v->color[0] == 3;
    }
    TEST_ASSERT_TRUE(floors > 0 && walls > 0 && ceilings > 0);
    tb_mesh_free(&m);
}

static void test_mesh_allocation_failure(void)
{
    tb_brush store[TB_TEST_ARENA_BRUSHES];
    tb_geometry g;
    tb_geometry_build_test_arena(&g, store);
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, 0);
    tb_mesh m;
    tb_mesh_init(&m, &ta.iface);
    TEST_ASSERT_FALSE(tb_mesh_add_geometry(&m, &g, WHITE, WHITE, WHITE));
    TEST_ASSERT_FALSE(tb_mesh_add_brush(&m, &store[0], WHITE));
    tb_vec3 tri[3] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
    TEST_ASSERT_FALSE(tb_mesh_add_polygon(&m, tri, 3, tb_v3(0, 0, 1), WHITE));
    /* Growth happens at vertex 9 (3rd of triangle 3) and vertex 17 (2nd of
     * triangle 6): failing there covers the other two push paths. */
    for (long ok_allocs = 1; ok_allocs <= 2; ok_allocs++) {
        tb_test_alloc_init(&ta, ok_allocs);
        tb_mesh_init(&m, &ta.iface);
        for (int i = 0; i < 12 && tb_mesh_add_polygon(&m, tri, 3, tb_v3(0, 0, 1), WHITE); i++) {
        }
        tb_mesh_free(&m);
    }
    TEST_PASS();
}

static uint8_t g_pal[256][4];

static size_t voxel_quads(const uint8_t *data, int sx, int sy, int sz)
{
    for (int i = 0; i < 256; i++) {
        g_pal[i][0] = (uint8_t)i;
        g_pal[i][3] = 255;
    }
    tb_voxels v = {sx, sy, sz, data, 0.5f, {0, 0, 0}, (const uint8_t(*)[4])g_pal};
    tb_mesh m;
    tb_mesh_init(&m, NULL);
    TEST_ASSERT_TRUE(tb_mesh_add_voxels(&m, &v));
    size_t quads = tb_mesh_triangle_count(&m) / 2;
    tb_mesh_free(&m);
    return quads;
}

static void test_greedy_voxel_meshing(void)
{
    const uint8_t one[1] = {5};
    TEST_ASSERT_EQUAL_UINT(6, voxel_quads(one, 1, 1, 1));
    const uint8_t bar[4] = {5, 5, 5, 5};
    TEST_ASSERT_EQUAL_UINT(6, voxel_quads(bar, 4, 1, 1)); /* merged into one long box */
    const uint8_t two[2] = {5, 6};
    TEST_ASSERT_EQUAL_UINT(10, voxel_quads(two, 2, 1, 1)); /* colours not merged */
    uint8_t cube[27];
    memset(cube, 7, sizeof(cube));
    TEST_ASSERT_EQUAL_UINT(6, voxel_quads(cube, 3, 3, 3)); /* inner faces culled */
    const uint8_t hole[3] = {7, 0, 7};
    TEST_ASSERT_EQUAL_UINT(12, voxel_quads(hole, 3, 1, 1));
    const uint8_t empty[8] = {0};
    TEST_ASSERT_EQUAL_UINT(0, voxel_quads(empty, 2, 2, 2));
    /* L-shape in a 2x2 slice: greedy rows stop at a colour change. */
    const uint8_t ell[4] = {5, 5, 5, 6};
    TEST_ASSERT_TRUE(voxel_quads(ell, 2, 2, 1) > 6);
}

static void test_voxel_limits_and_oom(void)
{
    uint8_t one[1] = {1};
    tb_voxels big = {1000, 1000, 1, one, 1.0f, {0, 0, 0}, (const uint8_t(*)[4])g_pal};
    tb_mesh m;
    tb_mesh_init(&m, NULL);
    TEST_ASSERT_FALSE(tb_mesh_add_voxels(&m, &big)); /* slice too large */
    tb_mesh_free(&m);
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, 0);
    tb_voxels v = {1, 1, 1, one, 1.0f, {0, 0, 0}, (const uint8_t(*)[4])g_pal};
    tb_mesh_init(&m, &ta.iface);
    TEST_ASSERT_FALSE(tb_mesh_add_voxels(&m, &v));
}

/* ---- camera ---- */

static void test_camera_projects_forward_point_to_centre(void)
{
    tb_camera c = {{0, 0, 1.6f}, TB_PI / 2, 0.0f, 1.2f, 0.1f, 100.0f};
    tb_vec3 f = tb_camera_forward(&c);
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, 1.0f, f.y);
    tb_mat4 vp = tb_mat4_mul(tb_camera_projection(&c, 16.0f / 9.0f), tb_camera_view(&c));
    tb_vec3 p = tb_mat4_project(vp, tb_v3(0, 10, 1.6f));
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, p.x);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, p.y);
    TEST_ASSERT_TRUE(p.z > -1.0f && p.z < 1.0f);
    tb_vec3 right = tb_mat4_project(vp, tb_v3(2, 10, 1.6f)); /* +x is to the right when facing +y */
    TEST_ASSERT_TRUE(right.x > 0.0f);
    tb_vec3 up = tb_mat4_project(vp, tb_v3(0, 10, 3.0f));
    TEST_ASSERT_TRUE(up.y > 0.0f);
}

static void test_matrix_helpers(void)
{
    tb_mat4 i = tb_mat4_identity();
    tb_mat4 o = tb_mat4_ortho(-1, 1, -1, 1, 0.1f, 10);
    tb_mat4 io = tb_mat4_mul(i, o);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(o.m, io.m, 16);
    tb_vec3 p = tb_mat4_project(o, tb_v3(0.5f, -0.5f, -5.05f));
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, 0.5f, p.x);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, p.z);
    /* Looking straight down along the up vector still gives a valid basis. */
    tb_mat4 down = tb_mat4_look_at(tb_v3(0, 0, 10), tb_v3(0, 0, 0), tb_v3(0, 0, 1));
    TEST_ASSERT_FALSE(isnan(down.m[0]));
    tb_mat4 zero;
    memset(&zero, 0, sizeof(zero));
    tb_vec3 q = tb_mat4_project(zero, tb_v3(1, 2, 3)); /* w = 0: no division */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, q.x);
    float v = tb_fov_horizontal_to_vertical(TB_PI / 2, 16.0f / 9.0f);
    TEST_ASSERT_TRUE(v > 1.0f && v < TB_PI / 2);
}

/* ---- scene ---- */

static void test_scene_from_sim(void)
{
    tb_brush store[TB_TEST_ARENA_BRUSHES];
    tb_geometry g;
    tb_geometry_build_test_arena(&g, store);
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g;
    static tb_sim sim;
    tb_sim_init(&sim, &cfg);
    tb_sim_add_player(&sim, 0);
    tb_sim_add_player(&sim, 1);
    tb_handle t = tb_sim_add_target(&sim, tb_v3(3, 0, 0.9f), 10);
    tb_sim_add_pickup(&sim, TB_PICKUP_BAUBLE, tb_v3(5, 5, 0.5f), 0, 0, false);
    tb_sim_add_pickup(&sim, TB_PICKUP_HEALTH, tb_v3(6, 5, 0.5f), 0, 10, false);
    tb_sim_spawn(&sim, TB_ENT_PROJECTILE, tb_v3(1, 1, 1), tb_v3(0.1f, 0.1f, 0.1f));
    tb_sim_add_trigger(&sim, TB_TRIGGER_EXIT, tb_v3(0, 0, 0), tb_v3(1, 1, 1), 0);
    tb_sim_get(&sim, t)->stun_ticks = 5;

    static tb_scene s;
    tb_scene_init_test_arena(&s);
    int static_lights = s.light_count;
    tb_scene_from_sim(&s, &sim, 0, TB_PI / 2, 16.0f / 9.0f);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, TB_PLAYER_EYE, s.camera.eye.z);
    TEST_ASSERT_EQUAL_INT(5, s.box_count);                   /* teammate, target, 2 pickups, projectile */
    TEST_ASSERT_EQUAL_INT(static_lights + 1, s.light_count); /* projectile glow */
    tb_scene_reset_dynamic(&s, static_lights);
    TEST_ASSERT_EQUAL_INT(0, s.box_count);
    TEST_ASSERT_EQUAL_INT(static_lights, s.light_count);
    tb_scene_reset_dynamic(&s, 99); /* more than present: unchanged */
    tb_scene_reset_dynamic(&s, -3);
    TEST_ASSERT_EQUAL_INT(0, s.light_count);
}

static void test_scene_limits(void)
{
    static tb_scene s;
    memset(&s, 0, sizeof(s));
    for (int i = 0; i < TB_SCENE_MAX_LIGHTS; i++) {
        TEST_ASSERT_TRUE(tb_scene_add_light(&s, tb_v3(0, 0, 0), tb_v3(1, 1, 1), 1));
    }
    TEST_ASSERT_FALSE(tb_scene_add_light(&s, tb_v3(0, 0, 0), tb_v3(1, 1, 1), 1));
    for (int i = 0; i < TB_SCENE_MAX_BOXES; i++) {
        TEST_ASSERT_TRUE(tb_scene_add_box(&s, tb_v3(0, 0, 0), tb_v3(1, 1, 1), WHITE, false));
    }
    TEST_ASSERT_FALSE(tb_scene_add_box(&s, tb_v3(0, 0, 0), tb_v3(1, 1, 1), WHITE, true));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_default_preset_at_1080p);
    RUN_TEST(test_presets_and_odd_windows);
    RUN_TEST(test_split_screen);
    RUN_TEST(test_palette_and_lut);
    RUN_TEST(test_bayer_matrix);
    RUN_TEST(test_box_brush_has_twelve_triangles);
    RUN_TEST(test_arena_mesh_and_colours);
    RUN_TEST(test_mesh_allocation_failure);
    RUN_TEST(test_greedy_voxel_meshing);
    RUN_TEST(test_voxel_limits_and_oom);
    RUN_TEST(test_camera_projects_forward_point_to_centre);
    RUN_TEST(test_matrix_helpers);
    RUN_TEST(test_scene_from_sim);
    RUN_TEST(test_scene_limits);
    return UNITY_END();
}
