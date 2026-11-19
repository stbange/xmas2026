/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/log.h"

#include <string.h>

static char g_last[256];
static int g_calls;
static tb_log_level g_last_level;

static void capture(void *user, tb_log_level level, const char *message)
{
    (void)user;
    g_calls++;
    g_last_level = level;
    strncpy(g_last, message, sizeof(g_last) - 1);
}

void setUp(void)
{
    g_calls = 0;
    g_last[0] = '\0';
    tb_log_set_sink(capture, NULL);
    tb_log_set_min_level(TB_LOG_DEBUG);
}

void tearDown(void)
{
    tb_log_set_sink(NULL, NULL);
    tb_log_set_min_level(TB_LOG_INFO);
}

static void test_formats_message(void)
{
    tb_log_info("door %d opens in %s", 7, "3 d");
    TEST_ASSERT_EQUAL_INT(1, g_calls);
    TEST_ASSERT_EQUAL_STRING("door 7 opens in 3 d", g_last);
    TEST_ASSERT_EQUAL_INT(TB_LOG_INFO, g_last_level);
}

static void test_level_filtering(void)
{
    tb_log_set_min_level(TB_LOG_WARN);
    TEST_ASSERT_EQUAL_INT(TB_LOG_WARN, tb_log_min_level());
    tb_log_debug("hidden");
    tb_log_info("hidden");
    tb_log_warn("shown");
    tb_log_error("shown too");
    TEST_ASSERT_EQUAL_INT(2, g_calls);
    tb_log(TB_LOG_OFF, "never");
    TEST_ASSERT_EQUAL_INT(2, g_calls);
}

static void test_level_names(void)
{
    TEST_ASSERT_EQUAL_STRING("debug", tb_log_level_name(TB_LOG_DEBUG));
    TEST_ASSERT_EQUAL_STRING("info", tb_log_level_name(TB_LOG_INFO));
    TEST_ASSERT_EQUAL_STRING("warn", tb_log_level_name(TB_LOG_WARN));
    TEST_ASSERT_EQUAL_STRING("error", tb_log_level_name(TB_LOG_ERROR));
    TEST_ASSERT_EQUAL_STRING("?", tb_log_level_name(TB_LOG_OFF));
}

static void test_default_sink_writes_without_crashing(void)
{
    tb_log_set_sink(NULL, NULL);
    tb_log_error("default sink test (expected output)");
    TEST_ASSERT_EQUAL_INT(0, g_calls);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_formats_message);
    RUN_TEST(test_level_filtering);
    RUN_TEST(test_level_names);
    RUN_TEST(test_default_sink_writes_without_crashing);
    return UNITY_END();
}
