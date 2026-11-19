/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "game/settings.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static bool parse(tb_settings *s, const char *json)
{
    return tb_settings_from_json(s, json, strlen(json));
}

static void test_defaults(void)
{
    tb_settings s;
    tb_settings_defaults(&s);
    TEST_ASSERT_EQUAL_FLOAT(90.0f, s.fov);
    TEST_ASSERT_EQUAL_INT(1, s.resolution_preset);
    TEST_ASSERT_EQUAL_INT(TB_TOUCH_AUTO, s.touch_mode);
    TEST_ASSERT_EQUAL_INT(TB_GORE_CONFETTI, s.gore);
}

static void test_roundtrip(void)
{
    tb_settings s, out;
    tb_settings_defaults(&s);
    s.fov = 104.0f;
    s.mouse_sensitivity = 2.5f;
    s.master_volume = 0.25f;
    s.music_volume = 0.0f;
    s.resolution_preset = 2;
    s.palette_quantize = false;
    s.dithering = false;
    s.crt_filter = true;
    s.head_bob = false;
    s.camera_shake = false;
    s.reduce_flashes = true;
    s.subtitles = false;
    s.touch_mode = TB_TOUCH_OFF;
    s.gore = TB_GORE_CLASSIC;
    char *json = tb_settings_to_json(&s);
    TEST_ASSERT_NOT_NULL(json);
    TEST_ASSERT_TRUE(parse(&out, json));
    free(json);
    TEST_ASSERT_EQUAL_MEMORY(&s, &out, sizeof(s));
}

static void test_clamping_and_unknown_keys(void)
{
    tb_settings s;
    TEST_ASSERT_TRUE(parse(&s, "{\"fov\": 500, \"mouseSensitivity\": -3, \"resolutionPreset\": 9,"
                               " \"masterVolume\": 2, \"futureOption\": true}"));
    TEST_ASSERT_EQUAL_FLOAT(110.0f, s.fov);
    TEST_ASSERT_EQUAL_FLOAT(0.1f, s.mouse_sensitivity);
    TEST_ASSERT_EQUAL_INT(2, s.resolution_preset);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, s.master_volume);
    TEST_ASSERT_TRUE(parse(&s, "{\"resolutionPreset\": -4}"));
    TEST_ASSERT_EQUAL_INT(0, s.resolution_preset);
}

static void test_wrong_types_keep_defaults(void)
{
    tb_settings s;
    TEST_ASSERT_TRUE(parse(&s, "{\"fov\": \"wide\", \"crtFilter\": 1, \"resolutionPreset\": \"2\","
                               " \"touchMode\": \"sometimes\", \"gore\": 3}"));
    TEST_ASSERT_EQUAL_FLOAT(90.0f, s.fov);
    TEST_ASSERT_FALSE(s.crt_filter);
    TEST_ASSERT_EQUAL_INT(1, s.resolution_preset);
    TEST_ASSERT_EQUAL_INT(TB_TOUCH_AUTO, s.touch_mode);
    TEST_ASSERT_EQUAL_INT(TB_GORE_CONFETTI, s.gore);
    TEST_ASSERT_TRUE(parse(&s, "{\"touchMode\": \"on\", \"gore\": \"off\"}"));
    TEST_ASSERT_EQUAL_INT(TB_TOUCH_ON, s.touch_mode);
    TEST_ASSERT_EQUAL_INT(TB_GORE_OFF, s.gore);
}

static void test_invalid_json(void)
{
    tb_settings s;
    TEST_ASSERT_FALSE(parse(&s, "{ not json"));
    TEST_ASSERT_EQUAL_FLOAT(90.0f, s.fov);
    TEST_ASSERT_FALSE(parse(&s, "[1, 2, 3]"));
    TEST_ASSERT_FALSE(parse(&s, ""));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_defaults);
    RUN_TEST(test_roundtrip);
    RUN_TEST(test_clamping_and_unknown_keys);
    RUN_TEST(test_wrong_types_keep_defaults);
    RUN_TEST(test_invalid_json);
    return UNITY_END();
}
