/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "platform/fs.h"
#include "platform/storage.h"

#include <SDL3/SDL.h>
#include <string.h>

#define DIR TB_TEST_TMP_DIR "/storage"

static tb_storage g_st;

static void wipe(void)
{
    tb_fs_remove(DIR "/" TB_SAVE_FILE);
    tb_fs_remove(DIR "/" TB_SAVE_BACKUP);
    tb_fs_remove(DIR "/" TB_SETTINGS_FILE);
}

void setUp(void)
{
    TEST_ASSERT_TRUE(tb_storage_init(&g_st, DIR));
    wipe();
}

void tearDown(void)
{
    wipe();
}

static void test_dir_gets_trailing_separator(void)
{
    TEST_ASSERT_EQUAL_STRING(DIR "/", g_st.dir);
    tb_storage other;
    TEST_ASSERT_TRUE(tb_storage_init(&other, DIR "/"));
    TEST_ASSERT_EQUAL_STRING(DIR "/", other.dir);
}

static void test_default_dir_is_pref_path(void)
{
    tb_storage st;
    TEST_ASSERT_TRUE(tb_storage_init(&st, ""));
    TEST_ASSERT_NOT_NULL(strstr(st.dir, "xmas2026"));
    TEST_ASSERT_TRUE(tb_storage_init(&st, NULL));
}

static void test_uncreatable_dir_fails(void)
{
    /* A path below a regular file cannot be created. */
    TEST_ASSERT_TRUE(tb_fs_write_atomic(DIR "/" TB_SETTINGS_FILE, "{}", 2));
    tb_storage st;
    TEST_ASSERT_FALSE(tb_storage_init(&st, DIR "/" TB_SETTINGS_FILE "/sub"));
}

static void test_save_roundtrip_through_files(void)
{
    tb_save s, out;
    tb_save_defaults(&s);
    s.unlocked = 0x7F;
    s.completed = 0x3;
    TEST_ASSERT_TRUE(tb_save_store(&g_st.io, &s));
    TEST_ASSERT_TRUE(tb_fs_exists(DIR "/" TB_SAVE_FILE));
    TEST_ASSERT_EQUAL_INT(TB_SAVE_OK, tb_save_load(&g_st.io, &out));
    TEST_ASSERT_EQUAL_HEX32(0x7F, out.unlocked);
}

static void test_settings_roundtrip_and_fallbacks(void)
{
    tb_settings s, out;
    tb_storage_load_settings(&g_st, &out); /* missing file */
    TEST_ASSERT_EQUAL_FLOAT(90.0f, out.fov);
    tb_settings_defaults(&s);
    s.fov = 75.0f;
    TEST_ASSERT_TRUE(tb_storage_save_settings(&g_st, &s));
    tb_storage_load_settings(&g_st, &out);
    TEST_ASSERT_EQUAL_FLOAT(75.0f, out.fov);
    TEST_ASSERT_TRUE(tb_fs_write_atomic(DIR "/" TB_SETTINGS_FILE, "{broken", 7));
    tb_storage_load_settings(&g_st, &out);
    TEST_ASSERT_EQUAL_FLOAT(90.0f, out.fov);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dir_gets_trailing_separator);
    RUN_TEST(test_default_dir_is_pref_path);
    RUN_TEST(test_uncreatable_dir_fails);
    RUN_TEST(test_save_roundtrip_through_files);
    RUN_TEST(test_settings_roundtrip_and_fallbacks);
    return UNITY_END();
}
