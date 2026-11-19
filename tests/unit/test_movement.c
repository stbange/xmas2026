/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/movement.h"

#include <string.h>

#define DT        (1.0f / 60.0f)
#define YAW_EAST  0
#define YAW_NORTH 16384

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_move_params g_p;

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_move_default_params(&g_p);
}
void tearDown(void) {}

static tb_move_state at(float x, float y, float z)
{
    tb_move_state s;
    memset(&s, 0, sizeof(s));
    s.origin = tb_v3(x, y, z);
    s.ground_normal = tb_v3(0, 0, 1);
    return s;
}

static tb_input input(int8_t fwd, int8_t side, uint16_t buttons, int16_t yaw)
{
    tb_input in;
    memset(&in, 0, sizeof(in));
    in.forward = fwd;
    in.side = side;
    in.buttons = buttons;
    in.yaw = yaw;
    return in;
}

static void run(tb_move_state *s, tb_input in, int ticks)
{
    for (int i = 0; i < ticks; i++) {
        tb_move_step(s, &in, &g_p, &g_geo, DT);
    }
}

static void test_walk_reaches_walk_speed_and_stops(void)
{
    tb_move_state s = at(-15, -16, 0.01f);
    run(&s, input(127, 0, 0, YAW_EAST), 60);
    TEST_ASSERT_TRUE(s.on_ground);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, g_p.walk_speed, tb_move_ground_speed(&s));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, -16.0f, s.origin.y);
    run(&s, input(0, 0, 0, YAW_EAST), 60);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_move_ground_speed(&s));
}

static void test_sprint_and_crouch_speeds(void)
{
    tb_move_state s = at(-15, -16, 0.01f);
    run(&s, input(127, 0, TB_BTN_SPRINT, YAW_EAST), 60);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, g_p.walk_speed * g_p.sprint_factor, tb_move_ground_speed(&s));
    s = at(-15, -16, 0.01f);
    run(&s, input(127, 0, TB_BTN_CROUCH | TB_BTN_SPRINT, YAW_EAST), 60);
    TEST_ASSERT_TRUE(s.crouched);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, g_p.walk_speed * g_p.crouch_factor, tb_move_ground_speed(&s));
    TEST_ASSERT_EQUAL_FLOAT(TB_PLAYER_CROUCH_EYE, tb_move_eye_height(&s));
}

static void test_jump_and_jump_hold(void)
{
    tb_move_state s = at(0, -10, 0.01f);
    run(&s, input(0, 0, 0, 0), 2);
    run(&s, input(0, 0, TB_BTN_JUMP, 0), 1);
    TEST_ASSERT_FALSE(s.on_ground);
    float top = 0;
    for (int i = 0; i < 60; i++) {
        run(&s, input(0, 0, TB_BTN_JUMP, 0), 1);
        top = s.origin.z > top ? s.origin.z : top;
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.9f, top); /* v^2 / 2g = 36 / 40 */
    TEST_ASSERT_TRUE(s.on_ground);
    TEST_ASSERT_TRUE(s.origin.z < 0.01f); /* holding jump does not jump again */
    run(&s, input(0, 0, 0, 0), 1);
    run(&s, input(0, 0, TB_BTN_JUMP, 0), 1);
    TEST_ASSERT_FALSE(s.on_ground);
}

static void test_steps_up_stairs_but_not_ledges(void)
{
    tb_move_state s = at(2, 0, 0.01f);
    float top = 0;
    for (int i = 0; i < 150; i++) {
        run(&s, input(127, 0, 0, YAW_EAST), 1);
        top = s.origin.z > top ? s.origin.z : top;
    }
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 0.4f, top);   /* climbed the 0.4 m step */
    TEST_ASSERT_TRUE(s.origin.x < 10.0f - 0.29f); /* stopped by the 1 m ledge */
    TEST_ASSERT_TRUE(s.origin.x > 9.0f);
}

static void test_slides_along_walls(void)
{
    tb_move_state s = at(18, -5, 0.01f);
    run(&s, input(127, 0, 0, 8192), 60); /* 45 degrees into the east wall */
    TEST_ASSERT_TRUE(s.origin.x <= 19.71f);
    TEST_ASSERT_TRUE(s.origin.y > -2.0f); /* kept moving north along the wall */
}

static void test_walks_up_ramp(void)
{
    tb_move_state s = at(0, 8, 0.01f);
    run(&s, input(127, 0, 0, YAW_NORTH), 60);
    TEST_ASSERT_TRUE(s.on_ground);
    TEST_ASSERT_TRUE(s.origin.z > 1.0f);
    /* The box rests on its front-bottom edge (y + 0.3) on an upward slope. */
    TEST_ASSERT_FLOAT_WITHIN(0.1f, (s.origin.y + 0.3f - 10.0f) / 2.0f, s.origin.z);
}

static void test_cannot_stand_under_low_ceiling(void)
{
    tb_move_state s = at(-10, -10, 0.01f);
    run(&s, input(0, 0, TB_BTN_CROUCH, YAW_EAST), 2);
    TEST_ASSERT_TRUE(s.crouched);
    run(&s, input(0, 0, 0, YAW_EAST), 2);
    TEST_ASSERT_TRUE(s.crouched); /* ceiling at 1.3 m */
    run(&s, input(127, 0, 0, YAW_EAST), 120);
    TEST_ASSERT_FALSE(s.crouched); /* out of the crawl space */
    TEST_ASSERT_EQUAL_FLOAT(TB_PLAYER_EYE, tb_move_eye_height(&s));
}

static void test_air_control_gains_speed_sideways(void)
{
    /* Strafe-jumping principle: accelerating sideways in the air raises
     * the total speed above the ground limit. */
    tb_move_state s = at(-15, -15, 3.0f);
    s.velocity = tb_v3(8, 0, 0);
    run(&s, input(0, -127, 0, YAW_EAST), 10); /* side < 0: strafe left (+y) */
    TEST_ASSERT_FALSE(s.on_ground);
    TEST_ASSERT_TRUE(tb_move_ground_speed(&s) > 8.0f);
}

static void test_fall_speed_is_capped(void)
{
    tb_move_state s = at(0, 0, 3.0f);
    g_p.gravity = 5000.0f; /* 83 m/s in one tick: capped to 60 */
    run(&s, input(0, 0, 0, 0), 1);
    TEST_ASSERT_EQUAL_FLOAT(-g_p.max_fall_speed, s.velocity.z);
}

static void test_stuck_in_solid_does_not_move(void)
{
    tb_move_state s = at(-5, 6, 1.0f); /* inside the pillar */
    s.velocity = tb_v3(3, 0, 0);
    run(&s, input(0, 0, 0, 0), 1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -5.0f, s.origin.x);
}

static void test_wedged_in_corner_stops(void)
{
    tb_move_state s = at(19.6f, 19.6f, 0.01f);
    run(&s, input(127, 0, 0, 8192), 30); /* straight into the north-east corner */
    TEST_ASSERT_TRUE(s.origin.x <= 19.71f && s.origin.y <= 19.71f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, tb_move_ground_speed(&s));
}

static void test_angle_conversion(void)
{
    TEST_ASSERT_EQUAL_INT16(0, tb_angle_to_short(0.0f));
    TEST_ASSERT_EQUAL_INT16(16384, tb_angle_to_short(TB_PI / 2));
    TEST_ASSERT_EQUAL_INT16(-16384, tb_angle_to_short(-TB_PI / 2));
    TEST_ASSERT_EQUAL_INT16(-32768, tb_angle_to_short(TB_PI));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, TB_PI / 2, tb_short_to_angle(16384));
    TEST_ASSERT_EQUAL_INT16(-32768, tb_angle_to_short(TB_PI - 0.00001f)); /* rounds to the wrap point */
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_walk_reaches_walk_speed_and_stops);
    RUN_TEST(test_sprint_and_crouch_speeds);
    RUN_TEST(test_jump_and_jump_hold);
    RUN_TEST(test_steps_up_stairs_but_not_ledges);
    RUN_TEST(test_slides_along_walls);
    RUN_TEST(test_walks_up_ramp);
    RUN_TEST(test_cannot_stand_under_low_ceiling);
    RUN_TEST(test_air_control_gains_speed_sideways);
    RUN_TEST(test_fall_speed_is_capped);
    RUN_TEST(test_stuck_in_solid_does_not_move);
    RUN_TEST(test_wedged_in_corner_stops);
    RUN_TEST(test_angle_conversion);
    return UNITY_END();
}
