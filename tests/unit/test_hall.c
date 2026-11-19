/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "game/hall.h"
#include "game/levels.h"

#include <string.h>

static tb_hall g_hall;
static tb_brush g_store[TB_HALL_MAX_BRUSHES];
static tb_geometry g_geo;

void setUp(void)
{
    tb_hall_layout(&g_hall);
}
void tearDown(void) {}

static void test_level_table(void)
{
    for (int d = 1; d <= 24; d++) {
        const tb_level_info *l = tb_level_info_get(d);
        TEST_ASSERT_NOT_NULL(l);
        TEST_ASSERT_EQUAL_INT(d, l->day);
        TEST_ASSERT_TRUE(strlen(l->title) > 3 && strlen(l->briefing) > 10 && strlen(l->inspired_by) > 3);
    }
    TEST_ASSERT_EQUAL_STRING("Baublegum City", tb_level_info_get(1)->title);
    TEST_ASSERT_EQUAL_STRING("Code Red-Nose", tb_level_info_get(24)->title);
    TEST_ASSERT_NULL(tb_level_info_get(0));
    TEST_ASSERT_NULL(tb_level_info_get(25));
}

static void test_acts(void)
{
    TEST_ASSERT_EQUAL_INT(TB_ACT_BOOT, tb_level_act(1));
    TEST_ASSERT_EQUAL_INT(TB_ACT_BOOT, tb_level_act(8));
    TEST_ASSERT_EQUAL_INT(TB_ACT_DEFRAG, tb_level_act(9));
    TEST_ASSERT_EQUAL_INT(TB_ACT_DEFRAG, tb_level_act(16));
    TEST_ASSERT_EQUAL_INT(TB_ACT_SHUTDOWN, tb_level_act(17));
    TEST_ASSERT_EQUAL_INT(TB_ACT_BOOT, tb_hall_act(0, 0));
    TEST_ASSERT_EQUAL_INT(TB_ACT_SHUTDOWN, tb_hall_act(25, 0));
    TEST_ASSERT_EQUAL_STRING("DEFRAG", tb_story_act_name(TB_ACT_DEFRAG));
    TEST_ASSERT_EQUAL_STRING("RESTORED", tb_story_act_name(TB_ACT_RESTORED));
    TEST_ASSERT_EQUAL_STRING("?", tb_story_act_name((tb_story_act)7));
}

/* @scenario festive-hall: Shutdown act */
static void test_shutdown_act_lighting(void)
{
    tb_story_act act = tb_hall_act(18, 1u << 16); /* 18 December, level 17 finished */
    TEST_ASSERT_EQUAL_INT(TB_ACT_SHUTDOWN, act);
    tb_hall_mood m = tb_hall_mood_for(act, 1);
    TEST_ASSERT_TRUE(m.emergency);
    TEST_ASSERT_TRUE(m.ambient.x > m.ambient.z); /* red */
    TEST_ASSERT_FALSE(tb_hall_mood_for(TB_ACT_BOOT, 1).emergency);
    TEST_ASSERT_FALSE(tb_hall_mood_for(TB_ACT_DEFRAG, 1).emergency);
}

static void test_restored_act(void)
{
    TEST_ASSERT_EQUAL_INT(TB_ACT_RESTORED, tb_hall_act(24, 1u << 23));
    tb_hall_mood m = tb_hall_mood_for(TB_ACT_RESTORED, 24);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, m.tree_light);
    TEST_ASSERT_TRUE(tb_hall_mood_for(TB_ACT_BOOT, 0).tree_light <
                     tb_hall_mood_for(TB_ACT_BOOT, 12).tree_light);
}

static void test_door_layout(void)
{
    for (int i = 0; i < 24; i++) {
        const tb_hall_door *d = &g_hall.doors[i];
        TEST_ASSERT_EQUAL_INT(i + 1, d->day);
        if (d->day <= 12) {
            TEST_ASSERT_EQUAL_FLOAT(0.0f, d->centre.z); /* ground floor */
            TEST_ASSERT_TRUE(d->normal.y > 0);
        } else {
            TEST_ASSERT_EQUAL_FLOAT(TB_HALL_BALCONY_Z, d->centre.z); /* balcony */
            TEST_ASSERT_TRUE(d->normal.y < 0);
        }
        for (int j = 0; j < i; j++) { /* doors never overlap */
            const tb_hall_door *o = &g_hall.doors[j];
            TEST_ASSERT_FALSE(tb_boxes_overlap(d->vol_min, d->vol_max, o->vol_min, o->vol_max));
        }
    }
}

static void test_geometry_and_closed_doors(void)
{
    TEST_ASSERT_TRUE(tb_hall_build_geometry(&g_hall, 0, &g_geo, g_store));
    int closed_all = g_geo.count;
    TEST_ASSERT_TRUE(tb_hall_build_geometry(&g_hall, TB_ALL_PORTALS, &g_geo, g_store));
    TEST_ASSERT_EQUAL_INT(closed_all - 24, g_geo.count); /* one blocker per closed door */
    /* A closed doorway blocks players; an open one does not. */
    const tb_hall_door *d = &g_hall.doors[6];
    tb_vec3 mins = {-0.3f, -0.3f, 0}, maxs = {0.3f, 0.3f, 1.8f};
    tb_vec3 from = tb_v3(d->centre.x, d->centre.y + 2.0f, 0.05f);
    tb_vec3 to = tb_v3(d->centre.x, d->centre.y + 0.2f, 0.05f);
    tb_trace open = tb_trace_box(&g_geo, from, to, mins, maxs, TB_CONTENTS_SOLID | TB_CONTENTS_PLAYERCLIP);
    tb_hall_build_geometry(&g_hall, 0, &g_geo, g_store);
    tb_trace closed = tb_trace_box(&g_geo, from, to, mins, maxs, TB_CONTENTS_SOLID | TB_CONTENTS_PLAYERCLIP);
    TEST_ASSERT_TRUE(open.fraction > closed.fraction);
}

static void test_spawn_points_are_free(void)
{
    tb_hall_build_geometry(&g_hall, TB_ALL_PORTALS, &g_geo, g_store);
    tb_vec3 mins = {-0.3f, -0.3f, 0}, maxs = {0.3f, 0.3f, 1.8f};
    tb_trace t = tb_trace_box(&g_geo, g_hall.spawn, g_hall.spawn, mins, maxs, TB_CONTENTS_SOLID);
    TEST_ASSERT_FALSE(t.start_solid);
    for (int i = 0; i < 24; i++) {
        tb_vec3 s = g_hall.doors[i].spawn;
        t = tb_trace_box(&g_geo, s, s, mins, maxs, TB_CONTENTS_SOLID | TB_CONTENTS_PLAYERCLIP);
        TEST_ASSERT_FALSE_MESSAGE(t.start_solid, "door spawn inside geometry");
    }
}

static void test_portals_and_nearest_door(void)
{
    static tb_sim sim;
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    tb_hall_build_geometry(&g_hall, 0x7u, &g_geo, g_store);
    cfg.geometry = &g_geo;
    tb_sim_init(&sim, &cfg);
    tb_hall_add_portals(&g_hall, 0x7u, &sim);
    int portals = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        portals += sim.entities[i].type == TB_ENT_TRIGGER && sim.entities[i].kind == TB_TRIGGER_PORTAL;
    }
    TEST_ASSERT_EQUAL_INT(3, portals);
    const tb_hall_door *d = tb_hall_nearest_door(&g_hall, g_hall.doors[4].spawn, 4.0f);
    TEST_ASSERT_NOT_NULL(d);
    TEST_ASSERT_EQUAL_INT(5, d->day);
    TEST_ASSERT_NULL(tb_hall_nearest_door(&g_hall, tb_v3(0, 0, 0), 4.0f));
    /* Standing on the ground floor below a balcony door does not pick it. */
    tb_vec3 below = g_hall.doors[18].spawn;
    below.z = 0;
    TEST_ASSERT_NULL(tb_hall_nearest_door(&g_hall, below, 4.0f));
}

static void test_tree_positions(void)
{
    TEST_ASSERT_EQUAL_INT(0, tb_hall_tree_baubles(0));
    TEST_ASSERT_EQUAL_INT(3, tb_hall_tree_baubles(0x15u));
    TEST_ASSERT_EQUAL_INT(24, tb_hall_tree_baubles(TB_ALL_PORTALS | 0xFF000000u));
    float last_z = -1.0f;
    for (int ring = 0; ring < 24; ring++) {
        tb_vec3 p = tb_hall_ring_bulb_pos(&g_hall, ring, 0);
        TEST_ASSERT_TRUE(p.z > last_z); /* rings go upward */
        TEST_ASSERT_TRUE(p.z < g_hall.tree_height);
        last_z = p.z;
    }
    for (int n = 0; n < 24; n++) {
        tb_vec3 b = tb_hall_bauble_pos(&g_hall, n);
        float dx = b.x - g_hall.tree_base.x, dy = b.y - g_hall.tree_base.y;
        TEST_ASSERT_TRUE(dx * dx + dy * dy < g_hall.tree_radius * g_hall.tree_radius + 1.0f);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_level_table);
    RUN_TEST(test_acts);
    RUN_TEST(test_shutdown_act_lighting);
    RUN_TEST(test_restored_act);
    RUN_TEST(test_door_layout);
    RUN_TEST(test_geometry_and_closed_doors);
    RUN_TEST(test_spawn_points_are_free);
    RUN_TEST(test_portals_and_nearest_door);
    RUN_TEST(test_tree_positions);
    return UNITY_END();
}
