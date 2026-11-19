/* SPDX-License-Identifier: MIT */
/* Host tests. Windowed tests need a display with OpenGL 3.3 (CI runs them
 * under Xvfb with Mesa); without one they are ignored, not failed. */
#include "unity.h"

#include "platform/host.h"

#include <SDL3/SDL.h>

void setUp(void) {}
void tearDown(void)
{
    SDL_ResetHint(SDL_HINT_VIDEO_DRIVER);
}

static tb_host g_host;

static void test_headless_run_finishes_after_ticks(void)
{
    const char *argv[] = {"game", "--headless", "--ticks", "10", "--date", "2026-12-24"};
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, tb_host_init(&g_host, 6, argv));
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

static void test_events(void)
{
    SDL_Event e;
    SDL_zero(e);
    e.type = SDL_EVENT_QUIT;
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, tb_host_event(&g_host, &e));
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.key = SDLK_ESCAPE;
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, tb_host_event(&g_host, &e));
    e.key.key = SDLK_SPACE;
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, tb_host_event(&g_host, &e));
    e.type = SDL_EVENT_MOUSE_MOTION;
    TEST_ASSERT_EQUAL_INT(TB_HOST_CONTINUE, tb_host_event(&g_host, &e));
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
    const char *argv[] = {"game", "--ticks", "3"};
    if (tb_host_init(&g_host, 3, argv) != TB_HOST_CONTINUE) {
        tb_host_quit(&g_host);
        TEST_IGNORE_MESSAGE("no display with OpenGL 3.3 available");
    }
    TEST_ASSERT_TRUE(g_host.has_window);
    tb_host_result r = TB_HOST_CONTINUE;
    for (int i = 0; i < 1000 && r == TB_HOST_CONTINUE; i++) {
        r = tb_host_iterate(&g_host);
    }
    TEST_ASSERT_EQUAL_INT(TB_HOST_SUCCESS, r);
    tb_host_quit(&g_host);
    TEST_ASSERT_FALSE(g_host.has_window);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_headless_run_finishes_after_ticks);
    RUN_TEST(test_help_exits_successfully);
    RUN_TEST(test_bad_arguments_fail);
    RUN_TEST(test_events);
    RUN_TEST(test_missing_video_driver_fails);
    RUN_TEST(test_driver_without_opengl_fails);
    RUN_TEST(test_windowed_run_renders_frames);
    return UNITY_END();
}
