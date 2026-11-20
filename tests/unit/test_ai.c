/* SPDX-License-Identifier: MIT */
/* Enemies (sim/ai.c) and the day-1 weapons that act on them: Pipe
 * Baubles and the Gift-Shrinker. */
#include "unity.h"

#include "sim/sim.h"

#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_input g_in[TB_MAX_PLAYERS];

static void make(tb_difficulty diff)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    memset(g_in, 0, sizeof(g_in));
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = 3;
    cfg.difficulty = diff;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        cfg.spawn[i] = tb_v3(0, (float)i * -2.0f, 0.01f);
    }
    tb_sim_init(&g_sim, &cfg);
    TEST_ASSERT_TRUE(tb_sim_add_player(&g_sim, 0));
}

void setUp(void)
{
    make(TB_DIFF_COLA);
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

static tb_entity *E(tb_handle h)
{
    return tb_sim_get(&g_sim, h);
}

static void test_defs_and_spawning(void)
{
    TEST_ASSERT_NULL(tb_enemy_get(TB_ENEMY_KIND_COUNT));
    TEST_ASSERT_NULL(tb_enemy_get((tb_enemy_kind)-1));
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_enemy(&g_sim, TB_ENEMY_KIND_COUNT, tb_v3(0, 0, 0), NULL));
    for (int k = 0; k < TB_ENEMY_KIND_COUNT; k++) {
        const tb_enemy_def *d = tb_enemy_get((tb_enemy_kind)k);
        TEST_ASSERT_TRUE(d->health > 0);
        TEST_ASSERT_TRUE(d->sight > 0);
    }
    TEST_ASSERT_TRUE(tb_enemy_get(TB_ENEMY_BATTLELORD)->shrink_immune);
    TEST_ASSERT_TRUE(tb_enemy_get(TB_ENEMY_BRAIN)->flying);
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(10, 10, 0), "pig");
    TEST_ASSERT_EQUAL_INT(1, tb_sim_enemy_count(&g_sim));
    TEST_ASSERT_EQUAL_STRING("pig", E(h)->name);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.95f, E(h)->origin.z); /* origin is the box centre */
}

static void test_idle_enemy_stands_until_it_sees_someone(void)
{
    P(0)->move.origin = tb_v3(-15, -15, 0.01f);
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(15, -15, 0), NULL);
    tb_sim_get(&g_sim, h)->move.origin.z = 1.0f; /* dropped in from above: falls */
    P(0)->state = TB_PLAYER_DEAD;                /* nobody to see */
    step(60);
    TEST_ASSERT_EQUAL_UINT8(TB_AI_IDLE, E(h)->ai_state);
    TEST_ASSERT_TRUE(E(h)->move.origin.z < 0.1f);
}

/* @scenario homage-levels: Hogwash Trooper ambush */
static void test_trooper_chases_and_shoots(void)
{
    P(0)->move.origin = tb_v3(0, -8, 0.01f);
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(15, -8, 0), NULL);
    float start = E(h)->origin.x;
    step(600);
    TEST_ASSERT_EQUAL_UINT8(TB_AI_CHASE, E(h)->ai_state);
    TEST_ASSERT_EQUAL_INT8(0, E(h)->ai_target);
    TEST_ASSERT_TRUE(E(h)->origin.x < start); /* closed in */
    TEST_ASSERT_TRUE(P(0)->vitals.health < 100);
}

static void test_lizard_teleports_and_shoots_plasma(void)
{
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_LIZARD, tb_v3(12, 12, 0), NULL);
    tb_vec3 before = E(h)->origin;
    bool saw_plasma = false;
    for (int t = 0; t < 400; t++) {
        step(1);
        for (int i = 0; i < TB_MAX_ENTITIES; i++) {
            saw_plasma |= g_sim.entities[i].type == TB_ENT_PROJECTILE && g_sim.entities[i].from_enemy;
        }
    }
    TEST_ASSERT_TRUE(saw_plasma);
    TEST_ASSERT_TRUE(tb_v3_len(tb_v3_sub(E(h)->origin, before)) > 3.0f);
    TEST_ASSERT_TRUE(P(0)->vitals.health < 100);
}

static void test_brain_flies_towards_the_player(void)
{
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_BRAIN, tb_v3(14, 8, 2), NULL);
    step(120);
    TEST_ASSERT_TRUE(E(h)->origin.x < 14.0f);
    TEST_ASSERT_TRUE(E(h)->origin.y < 8.0f);
}

static void test_battlelord_mortar_and_chaingun(void)
{
    tb_handle h = tb_sim_add_enemy(&g_sim, TB_ENEMY_BATTLELORD, tb_v3(14, 0, 0), "boss");
    bool lobbed = false;
    for (int t = 0; t < 400 && !lobbed; t++) {
        step(1);
        for (int i = 0; i < TB_MAX_ENTITIES; i++) {
            tb_entity *e = &g_sim.entities[i];
            lobbed |= e->type == TB_ENT_PROJECTILE && e->from_enemy && e->gravity && e->splash_damage > 0;
        }
    }
    TEST_ASSERT_TRUE(lobbed);
    TEST_ASSERT_NOT_NULL(E(h));
    step(300);
    TEST_ASSERT_TRUE(P(0)->vitals.health < 100);
}

/* @scenario homage-levels: Boss shrugs off the shrinker */
static void test_shrinker_shrinks_but_not_the_boss(void)
{
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_GIFT_SHRINKER);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_CELLS, 10);
    tb_inventory_select(&P(0)->inv, 3);
    P(0)->move.origin = tb_v3(0, -5, 0.01f);
    tb_handle t = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(6, -5, 0), NULL);
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    TEST_ASSERT_TRUE(E(t)->shrink_ticks > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.4f * 0.3f, E(t)->maxs.x);
    /* Shrunk enemies flee; then grow back. */
    float x = E(t)->origin.x;
    step(60);
    TEST_ASSERT_TRUE(E(t)->origin.x > x);
    step(400);
    TEST_ASSERT_EQUAL_UINT32(0, E(t)->shrink_ticks);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.4f, E(t)->maxs.x);
    tb_sim_remove(&g_sim, t);

    tb_handle b = tb_sim_add_enemy(&g_sim, TB_ENEMY_BATTLELORD, tb_v3(6, -5, 0), NULL);
    P(0)->move.origin = tb_v3(0, -5, 0.01f);
    P(0)->move.yaw = 0;
    step(40);
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    TEST_ASSERT_EQUAL_UINT32(0, E(b)->shrink_ticks);
}

/* @scenario homage-levels: Shrink and stomp */
static void test_stomping_a_shrunken_enemy(void)
{
    tb_handle t = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(1.0f, 0, 0), "squish");
    E(t)->shrink_ticks = 300;
    E(t)->mins = tb_v3_scale(E(t)->base_half, -0.3f);
    E(t)->maxs = tb_v3_scale(E(t)->base_half, 0.3f);
    E(t)->origin = tb_v3(0.3f, 0, 0.3f);
    E(t)->move.origin = tb_v3(0.3f, 0, 0);
    step(1);
    TEST_ASSERT_NULL(E(t));
    int killed = 0;
    for (int i = 0; i < g_sim.event_count; i++) {
        killed += g_sim.events[i].type == TB_EVENT_KILLED && strcmp(g_sim.events[i].name, "squish") == 0;
    }
    TEST_ASSERT_EQUAL_INT(1, killed);
}

/* @scenario homage-levels: Holo-Santa distraction */
static void test_enemies_prefer_the_hologram(void)
{
    tb_handle t = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(12, 0, 0), NULL);
    tb_handle d = tb_sim_spawn(&g_sim, TB_ENT_DECOY, tb_v3(6, 4, 0.9f), tb_v3(0.3f, 0.3f, 0.9f));
    E(d)->ttl = 600;
    step(30);
    TEST_ASSERT_EQUAL_INT8(-2, E(t)->ai_target);
    step(300); /* shoots the hologram until it pops */
    TEST_ASSERT_NULL(E(d));
    step(30);
    TEST_ASSERT_EQUAL_INT8(0, E(t)->ai_target); /* back to the player */
}

static void test_enemy_projectiles_pop_holograms(void)
{
    tb_handle d = tb_sim_spawn(&g_sim, TB_ENT_DECOY, tb_v3(5, 0, 0.9f), tb_v3(0.3f, 0.3f, 0.9f));
    E(d)->ttl = 600;
    tb_sim_add_enemy(&g_sim, TB_ENEMY_LIZARD, tb_v3(12, 0, 0), NULL);
    step(400);
    TEST_ASSERT_NULL(E(d));
}

static void test_stunned_enemy_does_nothing(void)
{
    tb_handle t = tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(8, 0, 0), NULL);
    E(t)->stun_ticks = 100;
    tb_vec3 at = E(t)->origin;
    step(50);
    TEST_ASSERT_EQUAL_FLOAT(at.x, E(t)->origin.x);
    TEST_ASSERT_EQUAL_INT(100, P(0)->vitals.health);
}

/* @scenario homage-levels: Pipe bauble remote detonation */
static void test_pipe_baubles_bounce_rest_and_detonate(void)
{
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_PIPE_BAUBLES);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_PIPES, 3);
    tb_inventory_select(&P(0)->inv, 3);
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(5, 0, 0.9f), 100);
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(180); /* thrown, bounced, resting on the floor; no contact detonation */
    tb_entity *pipe = NULL;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (g_sim.entities[i].type == TB_ENT_PROJECTILE) {
            pipe = &g_sim.entities[i];
        }
    }
    TEST_ASSERT_NOT_NULL(pipe);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_v3_len(pipe->velocity));
    TEST_ASSERT_EQUAL_INT(100, E(t)->vitals.health);
    g_in[0].buttons = TB_BTN_ALT_FIRE;
    step(1);
    TEST_ASSERT_TRUE(E(t) == NULL || E(t)->vitals.health < 100);
    TEST_ASSERT_TRUE(g_sim.shake_ticks > 0);
}

static void test_unattended_pipe_bauble_goes_off_eventually(void)
{
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_PIPE_BAUBLES);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_PIPES, 1);
    tb_inventory_select(&P(0)->inv, 3);
    P(0)->move.pitch = 0.6f; /* up at the ceiling: bounces back down */
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(20 * 60 + 5);
    int pipes = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        pipes += g_sim.entities[i].type == TB_ENT_PROJECTILE;
    }
    TEST_ASSERT_EQUAL_INT(0, pipes);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_defs_and_spawning);
    RUN_TEST(test_idle_enemy_stands_until_it_sees_someone);
    RUN_TEST(test_trooper_chases_and_shoots);
    RUN_TEST(test_lizard_teleports_and_shoots_plasma);
    RUN_TEST(test_brain_flies_towards_the_player);
    RUN_TEST(test_battlelord_mortar_and_chaingun);
    RUN_TEST(test_shrinker_shrinks_but_not_the_boss);
    RUN_TEST(test_stomping_a_shrunken_enemy);
    RUN_TEST(test_enemies_prefer_the_hologram);
    RUN_TEST(test_enemy_projectiles_pop_holograms);
    RUN_TEST(test_stunned_enemy_does_nothing);
    RUN_TEST(test_pipe_baubles_bounce_rest_and_detonate);
    RUN_TEST(test_unattended_pipe_bauble_goes_off_eventually);
    return UNITY_END();
}
