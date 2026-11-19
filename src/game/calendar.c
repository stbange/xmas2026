/* SPDX-License-Identifier: MIT */
#include "game/calendar.h"

#include <stdio.h>

#define DAY 86400

/* Howard Hinnant's days_from_civil / civil_from_days. */
int64_t tb_days_from_civil(int y, int m, int d)
{
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const int64_t yoe = y - era * 400;
    const int64_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

void tb_civil_from_days(int64_t z, int *y, int *m, int *d)
{
    z += 719468;
    const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    const int64_t doe = z - era * 146097;
    const int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const int64_t mp = (5 * doy + 2) / 153;
    const int64_t dd = doy - (153 * mp + 2) / 5 + 1;
    const int64_t mm = mp < 10 ? mp + 3 : mp - 9;
    *y = (int)(yoe + era * 400 + (mm <= 2));
    *m = (int)mm;
    *d = (int)dd;
}

static int64_t portal_opens_at(int portal)
{
    return tb_days_from_civil(TB_CALENDAR_YEAR, TB_CALENDAR_MONTH, portal) * DAY;
}

bool tb_portal_is_open(tb_portal_mask mask, int portal)
{
    return portal >= 1 && portal <= TB_PORTAL_COUNT && (mask >> (portal - 1)) & 1u;
}

void tb_calendar_evaluate(const tb_calendar_input *in, tb_calendar_state *out)
{
    int64_t clock = in->local_clock_utc;
    out->clock_corrected = false;
    if (in->has_server_time && clock - in->server_utc > TB_CLOCK_TOLERANCE_SECONDS) {
        clock = in->server_utc;
        out->clock_corrected = true;
    }
    out->effective_local = clock + in->utc_offset;
    if (in->allow_date_override && in->has_date_override) {
        const tb_date *d = &in->date_override;
        out->effective_local = tb_days_from_civil(d->year, d->month, d->day) * DAY;
    }

    tb_portal_mask by_date = 0, fresh = 0;
    for (int n = 1; n <= TB_PORTAL_COUNT; n++) {
        int64_t since = out->effective_local - portal_opens_at(n);
        if (since >= 0) {
            by_date |= 1u << (n - 1);
            if (since < DAY) {
                fresh |= 1u << (n - 1);
            }
        }
    }
    out->persist = (in->persisted & TB_ALL_PORTALS) | by_date;
    out->open = in->playtest ? TB_ALL_PORTALS : out->persist;
    out->new_today = fresh & ~(in->persisted & TB_ALL_PORTALS);
}

int64_t tb_portal_seconds_until(const tb_calendar_state *s, int portal)
{
    if (tb_portal_is_open(s->open, portal) || portal < 1 || portal > TB_PORTAL_COUNT) {
        return 0;
    }
    int64_t left = portal_opens_at(portal) - s->effective_local;
    return left > 0 ? left : 0;
}

void tb_portal_countdown_text(const tb_calendar_state *s, int portal, char *out, size_t out_len)
{
    int64_t left = tb_portal_seconds_until(s, portal);
    if (left <= 0) {
        snprintf(out, out_len, "%s", "");
    } else if (left >= DAY) {
        snprintf(out, out_len, "opens in %lld d %lld h", (long long)(left / DAY),
                 (long long)(left % DAY / 3600));
    } else if (left >= 3600) {
        snprintf(out, out_len, "opens in %lld h %lld min", (long long)(left / 3600),
                 (long long)(left % 3600 / 60));
    } else if (left >= 60) {
        snprintf(out, out_len, "opens in %lld min", (long long)(left / 60));
    } else {
        snprintf(out, out_len, "opens in %lld s", (long long)left);
    }
}

void tb_portal_mask_text(tb_portal_mask mask, char *out, size_t out_len)
{
    size_t pos = 0;
    out[0] = '\0';
    for (int n = 1; n <= TB_PORTAL_COUNT; n++) {
        if (!tb_portal_is_open(mask, n)) {
            continue;
        }
        int end = n;
        while (end < TB_PORTAL_COUNT && tb_portal_is_open(mask, end + 1)) {
            end++;
        }
        int w = end > n ? snprintf(out + pos, out_len - pos, "%s%d-%d", pos ? ", " : "", n, end)
                        : snprintf(out + pos, out_len - pos, "%s%d", pos ? ", " : "", n);
        if (w < 0 || (size_t)w >= out_len - pos) {
            return; /* truncated */
        }
        pos += (size_t)w;
        n = end;
    }
    if (pos == 0) {
        snprintf(out, out_len, "none");
    }
}

int tb_calendar_day(const tb_calendar_state *s)
{
    int64_t day0 = tb_days_from_civil(TB_CALENDAR_YEAR, TB_CALENDAR_MONTH, 1);
    int64_t today = s->effective_local / DAY; /* dates are after 1970: non-negative */
    int64_t n = today - day0 + 1;
    return n < 1 ? 0 : (n > TB_PORTAL_COUNT ? TB_PORTAL_COUNT + 1 : (int)n);
}

tb_portal_mask tb_calendar_session_mask(tb_portal_mask host_open)
{
    return host_open & TB_ALL_PORTALS;
}

const char *tb_calendar_time_travel_message(void)
{
    return "Nice try, Marty McFly. Your clock says tomorrow; the server says today. "
           "Using the server's date.";
}
