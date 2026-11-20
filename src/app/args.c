/* SPDX-License-Identifier: MIT */
#include "app/args.h"

#include <stdio.h>
#include <string.h>

static bool parse_u64(const char *s, uint64_t *out)
{
    if (!s || !*s) {
        return false;
    }
    uint64_t v = 0;
    for (const char *p = s; *p; p++) {
        if (*p < '0' || *p > '9') {
            return false;
        }
        uint64_t d = (uint64_t)(*p - '0');
        if (v > (UINT64_MAX - d) / 10u) {
            return false;
        }
        v = v * 10u + d;
    }
    *out = v;
    return true;
}

const char *tb_args_usage(void)
{
    return "usage: advent_of_lan [options]\n"
           "  --headless         run without window, GPU or audio\n"
           "  --ticks N          stop after N simulation ticks\n"
           "  --seed N           simulation seed\n"
           "  --playtest         open all portals, show debug overlay\n"
           "  --date YYYY-MM-DD  pretend it is this date (debug/playtest builds only)\n"
           "  --level N          start in level N (debug/playtest builds only)\n"
           "  --bot              let the test bot play the level (with --level)\n"
           "  --data-dir PATH    store saves and settings in PATH\n"
           "  --help             show this help\n";
}

bool tb_args_parse(tb_args *out, int argc, const char *const *argv, char *err, size_t err_len)
{
    memset(out, 0, sizeof(*out));
    out->seed = 2026;
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        const char *next = i + 1 < argc ? argv[i + 1] : NULL;
        if (strcmp(a, "--headless") == 0) {
            out->headless = true;
        } else if (strcmp(a, "--playtest") == 0) {
            out->playtest = true;
        } else if (strcmp(a, "--bot") == 0) {
            out->bot = true;
        } else if (strcmp(a, "--level") == 0) {
            uint64_t day = 0;
            if (!parse_u64(next, &day) || day < 1 || day > 24) {
                snprintf(err, err_len, "--level needs a day 1-24");
                return false;
            }
            out->level = (int)day;
            i++;
        } else if (strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0) {
            out->help = true;
        } else if (strcmp(a, "--ticks") == 0) {
            if (!parse_u64(next, &out->max_ticks)) {
                snprintf(err, err_len, "--ticks needs a number");
                return false;
            }
            i++;
        } else if (strcmp(a, "--seed") == 0) {
            if (!parse_u64(next, &out->seed)) {
                snprintf(err, err_len, "--seed needs a number");
                return false;
            }
            i++;
        } else if (strcmp(a, "--data-dir") == 0) {
            if (!next || !*next || strlen(next) + 2 > sizeof(out->data_dir)) {
                snprintf(err, err_len, "--data-dir needs a path");
                return false;
            }
            snprintf(out->data_dir, sizeof(out->data_dir), "%s", next);
            i++;
        } else if (strcmp(a, "--date") == 0) {
            if (!tb_parse_date(next, &out->date)) {
                snprintf(err, err_len, "--date needs a valid date YYYY-MM-DD");
                return false;
            }
            out->has_date = true;
            i++;
        } else {
            snprintf(err, err_len, "unknown option '%s'", a);
            return false;
        }
    }
    return true;
}
