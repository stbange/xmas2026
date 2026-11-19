/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "game/calendar.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

#define CET  3600
#define CEST 7200
#define HOUR 3600
#define DAY  86400

/* UTC seconds for a local wall-clock time in a zone with `offset`. */
static int64_t utc_of_local(int y, int mo, int d, int h, int mi, int s, int32_t offset)
{
    return tb_days_from_civil(y, mo, d) * DAY + h * HOUR + mi * 60 + s - offset;
}

static tb_calendar_state eval_local(int y, int mo, int d, int h, int mi, int s, int32_t offset,
                                    tb_portal_mask persisted)
{
    tb_calendar_input in;
    memset(&in, 0, sizeof(in));
    in.local_clock_utc = utc_of_local(y, mo, d, h, mi, s, offset);
    in.utc_offset = offset;
    in.persisted = persisted;
    tb_calendar_state st;
    tb_calendar_evaluate(&in, &st);
    return st;
}

static tb_portal_mask first_n(int n)
{
    return n <= 0 ? 0u : (tb_portal_mask)((1u << n) - 1u);
}

/* @scenario advent-calendar: Portal opens at local midnight */
static void test_portal_opens_at_local_midnight(void)
{
    tb_calendar_state before = eval_local(2026, 12, 6, 23, 59, 59, CET, 0);
    tb_calendar_state after = eval_local(2026, 12, 7, 0, 0, 0, CET, 0);
    TEST_ASSERT_FALSE(tb_portal_is_open(before.open, 7));
    TEST_ASSERT_TRUE(tb_portal_is_open(after.open, 7));
    TEST_ASSERT_EQUAL_HEX32(first_n(7), after.open);
    for (int n = 8; n <= 24; n++) {
        TEST_ASSERT_FALSE(tb_portal_is_open(after.open, n));
    }
}

static void test_midnight_is_local_in_every_time_zone(void)
{
    const int32_t zones[] = {-10 * HOUR, -5 * HOUR, 0, CET, CEST, 5 * HOUR + 1800, 14 * HOUR};
    for (size_t i = 0; i < sizeof(zones) / sizeof(zones[0]); i++) {
        tb_calendar_state a = eval_local(2026, 12, 13, 23, 59, 59, zones[i], 0);
        tb_calendar_state b = eval_local(2026, 12, 14, 0, 0, 0, zones[i], 0);
        TEST_ASSERT_EQUAL_HEX32(first_n(13), a.open);
        TEST_ASSERT_EQUAL_HEX32(first_n(14), b.open);
    }
}

/* @scenario advent-calendar: Before December */
static void test_before_december(void)
{
    tb_calendar_state st = eval_local(2026, 11, 30, 12, 0, 0, CET, 0);
    TEST_ASSERT_EQUAL_HEX32(0, st.open);
    char text[64];
    for (int n = 1; n <= 24; n++) {
        tb_portal_countdown_text(&st, n, text, sizeof(text));
        TEST_ASSERT_EQUAL_STRING_LEN("opens in ", text, 9);
    }
}

/* @scenario advent-calendar: After Christmas Eve */
static void test_after_christmas_eve(void)
{
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, eval_local(2026, 12, 25, 0, 0, 0, CET, 0).open);
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, eval_local(2027, 7, 1, 9, 0, 0, CEST, 0).open);
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, eval_local(2031, 1, 1, 0, 0, 0, 0, 0).open);
}

/* @scenario advent-calendar: Clock set back */
static void test_clock_set_back_keeps_unlocks(void)
{
    tb_calendar_state day10 = eval_local(2026, 12, 10, 8, 0, 0, CET, 0);
    TEST_ASSERT_TRUE(tb_portal_is_open(day10.persist, 10));
    tb_calendar_state back = eval_local(2026, 12, 5, 8, 0, 0, CET, day10.persist);
    TEST_ASSERT_TRUE(tb_portal_is_open(back.open, 10));
    TEST_ASSERT_EQUAL_HEX32(day10.persist, back.persist);
    TEST_ASSERT_EQUAL_HEX32(0, back.new_today);
}

/* @scenario advent-calendar: Catching up */
static void test_catching_up_on_missed_days(void)
{
    tb_calendar_state st = eval_local(2026, 12, 15, 18, 30, 0, CET, 0);
    for (int n = 1; n <= 15; n++) {
        TEST_ASSERT_TRUE(tb_portal_is_open(st.open, n));
    }
    TEST_ASSERT_FALSE(tb_portal_is_open(st.open, 16));
    /* Only today's door counts as new. */
    TEST_ASSERT_EQUAL_HEX32(1u << 14, st.new_today);
}

/* @scenario advent-calendar: Bumping into a closed door */
static void test_closed_door_shows_countdown(void)
{
    tb_calendar_state st = eval_local(2026, 12, 18, 12, 0, 0, CET, 0);
    TEST_ASSERT_FALSE(tb_portal_is_open(st.open, 20));
    char text[64];
    tb_portal_countdown_text(&st, 20, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("opens in 1 d 12 h", text);
}

static void test_countdown_formats(void)
{
    char text[64];
    tb_calendar_state st = eval_local(2026, 12, 18, 21, 30, 0, CET, 0);
    tb_portal_countdown_text(&st, 19, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("opens in 2 h 30 min", text);
    st = eval_local(2026, 12, 18, 23, 45, 30, CET, 0);
    tb_portal_countdown_text(&st, 19, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("opens in 14 min", text);
    st = eval_local(2026, 12, 18, 23, 59, 18, CET, 0);
    tb_portal_countdown_text(&st, 19, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("opens in 42 s", text);
    tb_portal_countdown_text(&st, 3, text, sizeof(text)); /* open */
    TEST_ASSERT_EQUAL_STRING("", text);
    TEST_ASSERT_EQUAL_INT64(0, tb_portal_seconds_until(&st, 0));
    TEST_ASSERT_EQUAL_INT64(0, tb_portal_seconds_until(&st, 25));
}

/* @scenario advent-calendar: Client is ahead of the host */
static void test_session_uses_host_date(void)
{
    tb_calendar_state host = eval_local(2026, 12, 3, 20, 0, 0, CET, 0);
    tb_calendar_state client = eval_local(2026, 12, 4, 1, 0, 0, CET, 0);
    TEST_ASSERT_TRUE(tb_portal_is_open(client.open, 4));
    tb_portal_mask session = tb_calendar_session_mask(host.open);
    TEST_ASSERT_FALSE(tb_portal_is_open(session, 4));
    TEST_ASSERT_TRUE(tb_portal_is_open(session, 3));
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, tb_calendar_session_mask(0xFFFFFFFFu));
}

/* @scenario advent-calendar: Time traveller */
static void test_time_traveller_uses_server_time(void)
{
    tb_calendar_input in;
    memset(&in, 0, sizeof(in));
    in.utc_offset = CET;
    in.local_clock_utc = utc_of_local(2026, 12, 10, 12, 0, 0, CET);
    in.has_server_time = true;
    in.server_utc = tb_days_from_civil(2026, 12, 2) * DAY + 10 * HOUR;
    tb_calendar_state st;
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_TRUE(st.clock_corrected);
    TEST_ASSERT_EQUAL_HEX32(first_n(2), st.open);
    TEST_ASSERT_NOT_NULL(strstr(tb_calendar_time_travel_message(), "Marty McFly"));

    /* Within tolerance (e.g. slightly wrong clock, or behind): local clock wins. */
    in.local_clock_utc = in.server_utc + TB_CLOCK_TOLERANCE_SECONDS;
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_FALSE(st.clock_corrected);
    in.local_clock_utc = in.server_utc - 5 * DAY;
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_FALSE(st.clock_corrected);
}

/* @scenario advent-calendar: Release build ignores override */
static void test_release_build_ignores_date_override(void)
{
    tb_calendar_input in;
    memset(&in, 0, sizeof(in));
    in.utc_offset = CET;
    in.local_clock_utc = utc_of_local(2026, 12, 2, 9, 0, 0, CET);
    in.has_date_override = true;
    in.date_override = (tb_date){2026, 12, 24};
    in.allow_date_override = false;
    tb_calendar_state st;
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_EQUAL_HEX32(first_n(2), st.open);

    in.allow_date_override = true; /* debug/playtest build */
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, st.open);
}

static void test_playtest_opens_everything_without_persisting(void)
{
    tb_calendar_input in;
    memset(&in, 0, sizeof(in));
    in.local_clock_utc = utc_of_local(2026, 11, 25, 12, 0, 0, 0);
    in.playtest = true;
    tb_calendar_state st;
    tb_calendar_evaluate(&in, &st);
    TEST_ASSERT_EQUAL_HEX32(TB_ALL_PORTALS, st.open);
    TEST_ASSERT_EQUAL_HEX32(0, st.persist);
}

static void test_persisted_garbage_bits_are_ignored(void)
{
    tb_calendar_state st = eval_local(2026, 11, 1, 0, 0, 0, 0, 0xFF000000u);
    TEST_ASSERT_EQUAL_HEX32(0, st.open);
    TEST_ASSERT_FALSE(tb_portal_is_open(TB_ALL_PORTALS, 0));
    TEST_ASSERT_FALSE(tb_portal_is_open(TB_ALL_PORTALS, 25));
}

static void test_mask_text(void)
{
    char text[96];
    tb_portal_mask_text(0, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("none", text);
    tb_portal_mask_text(first_n(7), text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("1-7", text);
    tb_portal_mask_text(first_n(3) | (1u << 9) | (3u << 11) | (1u << 23), text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("1-3, 10, 12-13, 24", text);
    tb_portal_mask_text(0x555555u, text, 8); /* truncates safely */
    TEST_ASSERT_TRUE(strlen(text) < 8);
}

static void test_civil_date_roundtrip(void)
{
    TEST_ASSERT_EQUAL_INT64(0, tb_days_from_civil(1970, 1, 1));
    TEST_ASSERT_EQUAL_INT64(20788, tb_days_from_civil(2026, 12, 1));
    for (int64_t z = -800000; z < 800000; z += 997) {
        int y, m, d;
        tb_civil_from_days(z, &y, &m, &d);
        TEST_ASSERT_EQUAL_INT64(z, tb_days_from_civil(y, m, d));
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_portal_opens_at_local_midnight);
    RUN_TEST(test_midnight_is_local_in_every_time_zone);
    RUN_TEST(test_before_december);
    RUN_TEST(test_after_christmas_eve);
    RUN_TEST(test_clock_set_back_keeps_unlocks);
    RUN_TEST(test_catching_up_on_missed_days);
    RUN_TEST(test_closed_door_shows_countdown);
    RUN_TEST(test_countdown_formats);
    RUN_TEST(test_session_uses_host_date);
    RUN_TEST(test_time_traveller_uses_server_time);
    RUN_TEST(test_release_build_ignores_date_override);
    RUN_TEST(test_playtest_opens_everything_without_persisting);
    RUN_TEST(test_persisted_garbage_bits_are_ignored);
    RUN_TEST(test_mask_text);
    RUN_TEST(test_civil_date_roundtrip);
    return UNITY_END();
}
