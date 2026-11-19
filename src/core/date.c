/* SPDX-License-Identifier: MIT */
#include "core/date.h"

#include <string.h>

static bool is_leap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

bool tb_date_is_valid(tb_date d)
{
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d.year < 1970 || d.year > 9999 || d.month < 1 || d.month > 12 || d.day < 1) {
        return false;
    }
    int max = days[d.month - 1] + (d.month == 2 && is_leap(d.year) ? 1 : 0);
    return d.day <= max;
}

bool tb_parse_date(const char *s, tb_date *out)
{
    if (!s || strlen(s) != 10 || s[4] != '-' || s[7] != '-') {
        return false;
    }
    int v[3] = {0, 0, 0};
    const int start[3] = {0, 5, 8}, len[3] = {4, 2, 2};
    for (int f = 0; f < 3; f++) {
        for (int i = 0; i < len[f]; i++) {
            char c = s[start[f] + i];
            if (c < '0' || c > '9') {
                return false;
            }
            v[f] = v[f] * 10 + (c - '0');
        }
    }
    tb_date d = {v[0], v[1], v[2]};
    if (!tb_date_is_valid(d)) {
        return false;
    }
    *out = d;
    return true;
}
