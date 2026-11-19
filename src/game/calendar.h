/* SPDX-License-Identifier: MIT */
/* The advent calendar: which of the 24 portals are open. Pure functions;
 * the platform supplies the clock (UTC seconds + local UTC offset). */
#ifndef TB_GAME_CALENDAR_H
#define TB_GAME_CALENDAR_H

#include "core/date.h"
#include "core/types.h"

#define TB_PORTAL_COUNT   24
#define TB_CALENDAR_YEAR  2026
#define TB_CALENDAR_MONTH 12
/* Local clock more than this far ahead of the server counts as tampering. */
#define TB_CLOCK_TOLERANCE_SECONDS ((int64_t)36 * 3600)

/* Bit (n - 1) set = portal n open. */
typedef uint32_t tb_portal_mask;
#define TB_ALL_PORTALS ((tb_portal_mask)((1u << TB_PORTAL_COUNT) - 1u))

typedef struct tb_calendar_input {
    int64_t local_clock_utc; /* device clock, seconds since 1970 UTC */
    int32_t utc_offset;      /* device time zone offset in seconds (CET = +3600) */
    bool has_server_time;
    int64_t server_utc;       /* relay server time, if known */
    tb_portal_mask persisted; /* portals opened before on this device */
    bool playtest;            /* playtest build switch: everything open */
    bool allow_date_override; /* build compiled with date override support */
    bool has_date_override;
    tb_date date_override; /* pretend it is this local date, 00:00 */
} tb_calendar_input;

typedef struct tb_calendar_state {
    int64_t effective_local;  /* seconds since 1970 in local wall-clock time */
    tb_portal_mask open;      /* portals open now */
    tb_portal_mask persist;   /* mask to save (never includes playtest-only opens) */
    tb_portal_mask new_today; /* opened within the last 24 h by the calendar */
    bool clock_corrected;     /* local clock was too far ahead: server time used */
} tb_calendar_state;

void tb_calendar_evaluate(const tb_calendar_input *in, tb_calendar_state *out);
bool tb_portal_is_open(tb_portal_mask mask, int portal);
/* Seconds until portal opens (0 if open or already due). */
int64_t tb_portal_seconds_until(const tb_calendar_state *s, int portal);
/* "opens in 1 d 12 h" / "opens in 5 h 3 min" / "opens in 42 s"; empty if open. */
void tb_portal_countdown_text(const tb_calendar_state *s, int portal, char *out, size_t out_len);
/* 0 before 1 December 2026, 1..24 during the calendar, 25 afterwards. */
int tb_calendar_day(const tb_calendar_state *s);
/* In an online session the host's open mask applies to everyone. */
tb_portal_mask tb_calendar_session_mask(tb_portal_mask host_open);
/* "1-7, 10, 12-13", or "none". */
void tb_portal_mask_text(tb_portal_mask mask, char *out, size_t out_len);
/* NOEL's line when the clock was corrected. */
const char *tb_calendar_time_travel_message(void);

/* Calendar helpers (proleptic Gregorian). */
int64_t tb_days_from_civil(int y, int m, int d);
void tb_civil_from_days(int64_t days, int *y, int *m, int *d);

#endif
