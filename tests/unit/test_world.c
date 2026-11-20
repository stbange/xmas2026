/* SPDX-License-Identifier: MIT */
/* Level wiring in the simulation: names, relays, movers, usables, light
 * groups, shrink zones, hurt volumes, jetpack, Holo-Santa and events. */
#include "unity.h"

#include "sim/sim.h"

#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES + 2];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_input g_in[TB_MAX_PLAYERS];

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    g_geo.cap = TB_TEST_ARENA_BRUSHES + 2;
    memset(g_in, 0, sizeof(g_in));
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = 7;
    cfg.difficulty = TB_DIFF_COLA;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        cfg.spawn[i] = tb_v3(0, (float)i * -2.0f, 0.01f);
    }
    tb_sim_init(&g_sim, &cfg);
    TEST_ASSERT_TRUE(tb_sim_add_player(&g_sim, 0));
}
void tearDown(void) {}

static void step(int ticks)
{
    for (int i = 0; i < ticks; i++) {
        tb_sim_step(&g_sim, g_in);
    }
}

static tb_player *P(int i)
{
    return &g_sim.players[i];
}

static int count_events(tb_event_type type, const char *name)
{
    int n = 0;
    for (int i = 0; i < g_sim.event_count; i++) {
        n += g_sim.events[i].type == type && (!name || strcmp(g_sim.events[i].name, name) == 0);
    }
    return n;
}

static void teleport(int slot, tb_vec3 at)
{
    P(slot)->move.origin = at;
    P(slot)->move.velocity = tb_v3(0, 0, 0);
}

static void press(uint16_t button)
{
    g_in[0].buttons = button;
    step(1);
    g_in[0].buttons = 0;
    step(1);
}

/* ---- names, firing, events -------------------------------------------- */

static void test_names_find_and_fire(void)
{
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(3, 3, 0.9f), 10);
    tb_sim_set_name(&g_sim, t, "dummy", "nothing");
    TEST_ASSERT_EQUAL_UINT32(t, tb_sim_find(&g_sim, "dummy"));
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_find(&g_sim, "missing"));
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_find(&g_sim, "")); /* unnamed entities never match */
    tb_sim_set_name(&g_sim, t, NULL, NULL);
    TEST_ASSERT_EQUAL_STRING("", tb_sim_get(&g_sim, t)->name);
    tb_sim_set_name(&g_sim, 0, "ignored", NULL); /* stale handle: no-op */
    TEST_ASSERT_EQUAL_INT(0, tb_sim_fire(&g_sim, NULL));
    TEST_ASSERT_EQUAL_INT(0, tb_sim_fire(&g_sim, ""));
    TEST_ASSERT_EQUAL_INT(0, tb_sim_fire(&g_sim, "nobody"));
}

static void test_relay_chains_are_depth_limited(void)
{
    /* r0 -> r1 -> ... -> r6: firing r0 reaches only FIRE_DEPTH_MAX deep. */
    char name[8], target[8];
    for (int i = 0; i < 7; i++) {
        snprintf(name, sizeof(name), "r%d", i);
        snprintf(target, sizeof(target), "r%d", i + 1);
        tb_sim_add_relay(&g_sim, tb_v3(15, 15, 0), tb_v3(16, 16, 1), name, target, false);
    }
    TEST_ASSERT_EQUAL_INT(5, tb_sim_fire(&g_sim, "r0"));
}

static void test_event_queue_overflow_is_dropped(void)
{
    g_sim.event_count = 0;
    for (int i = 0; i < TB_MAX_EVENTS + 5; i++) {
        tb_sim_push_event(&g_sim, TB_EVENT_TRIGGER, 0, i, NULL);
    }
    TEST_ASSERT_EQUAL_INT(TB_MAX_EVENTS, g_sim.event_count);
    TEST_ASSERT_EQUAL_STRING("", g_sim.events[0].name);
    step(1);
    TEST_ASSERT_EQUAL_INT(0, g_sim.event_count); /* cleared every tick */
}

/* @scenario level-framework: Relay trigger */
static void test_relay_fires_on_entry_once(void)
{
    tb_sim_add_relay(&g_sim, tb_v3(2, -1, 0), tb_v3(3, 1, 2), "hall", NULL, false);
    tb_sim_add_relay(&g_sim, tb_v3(-3, -1, 0), tb_v3(-2, 1, 2), "once", NULL, true);
    teleport(0, tb_v3(2.5f, 0, 0.01f));
    step(1);
    TEST_ASSERT_EQUAL_INT(1, count_events(TB_EVENT_TRIGGER, "hall"));
    step(1);
    TEST_ASSERT_EQUAL_INT(0, count_events(TB_EVENT_TRIGGER, "hall")); /* only on entry */
    teleport(0, tb_v3(0, 0, 0.01f));
    step(1);
    teleport(0, tb_v3(2.5f, 0, 0.01f));
    step(1);
    TEST_ASSERT_EQUAL_INT(1, count_events(TB_EVENT_TRIGGER, "hall")); /* re-entry fires again */
    teleport(0, tb_v3(-2.5f, 0, 0.01f));
    step(1);
    TEST_ASSERT_EQUAL_INT(1, count_events(TB_EVENT_TRIGGER, "once"));
    teleport(0, tb_v3(0, 0, 0.01f));
    step(1);
    teleport(0, tb_v3(-2.5f, 0, 0.01f));
    step(1);
    TEST_ASSERT_EQUAL_INT(0, count_events(TB_EVENT_TRIGGER, "once"));
}

/* ---- movers -------------------------------------------------------------- */

static int add_door(void)
{
    int brush = g_geo.count;
    TEST_ASSERT_TRUE(tb_geometry_add_box(&g_geo, tb_v3(3, -2, 0), tb_v3(3.5f, 2, 3), TB_CONTENTS_SOLID));
    return brush;
}

/* @scenario level-framework: Door opened by a switch */
static void test_switch_opens_door_and_toggles_lights(void)
{
    int brush = add_door();
    TEST_ASSERT_NOT_EQUAL(0, tb_sim_add_mover(&g_sim, brush, tb_v3(0, 0, 2.9f), 3.0f, "door", false));
    tb_sim_add_usable(&g_sim, TB_USE_SWITCH, tb_v3(1.5f, -0.5f, 1.2f), tb_v3(2, 0.5f, 2), "switch", "door",
                      5);
    /* The closed door blocks shots. */
    TEST_ASSERT_TRUE(tb_trace_ray(&g_geo, tb_v3(0, 0, 1), tb_v3(6, 0, 1), TB_CONTENTS_SOLID).fraction < 1.0f);
    TEST_ASSERT_TRUE(tb_sim_light_on(&g_sim, 5));
    press(TB_BTN_USE);
    TEST_ASSERT_FALSE(tb_sim_light_on(&g_sim, 5));
    TEST_ASSERT_TRUE(P(0)->interactions & (1u << TB_USE_SWITCH));
    step(60);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tb_sim_get(&g_sim, tb_sim_find(&g_sim, "door"))->progress);
    TEST_ASSERT_EQUAL_FLOAT(1.0f,
                            tb_trace_ray(&g_geo, tb_v3(0, 0, 1), tb_v3(6, 0, 1), TB_CONTENTS_SOLID).fraction);
    /* Use again after the cooldown: the door closes. */
    step(30);
    press(TB_BTN_USE);
    TEST_ASSERT_TRUE(tb_sim_light_on(&g_sim, 5));
    step(60);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_sim_get(&g_sim, tb_sim_find(&g_sim, "door"))->progress);
}

static void test_mover_validation_and_open_start(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_mover(&g_sim, -1, tb_v3(0, 0, 1), 1, "x", false));
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_mover(&g_sim, g_geo.count, tb_v3(0, 0, 1), 1, "x", false));
    int brush = add_door();
    g_geo.brushes[brush].count = 7; /* not a box */
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_mover(&g_sim, brush, tb_v3(0, 0, 1), 1, "odd", false));
    g_geo.brushes[brush].count = 6;
    tb_handle h = tb_sim_add_mover(&g_sim, brush, tb_v3(0, 0, 3), 1000, "lift", true);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tb_sim_get(&g_sim, h)->progress);
    TEST_ASSERT_EQUAL_FLOAT(6.0f, g_geo.brushes[brush].planes[4].d); /* starts moved up by 3 */
    tb_sim_fire(&g_sim, "lift");
    step(1); /* fast: there in one tick */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_sim_get(&g_sim, h)->progress);
    tb_handle still = tb_sim_add_mover(&g_sim, brush, tb_v3(0, 0, 0), 1, "still", false);
    tb_sim_fire(&g_sim, "still");
    step(1); /* no offset: snaps */
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tb_sim_get(&g_sim, still)->progress);
}

/* ---- usables -------------------------------------------------------------- */

/* @scenario level-framework: Usable world objects */
static void test_usables_heal_report_and_cool_down(void)
{
    tb_sim_add_usable(&g_sim, TB_USE_VENDING, tb_v3(1.5f, -0.5f, 1.2f), tb_v3(2, 0.5f, 2), "cola", NULL, 0);
    P(0)->vitals.health = 50;
    g_in[0].buttons = TB_BTN_USE;
    step(1);
    TEST_ASSERT_EQUAL_INT(55, P(0)->vitals.health);
    TEST_ASSERT_EQUAL_INT(1, count_events(TB_EVENT_USED, "cola"));
    TEST_ASSERT_EQUAL_INT32(TB_USE_VENDING, g_sim.events[0].value);
    step(1); /* holding Use does not repeat */
    TEST_ASSERT_EQUAL_INT(55, P(0)->vitals.health);
    g_in[0].buttons = 0;
    step(1);
    press(TB_BTN_USE); /* still cooling down */
    TEST_ASSERT_EQUAL_INT(55, P(0)->vitals.health);
    step(40);
    press(TB_BTN_USE);
    TEST_ASSERT_EQUAL_INT(60, P(0)->vitals.health);

    tb_sim_remove(&g_sim, tb_sim_find(&g_sim, "cola"));
    tb_sim_add_usable(&g_sim, TB_USE_TOILET, tb_v3(1.5f, -0.5f, 1.2f), tb_v3(2, 0.5f, 2), "loo", NULL, 0);
    press(TB_BTN_USE);
    TEST_ASSERT_EQUAL_INT(61, P(0)->vitals.health);
    tb_sim_remove(&g_sim, tb_sim_find(&g_sim, "loo"));
    tb_sim_add_usable(&g_sim, TB_USE_ARCADE, tb_v3(1.5f, -0.5f, 1.2f), tb_v3(2, 0.5f, 2), "arcade", NULL, 0);
    press(TB_BTN_USE);
    TEST_ASSERT_TRUE(P(0)->interactions & (1u << TB_USE_ARCADE));
}

static void test_use_needs_range_and_no_wall(void)
{
    tb_sim_add_usable(&g_sim, TB_USE_BUTTON, tb_v3(4.5f, -0.5f, 1.2f), tb_v3(5, 0.5f, 2), "far", NULL, 0);
    press(TB_BTN_USE);
    TEST_ASSERT_EQUAL_UINT32(0, P(0)->interactions); /* 4.5 m away */
    int brush = add_door();
    (void)brush;
    tb_sim_add_usable(&g_sim, TB_USE_BUTTON, tb_v3(3.6f, -0.5f, 1.2f), tb_v3(4, 0.5f, 2), "behind", NULL, 0);
    teleport(0, tb_v3(2.2f, 0, 0.01f));
    press(TB_BTN_USE);
    TEST_ASSERT_EQUAL_UINT32(0, P(0)->interactions); /* the door is in the way */
}

/* ---- volumes ------------------------------------------------------------- */

/* @scenario homage-levels: Shrink zone */
static void test_shrink_zone_and_growing_back(void)
{
    tb_sim_add_trigger(&g_sim, TB_TRIGGER_SHRINK, tb_v3(-12, -12, 0), tb_v3(-6, -6, 1), 0);
    teleport(0, tb_v3(-10, -10, 0.01f)); /* under the 1.3 m crawl space */
    step(1);
    TEST_ASSERT_TRUE(P(0)->move.shrunk);
    TEST_ASSERT_TRUE(P(0)->in_shrink_zone);
    /* Leave the zone while still under the low ceiling: stays small. */
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (g_sim.entities[i].type == TB_ENT_TRIGGER) {
            g_sim.entities[i].type = TB_ENT_NONE;
        }
    }
    step(1);
    TEST_ASSERT_FALSE(P(0)->in_shrink_zone);
    TEST_ASSERT_TRUE(P(0)->move.shrunk);
    teleport(0, tb_v3(0, 0, 0.01f));
    step(1);
    TEST_ASSERT_FALSE(P(0)->move.shrunk);
}

static void test_hurt_volume_damages_every_tick(void)
{
    tb_entity *e =
        tb_sim_get(&g_sim, tb_sim_add_trigger(&g_sim, TB_TRIGGER_HURT, tb_v3(2, -1, 0), tb_v3(3, 1, 2), 0));
    e->value = 10;
    teleport(0, tb_v3(2.5f, 0, 0.01f));
    step(2);
    TEST_ASSERT_EQUAL_INT(80, P(0)->vitals.health);
    e->value = 1000;
    step(1);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DEAD, P(0)->state);
}

/* ---- items ---------------------------------------------------------------- */

/* @scenario homage-levels: Jetpack */
static void test_jetpack_burns_fuel_and_steers(void)
{
    g_in[0].buttons = TB_BTN_ABILITY1;
    step(5);
    TEST_ASSERT_TRUE(P(0)->move.origin.z < 0.1f); /* no jetpack yet */
    tb_sim_add_pickup(&g_sim, TB_PICKUP_ITEM, tb_v3(0, 0, 0.5f), TB_ITEM_JETPACK, 0, true);
    step(1);
    TEST_ASSERT_TRUE(P(0)->inv.items[TB_ITEM_JETPACK]);
    step(60);
    TEST_ASSERT_TRUE(P(0)->move.origin.z > 1.0f);
    TEST_ASSERT_TRUE(P(0)->move.velocity.z <= 6.0f);
    TEST_ASSERT_TRUE(P(0)->jet_fuel < TB_JET_FUEL_MAX);
    TEST_ASSERT_TRUE(P(0)->jetting);
    g_in[0].forward = 127;
    step(30);
    TEST_ASSERT_TRUE(P(0)->move.velocity.x > 2.0f); /* real air control while thrusting */
    P(0)->jet_fuel = 0;
    step(1);
    TEST_ASSERT_FALSE(P(0)->jetting);
}

/* @scenario homage-levels: Holo-Santa decoy */
static void test_holo_santa_decoy_and_cooldown(void)
{
    press(TB_BTN_ABILITY2);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_ITEM, tb_v3(0, 0, 0.5f), TB_ITEM_HOLO_SANTA, 0, true);
    step(1);
    int decoys = 0;
    press(TB_BTN_ABILITY2);
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        decoys += g_sim.entities[i].type == TB_ENT_DECOY;
    }
    TEST_ASSERT_EQUAL_INT(1, decoys);
    TEST_ASSERT_TRUE(P(0)->decoy_cooldown > 0);
    press(TB_BTN_ABILITY2); /* recharging */
    decoys = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        decoys += g_sim.entities[i].type == TB_ENT_DECOY;
    }
    TEST_ASSERT_EQUAL_INT(1, decoys);
    step((int)TB_DECOY_TICKS);
    decoys = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        decoys += g_sim.entities[i].type == TB_ENT_DECOY;
    }
    TEST_ASSERT_EQUAL_INT(0, decoys); /* the hologram fades */
    step((int)TB_DECOY_COOLDOWN);
    TEST_ASSERT_EQUAL_UINT32(0, P(0)->decoy_cooldown);
}

static void test_pickup_events_and_light_group_bounds(void)
{
    tb_handle h = tb_sim_add_pickup(&g_sim, TB_PICKUP_HEALTH, tb_v3(0, 0, 0.5f), 0, 10, false);
    tb_sim_set_name(&g_sim, h, "medkit", NULL);
    P(0)->vitals.health = 50;
    step(1);
    TEST_ASSERT_EQUAL_INT(1, count_events(TB_EVENT_PICKUP, "medkit"));
    tb_sim_set_light(&g_sim, -1, false);
    tb_sim_set_light(&g_sim, TB_LIGHT_GROUPS, false);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFFu, g_sim.light_groups);
    TEST_ASSERT_FALSE(tb_sim_light_on(&g_sim, -1));
    TEST_ASSERT_FALSE(tb_sim_light_on(&g_sim, TB_LIGHT_GROUPS));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_names_find_and_fire);
    RUN_TEST(test_relay_chains_are_depth_limited);
    RUN_TEST(test_event_queue_overflow_is_dropped);
    RUN_TEST(test_relay_fires_on_entry_once);
    RUN_TEST(test_switch_opens_door_and_toggles_lights);
    RUN_TEST(test_mover_validation_and_open_start);
    RUN_TEST(test_usables_heal_report_and_cool_down);
    RUN_TEST(test_use_needs_range_and_no_wall);
    RUN_TEST(test_shrink_zone_and_growing_back);
    RUN_TEST(test_hurt_volume_damages_every_tick);
    RUN_TEST(test_jetpack_burns_fuel_and_steers);
    RUN_TEST(test_holo_santa_decoy_and_cooldown);
    RUN_TEST(test_pickup_events_and_light_group_bounds);
    return UNITY_END();
}
