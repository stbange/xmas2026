/* SPDX-License-Identifier: MIT */
/* Host tests. Windowed tests need a display with OpenGL 3.3 (CI runs them
 * under Xvfb with Mesa); without one they are ignored, not failed. */
#include "unity.h"

#include "platform/host.h"

#include "platform/fs.h"

#include <SDL3/SDL.h>

#define DATA TB_TEST_TMP_DIR "/host"
#define DAY  86400

static void wipe(void)
{
    tb_fs_remove(DATA "/" TB_SAVE_FILE);
    tb_fs_remove(DATA "/" TB_SAVE_BACKUP);
    tb_fs_remove(DATA "/" TB_SETTINGS_FILE);
}

void setUp(void)
{
    wipe();
}

void tearDown(void)
{
    SDL_ResetHint(SDL_HINT_VIDEO_DRIVER);
    wipe();
}

static tb_host g_host;

static void test_headless_run_finishes_after_ticks(void)
{
    const char *argv[] = {"game",       "--headless", "--ticks",    "10", "--date",
                          "2026-12-24", "--playtest", "--data-dir", DATA};
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, tb_host_init(&g_host, 9, argv));
    TEST_ASSERT_TRUE(g_host.progress_loaded);
    TEST_ASSERT_FALSE(g_host.has_window);
    int frames = 0;
    tb_host_result r;
    while ((r = tb_host_iterate(&g_host)) == TB_HOST_CONTINUE) {
        frames++;
    }
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, r);
    TEST_ASSERT_EQUAL_INT(9, frames);
    TEST_ASSERT_EQUAL_UINT64(10, tb_app_tick(&g_host.app));
    TEST_ASSERT_TRUE(tb_host_now(&g_host) >= 0.0);
    tb_host_quit(&g_host);
    TEST_ASSERT_FALSE(g_host.sdl_initialised);
}

static void test_help_exits_successfully(void)
{
    const char *argv[] = {"game", "--help"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, tb_host_init(&g_host, 2, argv));
    tb_host_quit(&g_host);
}

static void test_bad_arguments_fail(void)
{
    const char *argv[] = {"game", "--warp-speed"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_FAILURE, tb_host_init(&g_host, 2, argv));
    tb_host_quit(&g_host);
}

static tb_host_result send(tb_host *h, SDL_EventType type, int scancode, int button, float dx)
{
    SDL_Event e;
    SDL_zero(e);
    e.type = type;
    e.key.scancode = (SDL_Scancode)scancode;
    if (type == SDL_EVENT_MOUSE_BUTTON_DOWN || type == SDL_EVENT_MOUSE_BUTTON_UP) {
        e.button.button = (Uint8)button;
    }
    if (type == SDL_EVENT_MOUSE_MOTION) {
        e.motion.xrel = dx;
    }
    return tb_host_event(h, &e);
}

static void test_events(void)
{
    static tb_host h;
    SDL_zero(h);
    tb_controls_init(&h.controls, 0.0f);
    h.settings.mouse_sensitivity = 1.0f;
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, send(&h, SDL_EVENT_QUIT, 0, 0, 0));
    /* Escape never quits (important in the browser); it frees the mouse. */
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, send(&h, SDL_EVENT_KEY_DOWN, SDL_SCANCODE_ESCAPE, 0, 0));
    send(&h, SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W, 0, 0);
    TEST_ASSERT_TRUE(h.controls.down[TB_KEY_FORWARD]);
    send(&h, SDL_EVENT_KEY_UP, SDL_SCANCODE_W, 0, 0);
    TEST_ASSERT_FALSE(h.controls.down[TB_KEY_FORWARD]);
    send(&h, SDL_EVENT_KEY_DOWN, SDL_SCANCODE_F12, 0, 0); /* unbound */
    /* Mouse look only while captured; the first click captures. */
    send(&h, SDL_EVENT_MOUSE_MOTION, 0, 0, 50.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, h.controls.yaw);
    send(&h, SDL_EVENT_MOUSE_BUTTON_DOWN, 0, SDL_BUTTON_LEFT, 0);
    TEST_ASSERT_TRUE(h.mouse_captured);
    TEST_ASSERT_FALSE(h.controls.down[TB_KEY_FIRE]);
    send(&h, SDL_EVENT_MOUSE_BUTTON_DOWN, 0, SDL_BUTTON_LEFT, 0);
    TEST_ASSERT_TRUE(h.controls.down[TB_KEY_FIRE]);
    send(&h, SDL_EVENT_MOUSE_BUTTON_UP, 0, SDL_BUTTON_LEFT, 0);
    send(&h, SDL_EVENT_MOUSE_BUTTON_DOWN, 0, SDL_BUTTON_RIGHT, 0);
    TEST_ASSERT_TRUE(h.controls.down[TB_KEY_ALT_FIRE]);
    send(&h, SDL_EVENT_MOUSE_BUTTON_DOWN, 0, SDL_BUTTON_MIDDLE, 0);
    send(&h, SDL_EVENT_MOUSE_MOTION, 0, 0, 50.0f);
    TEST_ASSERT_TRUE(h.controls.yaw < 0.0f);
    send(&h, SDL_EVENT_WINDOW_FOCUS_LOST, 0, 0, 0);
    TEST_ASSERT_FALSE(h.mouse_captured);
    TEST_ASSERT_FALSE(h.controls.down[TB_KEY_ALT_FIRE]);
    send(&h, SDL_EVENT_KEY_DOWN, SDL_SCANCODE_ESCAPE, 0, 0);
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, send(&h, SDL_EVENT_WINDOW_SHOWN, 0, 0, 0));
}

static void test_default_bindings(void)
{
    const int codes[] = {SDL_SCANCODE_W,     SDL_SCANCODE_UP,    SDL_SCANCODE_S, SDL_SCANCODE_DOWN,
                         SDL_SCANCODE_A,     SDL_SCANCODE_LEFT,  SDL_SCANCODE_D, SDL_SCANCODE_RIGHT,
                         SDL_SCANCODE_SPACE, SDL_SCANCODE_LCTRL, SDL_SCANCODE_C, SDL_SCANCODE_LSHIFT,
                         SDL_SCANCODE_E,     SDL_SCANCODE_V,     SDL_SCANCODE_F, SDL_SCANCODE_1,
                         SDL_SCANCODE_9};
    const int keys[] = {TB_KEY_FORWARD, TB_KEY_FORWARD, TB_KEY_BACK,  TB_KEY_BACK,  TB_KEY_LEFT,
                        TB_KEY_LEFT,    TB_KEY_RIGHT,   TB_KEY_RIGHT, TB_KEY_JUMP,  TB_KEY_CROUCH,
                        TB_KEY_CROUCH,  TB_KEY_SPRINT,  TB_KEY_USE,   TB_KEY_MELEE, TB_KEY_MELEE,
                        TB_KEY_WEAPON1, TB_KEY_WEAPON9};
    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
        TEST_ASSERT_EQUAL_INT(keys[i], tb_host_key_for_scancode(codes[i]));
    }
    TEST_ASSERT_EQUAL_INT(-1, tb_host_key_for_scancode(SDL_SCANCODE_F12));
}

static void test_missing_video_driver_fails(void)
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "no-such-driver");
    const char *argv[] = {"game"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_FAILURE, tb_host_init(&g_host, 1, argv));
    tb_host_quit(&g_host);
}

static void test_driver_without_opengl_fails(void)
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    const char *argv[] = {"game"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_FAILURE, tb_host_init(&g_host, 1, argv));
    TEST_ASSERT_FALSE(g_host.has_window);
    tb_host_quit(&g_host);
}

static void test_windowed_run_renders_frames(void)
{
    const char *argv[] = {"game", "--ticks", "3", "--data-dir", DATA};
    if (tb_host_init(&g_host, 5, argv) != TB_HOST_CONTINUE) {
        tb_host_quit(&g_host);
        TEST_IGNORE_MESSAGE("no display with OpenGL 3.3 available");
    }
    TEST_ASSERT_TRUE(g_host.has_window);
    TEST_ASSERT_TRUE(g_host.has_renderer);
    tb_controls_key(&g_host.controls, TB_KEY_FORWARD, true);
    tb_host_result r = TB_HOST_CONTINUE;
    for (int i = 0; i < 1000 && r == TB_HOST_CONTINUE; i++) {
        r = tb_host_iterate(&g_host);
    }
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, r);
    tb_host_quit(&g_host);
    TEST_ASSERT_FALSE(g_host.has_window);
}

static tb_host_result init_headless(tb_host *h)
{
    const char *argv[] = {"game", "--headless", "--data-dir", DATA};
    return tb_host_init(h, 4, argv);
}

static void test_new_unlocks_are_saved_and_reloaded(void)
{
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, init_headless(&g_host));
    /* 2026-12-07 10:00 UTC */
    g_host.clock_override_utc = tb_days_from_civil(2026, 12, 7) * DAY + 10 * 3600;
    tb_host_update_calendar(&g_host);
    TEST_ASSERT_TRUE(tb_portal_is_open(g_host.calendar.open, 7));
    TEST_ASSERT_TRUE(tb_fs_exists(DATA "/" TB_SAVE_FILE));
    tb_host_quit(&g_host);

    /* Next start, clock set back to November: door 7 stays open. */
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, init_headless(&g_host));
    TEST_ASSERT_EQUAL_HEX32(0x7Fu, g_host.save.unlocked);
    g_host.clock_override_utc = tb_days_from_civil(2026, 11, 20) * DAY;
    tb_host_update_calendar(&g_host);
    TEST_ASSERT_TRUE(tb_portal_is_open(g_host.calendar.open, 7));
    tb_host_quit(&g_host);
}

static void test_corrupt_save_is_backed_up_at_startup(void)
{
    TEST_ASSERT_TRUE(tb_fs_write_atomic(DATA "/" TB_SAVE_FILE, "garbage!", 8));
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, init_headless(&g_host));
    TEST_ASSERT_TRUE(tb_fs_exists(DATA "/" TB_SAVE_BACKUP));
    TEST_ASSERT_EQUAL_UINT32(0, g_host.save.completed);
    tb_host_quit(&g_host);
}

static void test_unwritable_save_does_not_crash(void)
{
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, init_headless(&g_host));
    TEST_ASSERT_TRUE(SDL_CreateDirectory(DATA "/" TB_SAVE_FILE)); /* blocks the file */
    g_host.clock_override_utc = tb_days_from_civil(2026, 12, 3) * DAY;
    tb_host_update_calendar(&g_host);
    TEST_ASSERT_EQUAL_HEX32(0x7u, g_host.save.unlocked); /* kept in memory */
    TEST_ASSERT_TRUE(SDL_RemovePath(DATA "/" TB_SAVE_FILE));
    tb_host_quit(&g_host);
}

static void test_unusable_data_dir_fails(void)
{
    TEST_ASSERT_TRUE(tb_fs_write_atomic(DATA "/" TB_SETTINGS_FILE, "{}", 2));
    const char *argv[] = {"game", "--headless", "--data-dir", DATA "/" TB_SETTINGS_FILE "/x"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_FAILURE, tb_host_init(&g_host, 4, argv));
    tb_host_quit(&g_host);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_headless_run_finishes_after_ticks);
    RUN_TEST(test_help_exits_successfully);
    RUN_TEST(test_bad_arguments_fail);
    RUN_TEST(test_events);
    RUN_TEST(test_default_bindings);
    RUN_TEST(test_missing_video_driver_fails);
    RUN_TEST(test_driver_without_opengl_fails);
    RUN_TEST(test_windowed_run_renders_frames);
    RUN_TEST(test_new_unlocks_are_saved_and_reloaded);
    RUN_TEST(test_corrupt_save_is_backed_up_at_startup);
    RUN_TEST(test_unwritable_save_does_not_crash);
    RUN_TEST(test_unusable_data_dir_fails);
    return UNITY_END();
}
