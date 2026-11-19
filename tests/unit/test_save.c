/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/hash.h"
#include "game/save.h"

#include <stdlib.h>
#include <string.h>

/* In-memory storage with two named slots. */
typedef struct mem_file {
    char name[32];
    uint8_t data[TB_SAVE_MAX_BYTES];
    size_t size;
    bool present;
} mem_file;

static mem_file g_files[2];
static bool g_write_fails;

static mem_file *slot(const char *name, bool create)
{
    for (int i = 0; i < 2; i++) {
        if (g_files[i].present && strcmp(g_files[i].name, name) == 0) {
            return &g_files[i];
        }
    }
    for (int i = 0; create && i < 2; i++) {
        if (!g_files[i].present) {
            strcpy(g_files[i].name, name);
            g_files[i].present = true;
            return &g_files[i];
        }
    }
    return NULL;
}

static bool mem_read(void *user, const char *name, void **data, size_t *size)
{
    (void)user;
    mem_file *f = slot(name, false);
    if (!f) {
        return false;
    }
    *data = malloc(f->size ? f->size : 1);
    memcpy(*data, f->data, f->size);
    *size = f->size;
    return true;
}

static void mem_free(void *user, void *data)
{
    (void)user;
    free(data);
}

static bool mem_write(void *user, const char *name, const void *data, size_t size)
{
    (void)user;
    if (g_write_fails || size > TB_SAVE_MAX_BYTES) {
        return false;
    }
    mem_file *f = slot(name, true);
    memcpy(f->data, data, size);
    f->size = size;
    return true;
}

static const tb_save_io g_io = {mem_read, mem_free, mem_write, NULL};

void setUp(void)
{
    memset(g_files, 0, sizeof(g_files));
    g_write_fails = false;
}
void tearDown(void) {}

static tb_save sample(void)
{
    tb_save s;
    tb_save_defaults(&s);
    s.unlocked = 0x3Fu;
    s.completed = 0x05u;
    s.floppies[0] = 7;
    s.floppies[2] = 2;
    s.challenges = 1;
    s.trophies = 5;
    s.alignment = -12;
    s.best_time_ms[0] = 1234567;
    return s;
}

static void test_roundtrip(void)
{
    tb_save s = sample(), out;
    TEST_ASSERT_TRUE(tb_save_store(&g_io, &s));
    TEST_ASSERT_EQUAL_INT(TB_SAVE_OK, tb_save_load(&g_io, &out));
    TEST_ASSERT_EQUAL_MEMORY(&s, &out, sizeof(s));
    TEST_ASSERT_NULL(slot(TB_SAVE_BACKUP, false));
}

static void test_missing_file_gives_defaults(void)
{
    tb_save out;
    out.completed = 99;
    TEST_ASSERT_EQUAL_INT(TB_SAVE_MISSING, tb_save_load(&g_io, &out));
    TEST_ASSERT_EQUAL_UINT32(0, out.completed);
}

/* @scenario quality-gates: Corrupt save */
static void test_truncated_save_is_backed_up_and_reset(void)
{
    tb_save s = sample(), out;
    tb_save_store(&g_io, &s);
    mem_file *f = slot(TB_SAVE_FILE, false);
    size_t full = f->size;
    f->size = full / 2; /* truncate */
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_load(&g_io, &out));
    TEST_ASSERT_EQUAL_UINT32(0, out.completed);
    mem_file *bak = slot(TB_SAVE_BACKUP, false);
    TEST_ASSERT_NOT_NULL(bak);
    TEST_ASSERT_EQUAL_UINT(full / 2, bak->size);
    TEST_ASSERT_NOT_NULL(strstr(tb_save_status_message(TB_SAVE_CORRUPT), "backup"));
}

static void test_flipped_bit_fails_checksum(void)
{
    uint8_t buf[TB_SAVE_MAX_BYTES];
    tb_save s = sample(), out;
    size_t n = tb_save_encode(&s, buf, sizeof(buf));
    buf[20] ^= 0x10;
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, n, &out));
}

static void test_header_problems(void)
{
    uint8_t buf[TB_SAVE_MAX_BYTES];
    tb_save s = sample(), out;
    size_t n = tb_save_encode(&s, buf, sizeof(buf));

    uint8_t bad[TB_SAVE_MAX_BYTES];
    memcpy(bad, buf, n);
    bad[0] = 'X'; /* magic */
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(bad, n, &out));

    memcpy(bad, buf, n);
    bad[4] = 2; /* newer version */
    TEST_ASSERT_EQUAL_INT(TB_SAVE_TOO_NEW, tb_save_decode(bad, n, &out));

    memcpy(bad, buf, n);
    bad[4] = 0; /* version 0 never existed */
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(bad, n, &out));

    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, n + 1, &out)); /* trailing bytes */
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, 3, &out));
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(NULL, 0, &out));
}

/* Re-signs a modified payload so only the value check can catch it. */
static void resign(uint8_t *buf, size_t n)
{
    size_t payload = n - 12;
    uint32_t crc = tb_crc32(buf + 8, payload);
    for (int i = 0; i < 4; i++) {
        buf[8 + payload + (size_t)i] = (uint8_t)(crc >> (8 * i));
    }
}

static void test_out_of_range_values_rejected(void)
{
    uint8_t buf[TB_SAVE_MAX_BYTES];
    tb_save out;
    tb_save s = sample();
    s.floppies[5] = 8; /* only three floppies per level */
    size_t n = tb_save_encode(&s, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, n, &out));

    const size_t mask_offsets[] = {8, 12, 8 + 4 + 4 + TB_PORTAL_COUNT, 8 + 4 + 4 + TB_PORTAL_COUNT + 4};
    for (size_t i = 0; i < 4; i++) {
        s = sample();
        n = tb_save_encode(&s, buf, sizeof(buf));
        buf[mask_offsets[i] + 3] = 0x80; /* bit 31: no portal 32 */
        resign(buf, n);
        TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, n, &out));
    }
}

static void test_payload_length_mismatch(void)
{
    uint8_t buf[TB_SAVE_MAX_BYTES];
    tb_save s = sample(), out;
    size_t n = tb_save_encode(&s, buf, sizeof(buf));
    /* Claim a shorter payload and resize the file to match: the reader
     * then runs out of payload before all fields are read. */
    size_t shorter = n - 12 - 8;
    buf[6] = (uint8_t)(shorter & 0xFF);
    buf[7] = (uint8_t)(shorter >> 8);
    uint32_t crc = tb_crc32(buf + 8, shorter);
    for (int i = 0; i < 4; i++) {
        buf[8 + shorter + (size_t)i] = (uint8_t)(crc >> (8 * i));
    }
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, 8 + shorter + 4, &out));

    /* Claim a longer payload padded with zeros: all fields read fine but
     * bytes are left over. */
    n = tb_save_encode(&s, buf, sizeof(buf));
    size_t longer = n - 12 + 4;
    memset(buf + n - 4, 0, 4);
    buf[6] = (uint8_t)(longer & 0xFF);
    buf[7] = (uint8_t)(longer >> 8);
    crc = tb_crc32(buf + 8, longer);
    for (int i = 0; i < 4; i++) {
        buf[8 + longer + (size_t)i] = (uint8_t)(crc >> (8 * i));
    }
    TEST_ASSERT_EQUAL_INT(TB_SAVE_CORRUPT, tb_save_decode(buf, 8 + longer + 4, &out));
}

static void test_encode_buffer_too_small(void)
{
    uint8_t buf[16];
    tb_save s = sample();
    TEST_ASSERT_EQUAL_UINT(0, tb_save_encode(&s, buf, sizeof(buf)));
    uint8_t big[TB_SAVE_MAX_BYTES];
    size_t full = tb_save_encode(&s, big, sizeof(big));
    TEST_ASSERT_EQUAL_UINT(0, tb_save_encode(&s, big, full - 2)); /* no room for CRC */
}

static void test_store_failure(void)
{
    tb_save s = sample();
    g_write_fails = true;
    TEST_ASSERT_FALSE(tb_save_store(&g_io, &s));
}

static void test_status_messages(void)
{
    TEST_ASSERT_NOT_NULL(strstr(tb_save_status_message(TB_SAVE_OK), "loaded"));
    TEST_ASSERT_NOT_NULL(strstr(tb_save_status_message(TB_SAVE_MISSING), "New game"));
    TEST_ASSERT_NOT_NULL(strstr(tb_save_status_message(TB_SAVE_TOO_NEW), "newer"));
    TEST_ASSERT_EQUAL_STRING("?", tb_save_status_message((tb_save_status)42));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_roundtrip);
    RUN_TEST(test_missing_file_gives_defaults);
    RUN_TEST(test_truncated_save_is_backed_up_and_reset);
    RUN_TEST(test_flipped_bit_fails_checksum);
    RUN_TEST(test_header_problems);
    RUN_TEST(test_out_of_range_values_rejected);
    RUN_TEST(test_payload_length_mismatch);
    RUN_TEST(test_encode_buffer_too_small);
    RUN_TEST(test_store_failure);
    RUN_TEST(test_status_messages);
    return UNITY_END();
}
