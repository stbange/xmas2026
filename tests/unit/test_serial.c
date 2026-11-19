/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/serial.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_roundtrip_all_types(void)
{
    uint8_t buf[128];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_u8(&w, 0xAB);
    tb_write_u16(&w, 0xBEEF);
    tb_write_u32(&w, 0xDEADBEEFu);
    tb_write_u64(&w, 0x0123456789ABCDEFull);
    tb_write_i32(&w, -42);
    tb_write_f32(&w, 3.5f);
    tb_write_bool(&w, true);
    tb_write_bool(&w, false);
    tb_write_bytes(&w, "xyz", 3);
    tb_write_str(&w, "NORTHPOLE");
    TEST_ASSERT_TRUE(w.ok);

    tb_reader r;
    tb_reader_init(&r, buf, w.pos);
    TEST_ASSERT_EQUAL_HEX8(0xAB, tb_read_u8(&r));
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, tb_read_u16(&r));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEFu, tb_read_u32(&r));
    TEST_ASSERT_EQUAL_HEX64(0x0123456789ABCDEFull, tb_read_u64(&r));
    TEST_ASSERT_EQUAL_INT32(-42, tb_read_i32(&r));
    TEST_ASSERT_EQUAL_FLOAT(3.5f, tb_read_f32(&r));
    TEST_ASSERT_TRUE(tb_read_bool(&r));
    TEST_ASSERT_FALSE(tb_read_bool(&r));
    char bytes[3];
    tb_read_bytes(&r, bytes, 3);
    TEST_ASSERT_EQUAL_MEMORY("xyz", bytes, 3);
    char s[16];
    tb_read_str(&r, s, sizeof(s));
    TEST_ASSERT_EQUAL_STRING("NORTHPOLE", s);
    TEST_ASSERT_TRUE(r.ok);
    TEST_ASSERT_EQUAL_UINT(0, tb_reader_remaining(&r));
}

static void test_little_endian_layout(void)
{
    uint8_t buf[4];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_u32(&w, 0x11223344u);
    TEST_ASSERT_EQUAL_HEX8(0x44, buf[0]);
    TEST_ASSERT_EQUAL_HEX8(0x11, buf[3]);
}

static void test_writer_overflow_is_sticky(void)
{
    uint8_t buf[3];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_u16(&w, 1);
    tb_write_u16(&w, 2); /* does not fit */
    TEST_ASSERT_FALSE(w.ok);
    tb_write_u8(&w, 3); /* would fit, but error is sticky */
    TEST_ASSERT_EQUAL_UINT(2, w.pos);
    tb_writer null_w;
    tb_writer_init(&null_w, NULL, 10);
    tb_write_u8(&null_w, 1);
    TEST_ASSERT_FALSE(null_w.ok);
}

static void test_reader_truncated_input(void)
{
    uint8_t buf[2] = {1, 2};
    tb_reader r;
    tb_reader_init(&r, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_UINT32(0, tb_read_u32(&r));
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_UINT(0, tb_reader_remaining(&r));
    uint8_t out[4] = {9, 9, 9, 9};
    tb_read_bytes(&r, out, 4);
    TEST_ASSERT_EQUAL_UINT8(9, out[0]);
    tb_reader null_r;
    tb_reader_init(&null_r, NULL, 5);
    tb_read_u8(&null_r);
    TEST_ASSERT_FALSE(null_r.ok);
}

static void test_bool_rejects_garbage(void)
{
    uint8_t buf[1] = {7};
    tb_reader r;
    tb_reader_init(&r, buf, 1);
    TEST_ASSERT_FALSE(tb_read_bool(&r));
    TEST_ASSERT_FALSE(r.ok);
}

static void test_string_limits(void)
{
    uint8_t buf[64];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_str(&w, "too long for out");
    tb_reader r;
    tb_reader_init(&r, buf, w.pos);
    char small[4];
    tb_read_str(&r, small, sizeof(small));
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("", small);

    /* Declared length longer than the data. */
    uint8_t bad[3] = {10, 0, 'a'};
    tb_reader_init(&r, bad, sizeof(bad));
    char out[32];
    tb_read_str(&r, out, sizeof(out));
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("", out);

    /* Zero-capacity output. */
    tb_reader_init(&r, buf, w.pos);
    tb_read_str(&r, out, 0);
    TEST_ASSERT_FALSE(r.ok);

    /* Strings longer than 65535 bytes cannot be written. */
    static char huge[70000];
    memset(huge, 'x', sizeof(huge) - 1);
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_str(&w, huge);
    TEST_ASSERT_FALSE(w.ok);
}

static void test_zero_length_bytes(void)
{
    uint8_t buf[1];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_write_bytes(&w, NULL, 0);
    TEST_ASSERT_TRUE(w.ok);
    tb_reader r;
    tb_reader_init(&r, buf, 0);
    tb_read_bytes(&r, NULL, 0);
    TEST_ASSERT_TRUE(r.ok);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_roundtrip_all_types);
    RUN_TEST(test_little_endian_layout);
    RUN_TEST(test_writer_overflow_is_sticky);
    RUN_TEST(test_reader_truncated_input);
    RUN_TEST(test_bool_rejects_garbage);
    RUN_TEST(test_string_limits);
    RUN_TEST(test_zero_length_bytes);
    return UNITY_END();
}
