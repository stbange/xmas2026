/* SPDX-License-Identifier: MIT */
/* Renderer tests with a real OpenGL context (CI: Xvfb + Mesa llvmpipe).
 * Without a usable display/GL 3.3 the tests are ignored, not failed. */
#include "unity.h"

#include "render/gl/renderer.h"
#include "sim/sim.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SDL_Window *g_window;
static SDL_GLContext g_ctx;
static tb_renderer g_r;
static bool g_have_gl;
static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_scene g_scene;

static void *proc(const char *name)
{
    SDL_FunctionPointer fn = SDL_GL_GetProcAddress(name);
    void *p;
    memcpy(&p, &fn, sizeof(p));
    return p;
}

static void *missing_proc(const char *name)
{
    return strcmp(name, "glTexImage3D") == 0 ? NULL : proc(name);
}

void setUp(void)
{
    if (!g_have_gl) {
        TEST_IGNORE_MESSAGE("no display with OpenGL 3.3 available");
    }
}
void tearDown(void) {}

static void open_gl(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    g_window = SDL_CreateWindow("renderer test", 640, 360, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    g_ctx = g_window ? SDL_GL_CreateContext(g_window) : NULL;
    g_have_gl = g_ctx != NULL;
}

static void make_world(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.spawn[0] = tb_v3(0, -15, 0.01f);
    cfg.spawn_yaw = TB_PI / 2;
    tb_sim_init(&g_sim, &cfg);
    tb_sim_add_player(&g_sim, 0);
    tb_sim_add_player(&g_sim, 1);
    g_sim.players[1].move.origin = tb_v3(3, -9, 0.01f);
    tb_sim_add_target(&g_sim, tb_v3(0, -8, 0.9f), 50);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_BAUBLE, tb_v3(-2, -10, 0.5f), 0, 0, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_HEALTH, tb_v3(2, -10, 0.5f), 0, 25, false);
    tb_handle shot = tb_sim_spawn(&g_sim, TB_ENT_PROJECTILE, tb_v3(0, -12, 1.6f), tb_v3(0.1f, 0.1f, 0.1f));
    tb_sim_get(&g_sim, shot)->ttl = 100;
    tb_sim_add_trigger(&g_sim, TB_TRIGGER_EXIT, tb_v3(10, 10, 0), tb_v3(12, 12, 2), 0);
    tb_scene_init_test_arena(&g_scene);
    tb_scene_from_sim(&g_scene, &g_sim, 0, 90.0f * TB_PI / 180.0f, 16.0f / 9.0f);
}

static bool upload_world(void)
{
    static const uint8_t f[4] = {120, 110, 100, 255}, w[4] = {150, 140, 170, 255}, c[4] = {70, 70, 90, 255};
    tb_mesh mesh;
    tb_mesh_init(&mesh, NULL);
    bool ok = tb_mesh_add_geometry(&mesh, &g_geo, f, w, c) && tb_renderer_upload_world(&g_r, &mesh);
    tb_mesh_free(&mesh);
    return ok;
}

static void write_ppm(const char *path, const uint8_t *rgba, int w, int h)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        return;
    }
    fprintf(fp, "P6\n%d %d\n255\n", w, h);
    for (int y = h - 1; y >= 0; y--) { /* GL rows are bottom-up */
        for (int x = 0; x < w; x++) {
            fwrite(&rgba[(y * w + x) * 4], 1, 3, fp);
        }
    }
    fclose(fp);
}

static uint8_t *render(tb_view_layout *layout)
{
    *layout = tb_view_layout_for(1920, 1080, TB_RES_CLASSIC);
    tb_renderer_draw(&g_r, &g_scene, layout, 640, 360);
    uint8_t *px = malloc((size_t)layout->internal_w * (size_t)layout->internal_h * 4);
    TEST_ASSERT_TRUE(tb_renderer_read_scene(&g_r, layout, px));
    return px;
}

static void test_create_and_render_scene(void)
{
    TEST_ASSERT_TRUE_MESSAGE(tb_renderer_create(&g_r, proc, false), g_r.error);
    TEST_ASSERT_TRUE(upload_world());
    tb_view_layout layout;
    uint8_t *px = render(&layout);
    write_ppm(TB_TEST_TMP_DIR "/renderer-arena.ppm", px, layout.internal_w, layout.internal_h);
    /* The image is not uniform: there is a lit scene. */
    int distinct = 0;
    uint32_t seen[64];
    for (int i = 0; i < layout.internal_w * layout.internal_h && distinct < 64; i += 97) {
        uint32_t c = (uint32_t)px[i * 4] << 16 | (uint32_t)px[i * 4 + 1] << 8 | px[i * 4 + 2];
        bool known = false;
        for (int k = 0; k < distinct; k++) {
            known |= seen[k] == c;
        }
        if (!known) {
            seen[distinct++] = c;
        }
    }
    TEST_ASSERT_TRUE(distinct >= 8);
    free(px);
}

/* @scenario retro-rendering: Quantised output */
static void test_output_uses_only_palette_colours(void)
{
    tb_palette pal;
    tb_palette_default(&pal);
    tb_view_layout layout;
    uint8_t *px = render(&layout);
    for (int i = 0; i < layout.internal_w * layout.internal_h; i++) {
        tb_rgb c = {px[i * 4], px[i * 4 + 1], px[i * 4 + 2]};
        tb_rgb n = pal.colors[tb_palette_nearest(&pal, c)];
        TEST_ASSERT_TRUE_MESSAGE(n.r == c.r && n.g == c.g && n.b == c.b, "pixel not in palette");
    }
    free(px);
}

static void test_options_change_the_image(void)
{
    tb_view_layout layout;
    uint8_t *with = render(&layout);
    g_r.options.palette_quantize = false;
    g_r.options.shadows = false;
    uint8_t *without = render(&layout);
    TEST_ASSERT_TRUE(memcmp(with, without, (size_t)layout.internal_w * (size_t)layout.internal_h * 4) != 0);
    g_r.options.palette_quantize = true;
    g_r.options.dithering = false;
    uint8_t *flat = render(&layout);
    TEST_ASSERT_TRUE(memcmp(with, flat, (size_t)layout.internal_w * (size_t)layout.internal_h * 4) != 0);
    g_r.options.dithering = true;
    g_r.options.shadows = true;
    free(with);
    free(without);
    free(flat);
}

static void test_resize_and_read_mismatch(void)
{
    tb_view_layout small = tb_view_layout_for(640, 360, TB_RES_CRUNCHY);
    tb_renderer_draw(&g_r, &g_scene, &small, 640, 360);
    TEST_ASSERT_EQUAL_INT(320, g_r.scene_w);
    tb_view_layout other = tb_view_layout_for(1920, 1080, TB_RES_CRISP);
    uint8_t dummy[4];
    TEST_ASSERT_FALSE(tb_renderer_read_scene(&g_r, &other, dummy));
}

static void test_empty_scene_and_custom_palette(void)
{
    tb_scene empty;
    memset(&empty, 0, sizeof(empty));
    empty.camera.fov_y = 1.0f;
    empty.camera.near_z = 0.1f;
    empty.camera.far_z = 10.0f;
    empty.fog_color = tb_v3(1, 0, 0);
    tb_palette two = {{{0, 0, 0}, {255, 0, 0}}, 2};
    tb_renderer_set_palette(&g_r, &two);
    tb_renderer_upload_world(&g_r, &(tb_mesh){{NULL, NULL, 0, 0, sizeof(tb_vertex)}});
    tb_view_layout layout = tb_view_layout_for(320, 180, TB_RES_CRUNCHY);
    tb_renderer_draw(&g_r, &empty, &layout, 320, 180);
    uint8_t *px = malloc((size_t)layout.internal_w * (size_t)layout.internal_h * 4);
    TEST_ASSERT_TRUE(tb_renderer_read_scene(&g_r, &layout, px));
    TEST_ASSERT_EQUAL_UINT8(255, px[0]); /* fog colour, quantised to red */
    TEST_ASSERT_EQUAL_UINT8(0, px[1]);
    free(px);
    tb_renderer_destroy(&g_r);
}

static void test_missing_function_is_reported(void)
{
    tb_renderer r;
    TEST_ASSERT_FALSE(tb_renderer_create(&r, missing_proc, false));
    TEST_ASSERT_NOT_NULL(strstr(r.error, "glTexImage3D"));
    tb_renderer_destroy(&r);
}

static void test_shader_errors_are_reported(void)
{
    tb_renderer r;
    /* Asking for the ES dialect on a desktop core context fails to compile. */
    if (tb_renderer_create(&r, proc, true)) {
        tb_renderer_destroy(&r);
        TEST_IGNORE_MESSAGE("driver accepts GLSL ES 3.00 on desktop");
    }
    TEST_ASSERT_NOT_NULL(strstr(r.error, "shader"));
    tb_renderer_destroy(&r);
}

int main(void)
{
    open_gl();
    make_world();
    UNITY_BEGIN();
    RUN_TEST(test_create_and_render_scene);
    RUN_TEST(test_output_uses_only_palette_colours);
    RUN_TEST(test_options_change_the_image);
    RUN_TEST(test_resize_and_read_mismatch);
    RUN_TEST(test_empty_scene_and_custom_palette);
    RUN_TEST(test_missing_function_is_reported);
    RUN_TEST(test_shader_errors_are_reported);
    int result = UNITY_END();
    if (g_ctx) {
        SDL_GL_DestroyContext(g_ctx);
    }
    if (g_window) {
        SDL_DestroyWindow(g_window);
    }
    SDL_Quit();
    return result;
}
