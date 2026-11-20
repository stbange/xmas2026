/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "app/args.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static char g_err[128];

static bool parse(tb_args *a, int argc, const char *const *argv)
{
    g_err[0] = '\0';
    return tb_args_parse(a, argc, argv, g_err, sizeof(g_err));
}

static void test_defaults(void)
{
    const char *argv[] = {"game"};
    tb_args a;
    TEST_ASSERT_TRUE(parse(&a, 1, argv));
    TEST_ASSERT_FALSE(a.headless);
    TEST_ASSERT_FALSE(a.playtest);
    TEST_ASSERT_FALSE(a.has_date);
    TEST_ASSERT_EQUAL_UINT64(0, a.max_ticks);
    TEST_ASSERT_EQUAL_UINT64(2026, a.seed);
}

static void test_all_options(void)
{
    const char *argv[] = {"game",   "--headless", "--playtest", "--ticks",    "600",
                          "--seed", "42",         "--date",     "2026-12-07", "--help"};
    tb_args a;
    TEST_ASSERT_TRUE(parse(&a, 10, argv));
    TEST_ASSERT_TRUE(a.headless);
    TEST_ASSERT_TRUE(a.playtest);
    TEST_ASSERT_TRUE(a.help);
    TEST_ASSERT_EQUAL_UINT64(600, a.max_ticks);
    TEST_ASSERT_EQUAL_UINT64(42, a.seed);
    TEST_ASSERT_TRUE(a.has_date);
    TEST_ASSERT_EQUAL_INT(2026, a.date.year);
    TEST_ASSERT_EQUAL_INT(12, a.date.month);
    TEST_ASSERT_EQUAL_INT(7, a.date.day);
    const char *argv2[] = {"game", "-h"};
    TEST_ASSERT_TRUE(parse(&a, 2, argv2));
    TEST_ASSERT_TRUE(a.help);
}

static void test_errors(void)
{
    tb_args a;
    const char *unknown[] = {"game", "--fly"};
    TEST_ASSERT_FALSE(parse(&a, 2, unknown));
    TEST_ASSERT_NOT_NULL(strstr(g_err, "--fly"));
    const char *ticks_missing[] = {"game", "--ticks"};
    TEST_ASSERT_FALSE(parse(&a, 2, ticks_missing));
    const char *ticks_bad[] = {"game", "--ticks", "12a"};
    TEST_ASSERT_FALSE(parse(&a, 3, ticks_bad));
    const char *ticks_empty[] = {"game", "--ticks", ""};
    TEST_ASSERT_FALSE(parse(&a, 3, ticks_empty));
    const char *ticks_overflow[] = {"game", "--ticks", "99999999999999999999"};
    TEST_ASSERT_FALSE(parse(&a, 3, ticks_overflow));
    const char *seed_bad[] = {"game", "--seed", "x"};
    TEST_ASSERT_FALSE(parse(&a, 3, seed_bad));
    const char *date_bad[] = {"game", "--date", "2026-13-01"};
    TEST_ASSERT_FALSE(parse(&a, 3, date_bad));
    const char *level_bad[] = {"game", "--level", "25"};
    TEST_ASSERT_FALSE(parse(&a, 3, level_bad));
    TEST_ASSERT_NOT_NULL(strstr(g_err, "--level"));
    const char *level_zero[] = {"game", "--level", "0"};
    TEST_ASSERT_FALSE(parse(&a, 3, level_zero));
    const char *level_missing[] = {"game", "--level"};
    TEST_ASSERT_FALSE(parse(&a, 2, level_missing));
}

static void test_level_and_bot(void)
{
    const char *argv[] = {"game", "--level", "1", "--bot"};
    tb_args a;
    TEST_ASSERT_TRUE(parse(&a, 4, argv));
    TEST_ASSERT_EQUAL_INT(1, a.level);
    TEST_ASSERT_TRUE(a.bot);
    TEST_ASSERT_NOT_NULL(strstr(tb_args_usage(), "--level"));
    TEST_ASSERT_NOT_NULL(strstr(tb_args_usage(), "--bot"));
}

static void test_data_dir(void)
{
    tb_args a;
    const char *ok[] = {"game", "--data-dir", "/tmp/aol"};
    TEST_ASSERT_TRUE(parse(&a, 3, ok));
    TEST_ASSERT_EQUAL_STRING("/tmp/aol", a.data_dir);
    const char *missing[] = {"game", "--data-dir"};
    TEST_ASSERT_FALSE(parse(&a, 2, missing));
    const char *empty[] = {"game", "--data-dir", ""};
    TEST_ASSERT_FALSE(parse(&a, 3, empty));
    static char longpath[600];
    memset(longpath, 'a', sizeof(longpath) - 1);
    const char *too_long[] = {"game", "--data-dir", longpath};
    TEST_ASSERT_FALSE(parse(&a, 3, too_long));
}

static void test_parse_date_validation(void)
{
    tb_date d;
    TEST_ASSERT_TRUE(tb_parse_date("2024-02-29", &d)); /* leap year */
    TEST_ASSERT_FALSE(tb_parse_date("2026-02-29", &d));
    TEST_ASSERT_TRUE(tb_parse_date("2000-02-29", &d));  /* divisible by 400 */
    TEST_ASSERT_FALSE(tb_parse_date("2100-02-29", &d)); /* divisible by 100 */
    TEST_ASSERT_FALSE(tb_parse_date("2026-04-31", &d));
    TEST_ASSERT_TRUE(tb_parse_date("2026-12-31", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026-12-00", &d));
    TEST_ASSERT_FALSE(tb_parse_date("1969-12-31", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026-00-10", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026/12/01", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026-12-0x", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026-1-01", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026-12-011", &d));
    TEST_ASSERT_FALSE(tb_parse_date("2026x12-01", &d));
    TEST_ASSERT_FALSE(tb_parse_date(NULL, &d));
    tb_date far = {10000, 1, 1};
    TEST_ASSERT_FALSE(tb_date_is_valid(far));
}

static void test_usage_mentions_options(void)
{
    TEST_ASSERT_NOT_NULL(strstr(tb_args_usage(), "--headless"));
    TEST_ASSERT_NOT_NULL(strstr(tb_args_usage(), "--date"));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_defaults);
    RUN_TEST(test_all_options);
    RUN_TEST(test_errors);
    RUN_TEST(test_level_and_bot);
    RUN_TEST(test_data_dir);
    RUN_TEST(test_parse_date_validation);
    RUN_TEST(test_usage_mentions_options);
    return UNITY_END();
}
