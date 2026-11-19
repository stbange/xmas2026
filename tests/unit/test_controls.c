/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "app/controls.h"
#include "core/mathx.h"

void setUp(void) {}
void tearDown(void) {}

static void test_movement_keys(void)
{
    tb_controls c;
    tb_controls_init(&c, 0.0f);
    tb_controls_key(&c, TB_KEY_FORWARD, true);
    tb_controls_key(&c, TB_KEY_RIGHT, true);
    tb_input in = tb_controls_input(&c);
    TEST_ASSERT_EQUAL_INT8(127, in.forward);
    TEST_ASSERT_EQUAL_INT8(127, in.side);
    tb_controls_key(&c, TB_KEY_BACK, true);
    tb_controls_key(&c, TB_KEY_LEFT, true);
    in = tb_controls_input(&c);
    TEST_ASSERT_EQUAL_INT8(0, in.forward); /* opposite keys cancel */
    TEST_ASSERT_EQUAL_INT8(0, in.side);
    tb_controls_release_all(&c);
    in = tb_controls_input(&c);
    TEST_ASSERT_EQUAL_INT8(0, in.forward);
}

static void test_buttons(void)
{
    tb_controls c;
    tb_controls_init(&c, 0.0f);
    const tb_key keys[] = {TB_KEY_JUMP,     TB_KEY_CROUCH, TB_KEY_SPRINT, TB_KEY_FIRE,
                           TB_KEY_ALT_FIRE, TB_KEY_USE,    TB_KEY_MELEE};
    const uint16_t buttons[] = {TB_BTN_JUMP,     TB_BTN_CROUCH, TB_BTN_SPRINT, TB_BTN_FIRE,
                                TB_BTN_ALT_FIRE, TB_BTN_USE,    TB_BTN_MELEE};
    for (int i = 0; i < 7; i++) {
        tb_controls_release_all(&c);
        tb_controls_key(&c, keys[i], true);
        TEST_ASSERT_EQUAL_HEX16(buttons[i], tb_controls_input(&c).buttons);
    }
    tb_controls_key(&c, (tb_key)-1, true); /* ignored */
    tb_controls_key(&c, TB_KEY_COUNT, true);
}

static void test_weapon_keys_send_once(void)
{
    tb_controls c;
    tb_controls_init(&c, 0.0f);
    tb_controls_key(&c, TB_KEY_WEAPON1 + 2, true);
    TEST_ASSERT_EQUAL_UINT8(3, tb_controls_input(&c).weapon);
    TEST_ASSERT_EQUAL_UINT8(0, tb_controls_input(&c).weapon);
    tb_controls_key(&c, TB_KEY_WEAPON1 + 2, true); /* key repeat: no new selection */
    TEST_ASSERT_EQUAL_UINT8(0, tb_controls_input(&c).weapon);
    tb_controls_key(&c, TB_KEY_WEAPON1 + 2, false);
    tb_controls_key(&c, TB_KEY_WEAPON9, true);
    TEST_ASSERT_EQUAL_UINT8(9, tb_controls_input(&c).weapon);
}

static void test_mouse_look(void)
{
    tb_controls c;
    tb_controls_init(&c, 0.0f);
    tb_controls_mouse(&c, 100.0f, 0.0f, 1.0f); /* right: yaw decreases (clockwise from above) */
    TEST_ASSERT_TRUE(c.yaw < 0.0f);
    tb_controls_mouse(&c, 0.0f, -100000.0f, 1.0f); /* far up: pitch clamps */
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, 1.55f, c.pitch);
    tb_controls_mouse(&c, 0.0f, 100000.0f, 1.0f);
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, -1.55f, c.pitch);
    tb_controls_mouse(&c, -100000.0f, 0.0f, 1.0f); /* yaw wraps */
    TEST_ASSERT_TRUE(c.yaw >= -TB_PI && c.yaw < TB_PI);
    tb_input in = tb_controls_input(&c);
    TEST_ASSERT_EQUAL_INT16(tb_angle_to_short(c.yaw), in.yaw);
    TEST_ASSERT_EQUAL_INT16(tb_angle_to_short(c.pitch), in.pitch);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_movement_keys);
    RUN_TEST(test_buttons);
    RUN_TEST(test_weapon_keys_send_once);
    RUN_TEST(test_mouse_look);
    return UNITY_END();
}
