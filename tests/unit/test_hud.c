/* SPDX-License-Identifier: MIT */
/* HUD, world view and UI font. */
#include "unity.h"

#include "app/hud.h"
#include "app/world_view.h"
#include "render/ui.h"

#include <string.h>

static tb_app g_app;
static tb_scene g_scene;
static tb_ui g_ui;

static void start(tb_portal_mask open, int day, uint32_t completed, bool playtest)
{
    tb_args a;
    memset(&a, 0, sizeof(a));
    a.headless = true;
    a.playtest = playtest;
    tb_app_init(&g_app, &a);
    tb_save s;
    tb_save_defaults(&s);
    s.completed = completed;
    tb_calendar_input in;
    memset(&in, 0, sizeof(in));
    in.local_clock_utc =
        (tb_days_from_civil(2026, 12, day > 0 ? day : 1) - (day > 0 ? 0 : 3)) * 86400 + 43200;
    in.persisted = open;
    in.playtest = playtest;
    tb_calendar_state c;
    tb_calendar_evaluate(&in, &c);
    tb_app_set_progress(&g_app, &s, &c);
}

static int build(void)
{
    tb_world_scene(&g_app, &g_scene, 1.57f, 16.0f / 9.0f);
    tb_ui_begin(&g_ui, 960, 540);
    tb_hud_build(&g_app, &g_scene, 16.0f / 9.0f, &g_ui);
    return g_ui.count;
}

void setUp(void)
{
    start(0, 7, 0, false);
}
void tearDown(void) {}

/* ---- world view ---- */

static int count_boxes(const uint8_t color[4])
{
    int n = 0;
    for (int i = 0; i < g_scene.box_count; i++) {
        n += memcmp(g_scene.boxes[i].color, color, 3) == 0;
    }
    return n;
}

static int lit_bulbs(void)
{
    int n = 0;
    for (int i = 0; i < g_scene.box_count; i++) {
        const tb_box *b = &g_scene.boxes[i];
        float size = b->maxs.x - b->mins.x;
        n += b->emissive && size < 0.2f; /* bulbs are 0.14 m */
    }
    return n;
}

/* @scenario festive-hall: Three baubles */
static void test_three_baubles_three_rings(void)
{
    start(0x7Fu, 7, (1u << 0) | (1u << 1) | (1u << 3), false);
    build();
    static const uint8_t red[4] = {230, 30, 50, 255}, gold[4] = {255, 185, 40, 255},
                         blue[4] = {60, 120, 255, 255};
    TEST_ASSERT_EQUAL_INT(3, count_boxes(red) + count_boxes(gold) + count_boxes(blue)); /* baubles 1, 2, 4 */
    TEST_ASSERT_EQUAL_INT(3 * TB_TREE_RING_BULBS, lit_bulbs());
}

/* @scenario festive-hall: Restored hall */
static void test_restored_hall_has_top_ornament(void)
{
    start(TB_ALL_PORTALS, 25, TB_ALL_PORTALS, false);
    build();
    static const uint8_t star[4] = {255, 230, 120, 255};
    TEST_ASSERT_EQUAL_INT(1, count_boxes(star));
    TEST_ASSERT_EQUAL_INT(24 * TB_TREE_RING_BULBS, lit_bulbs());
    TEST_ASSERT_TRUE(g_scene.ambient.x > 0.3f); /* warm, fully lit */
}

static void test_open_portals_glow_and_emergency_light(void)
{
    start(TB_ALL_PORTALS, 18, 1u << 16, false);
    int lights_before = 0;
    build();
    lights_before = g_scene.light_count;
    int portals = 0;
    for (int i = 0; i < g_scene.box_count; i++) {
        float h = g_scene.boxes[i].maxs.z - g_scene.boxes[i].mins.z;
        portals += g_scene.boxes[i].emissive && h > 3.0f;
    }
    TEST_ASSERT_EQUAL_INT(24, portals);
    TEST_ASSERT_TRUE(lights_before >= 9); /* tree x4, decor x4, emergency */
}

static void test_world_meshes(void)
{
    tb_mesh m;
    tb_mesh_init(&m, NULL);
    TEST_ASSERT_TRUE(tb_world_mesh_build(&g_app, &m));
    size_t hall_tris = tb_mesh_triangle_count(&m);
    TEST_ASSERT_TRUE(hall_tris > 2000); /* shell, doors, props, voxel tree */
    start(TB_ALL_PORTALS, 7, 0, false);
    TEST_ASSERT_TRUE(tb_world_mesh_build(&g_app, &m));
    TEST_ASSERT_TRUE(tb_mesh_triangle_count(&m) < hall_tris); /* no closed door panels */
    g_app.flow = TB_FLOW_LEVEL; /* levels draw only their geometry: no props, doors or tree */
    TEST_ASSERT_TRUE(tb_world_mesh_build(&g_app, &m));
    TEST_ASSERT_TRUE(tb_mesh_triangle_count(&m) < hall_tris / 2);
    tb_mesh_free(&m);
    tb_voxels v;
    tb_world_tree_voxels(&v);
    TEST_ASSERT_EQUAL_INT(48, v.sz);
}

/* ---- HUD ---- */

static void test_door_texts(void)
{
    start(0x7Fu, 7, 0, false);
    char t[128];
    tb_hud_door_text(&g_app, 7, t, sizeof(t));
    TEST_ASSERT_EQUAL_STRING("DAY 7 - de_december - WALK IN - NEW!", t);
    tb_hud_door_text(&g_app, 3, t, sizeof(t));
    TEST_ASSERT_EQUAL_STRING("DAY 3 - Operation Tinselium - WALK IN", t);
    tb_hud_door_text(&g_app, 9, t, sizeof(t));
    TEST_ASSERT_EQUAL_STRING("DOOR 9 - opens in 1 d 12 h", t);
    g_app.calendar.effective_local = tb_days_from_civil(2026, 12, 9) * 86400; /* due now, not opened yet */
    tb_hud_door_text(&g_app, 9, t, sizeof(t));
    TEST_ASSERT_EQUAL_STRING("DOOR 9 - OPENS SOON", t);
}

static void test_hud_in_every_flow_state(void)
{
    start(0x7Fu, 7, 0, false);
    int hall = build();
    TEST_ASSERT_TRUE(hall > 20);
    g_app.sim.players[0].move.origin = g_app.hall.doors[2].spawn; /* near door 3: info line */
    g_app.sim.players[0].move.yaw = -TB_PI / 2;
    int near = build();
    TEST_ASSERT_TRUE(near > hall);
    g_app.flow = TB_FLOW_BRIEFING;
    g_app.level_day = 11;
    TEST_ASSERT_TRUE(build() > 100);
    g_app.flow = TB_FLOW_LEVEL;
    tb_inventory_give_weapon(&g_app.sim.players[0].inv, TB_WEAPON_TEST_SHOTGUN);
    tb_inventory_select(&g_app.sim.players[0].inv, 3);
    g_app.sim.players[0].vitals.health = 10;
    g_app.sim.players[0].state = TB_PLAYER_DEAD;
    g_app.sim.bauble_collected = true;
    TEST_ASSERT_TRUE(build() > 50);
    g_app.flow = TB_FLOW_RESULTS;
    snprintf(g_app.results, sizeof(g_app.results), "DAY 11\nFloppies 3/3");
    tb_app_say(&g_app, "Hello from NOEL");
    TEST_ASSERT_TRUE(build() > 50);
}

static void test_countdown_before_december(void)
{
    start(0, 0, 0, false);
    int n = build();
    TEST_ASSERT_TRUE(n > 20); /* crosshair, status, "DOOR 1 - opens in ..." */
}

/* @scenario advent-calendar: Playtesting all levels early */
static void test_playtest_opens_all_and_shows_overlay(void)
{
    start(0, 0, 0, true); /* 25 November with ?playtest=1 */
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, g_app.open);
    int with = build();
    g_app.args.playtest = false;
    int without = build();
    TEST_ASSERT_TRUE(with > without + 20); /* the debug overlay line */
}

/* ---- UI / font ---- */

static void test_font_and_text_metrics(void)
{
    const uint8_t *a = tb_font_glyph('A'), *la = tb_font_glyph('a');
    TEST_ASSERT_EQUAL_PTR(a, la);
    TEST_ASSERT_EQUAL_HEX8(14, tb_font_glyph('0')[0]);
    const uint8_t *unknown = tb_font_glyph('~');
    TEST_ASSERT_EQUAL_HEX8(31, unknown[0]);
    TEST_ASSERT_EQUAL_INT(0, tb_ui_text_width("", 1));
    TEST_ASSERT_EQUAL_INT(5, tb_ui_text_width("A", 1));
    TEST_ASSERT_EQUAL_INT(23, tb_ui_text_width("AB\nABCD", 1));
    TEST_ASSERT_EQUAL_INT(2, tb_ui_text_lines("A\nB"));
}

static void test_text_layout_and_overflow(void)
{
    static const uint8_t w[4] = {255, 255, 255, 255};
    tb_ui_begin(&g_ui, 100, 50);
    tb_ui_text(&g_ui, 50, 0, 1, TB_ALIGN_CENTER, w, "I");
    TEST_ASSERT_EQUAL_INT(14, g_ui.count);      /* 7 rows (runs merged), shadow + glyph */
    TEST_ASSERT_EQUAL_INT(49, g_ui.rects[7].x); /* glyph top bar starts 1 px right of the left edge */
    tb_ui_begin(&g_ui, 100, 50);
    tb_ui_text(&g_ui, 100, 0, 2, TB_ALIGN_RIGHT, w, "-\n ");
    TEST_ASSERT_EQUAL_INT(100 - 10, g_ui.rects[1].x);
    tb_ui_text_boxed(&g_ui, 10, 10, 1, TB_ALIGN_LEFT, w, "OK");
    tb_ui_rect_add(&g_ui, 0, 0, 0, 5, w); /* empty: ignored */
    int before = g_ui.count;
    for (int i = 0; i < TB_UI_MAX_RECTS + 10; i++) {
        tb_ui_rect_add(&g_ui, 0, 0, 1, 1, w);
    }
    TEST_ASSERT_TRUE(g_ui.overflow);
    TEST_ASSERT_EQUAL_INT(TB_UI_MAX_RECTS, g_ui.count);
    TEST_ASSERT_TRUE(before < TB_UI_MAX_RECTS);
}

static void test_word_wrap(void)
{
    char out[64];
    tb_ui_wrap("one two three four", 9, out, sizeof(out));
    TEST_ASSERT_EQUAL_STRING("one two\nthree\nfour", out);
    tb_ui_wrap("a\nbb cc", 4, out, sizeof(out));
    TEST_ASSERT_EQUAL_STRING("a\nbb\ncc", out);
    tb_ui_wrap("superlongword x", 4, out, sizeof(out)); /* long words are kept whole */
    TEST_ASSERT_EQUAL_STRING("superlongword\nx", out);
    tb_ui_wrap("abcdef", 10, out, 4); /* truncates to the buffer */
    TEST_ASSERT_EQUAL_STRING("abc", out);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_three_baubles_three_rings);
    RUN_TEST(test_restored_hall_has_top_ornament);
    RUN_TEST(test_open_portals_glow_and_emergency_light);
    RUN_TEST(test_world_meshes);
    RUN_TEST(test_door_texts);
    RUN_TEST(test_hud_in_every_flow_state);
    RUN_TEST(test_countdown_before_december);
    RUN_TEST(test_playtest_opens_all_and_shows_overlay);
    RUN_TEST(test_font_and_text_metrics);
    RUN_TEST(test_text_layout_and_overflow);
    RUN_TEST(test_word_wrap);
    return UNITY_END();
}
