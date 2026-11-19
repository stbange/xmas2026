/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/version.h"

#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_version_is_semver(void)
{
    int major = -1, minor = -1, patch = -1;
    TEST_ASSERT_EQUAL_INT(3, sscanf(tb_version(), "%d.%d.%d", &major, &minor, &patch));
    TEST_ASSERT_TRUE(major >= 0 && minor >= 0 && patch >= 0);
}

static void test_build_id_is_short(void)
{
    size_t n = strlen(tb_build_id());
    TEST_ASSERT_TRUE(n >= 3 && n <= 8);
}

static void test_banner_contains_version_and_build(void)
{
    const char *b = tb_version_banner();
    TEST_ASSERT_NOT_NULL(strstr(b, "Advent of LAN"));
    TEST_ASSERT_NOT_NULL(strstr(b, tb_version()));
    TEST_ASSERT_NOT_NULL(strstr(b, tb_build_id()));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_version_is_semver);
    RUN_TEST(test_build_id_is_short);
    RUN_TEST(test_banner_contains_version_and_build);
    return UNITY_END();
}
