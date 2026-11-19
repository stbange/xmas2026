/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "platform/fs.h"

#include <SDL3/SDL.h>
#include <string.h>

static char *g_dir;
static char g_path[1024];

void setUp(void)
{
    g_dir = tb_fs_pref_dir("AdventOfLAN-tests", "fs");
    TEST_ASSERT_NOT_NULL(g_dir);
    SDL_snprintf(g_path, sizeof(g_path), "%ssave-test.bin", g_dir);
    tb_fs_remove(g_path);
}

void tearDown(void)
{
    tb_fs_remove(g_path);
    SDL_free(g_dir);
}

static void test_pref_dir_ends_with_separator(void)
{
    size_t n = strlen(g_dir);
    TEST_ASSERT_TRUE(n > 0);
    TEST_ASSERT_TRUE(g_dir[n - 1] == '/' || g_dir[n - 1] == '\\');
}

static void test_write_read_roundtrip(void)
{
    const char msg[] = "portal 7 open";
    TEST_ASSERT_FALSE(tb_fs_exists(g_path));
    TEST_ASSERT_TRUE(tb_fs_write_atomic(g_path, msg, sizeof(msg) - 1));
    TEST_ASSERT_TRUE(tb_fs_exists(g_path));
    void *data = NULL;
    size_t size = 0;
    TEST_ASSERT_TRUE(tb_fs_read(g_path, &data, &size));
    TEST_ASSERT_EQUAL_UINT(sizeof(msg) - 1, size);
    TEST_ASSERT_EQUAL_STRING(msg, (const char *)data); /* NUL-terminated */
    SDL_free(data);
}

static void test_overwrite_replaces_content_and_leaves_no_temp(void)
{
    TEST_ASSERT_TRUE(tb_fs_write_atomic(g_path, "first", 5));
    TEST_ASSERT_TRUE(tb_fs_write_atomic(g_path, "2nd", 3));
    void *data = NULL;
    size_t size = 0;
    TEST_ASSERT_TRUE(tb_fs_read(g_path, &data, &size));
    TEST_ASSERT_EQUAL_STRING("2nd", (const char *)data);
    SDL_free(data);
    char tmp[1100];
    SDL_snprintf(tmp, sizeof(tmp), "%s.tmp", g_path);
    TEST_ASSERT_FALSE(tb_fs_exists(tmp));
}

static void test_missing_and_invalid_paths(void)
{
    void *data = (void *)1;
    size_t size = 0;
    TEST_ASSERT_FALSE(tb_fs_read(g_path, &data, &size));
    TEST_ASSERT_NULL(data);
    TEST_ASSERT_TRUE(tb_fs_remove(g_path)); /* already gone counts as removed */
    char bad[1100];
    SDL_snprintf(bad, sizeof(bad), "%sno/such/dir/file.bin", g_dir);
    TEST_ASSERT_FALSE(tb_fs_write_atomic(bad, "x", 1));
}

static void test_remove_failure_is_reported(void)
{
    char sub[1100], inner[1200];
    SDL_snprintf(sub, sizeof(sub), "%sremove-test", g_dir);
    SDL_snprintf(inner, sizeof(inner), "%s/keep.bin", sub);
    TEST_ASSERT_TRUE(SDL_CreateDirectory(sub));
    TEST_ASSERT_TRUE(tb_fs_write_atomic(inner, "x", 1));
    TEST_ASSERT_FALSE(tb_fs_remove(sub)); /* non-empty directory */
    TEST_ASSERT_TRUE(tb_fs_remove(inner));
    TEST_ASSERT_TRUE(tb_fs_remove(sub));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pref_dir_ends_with_separator);
    RUN_TEST(test_write_read_roundtrip);
    RUN_TEST(test_overwrite_replaces_content_and_leaves_no_temp);
    RUN_TEST(test_missing_and_invalid_paths);
    RUN_TEST(test_remove_failure_is_reported);
    return UNITY_END();
}
