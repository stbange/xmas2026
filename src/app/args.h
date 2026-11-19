/* SPDX-License-Identifier: MIT */
/* Command-line options (the web shell turns URL parameters into the same
 * arguments: ?playtest=1 -> --playtest, ?date=YYYY-MM-DD -> --date ...). */
#ifndef TB_APP_ARGS_H
#define TB_APP_ARGS_H

#include "core/types.h"

typedef struct tb_date {
    int year, month, day;
} tb_date;

typedef struct tb_args {
    bool headless; /* no window/GPU/audio, simulate as fast as possible */
    bool playtest; /* open all portals + debug overlay */
    bool help;     /* print usage and exit */
    bool has_date; /* --date given (only honoured if the build allows it) */
    tb_date date;
    uint64_t max_ticks; /* 0 = run until quit */
    uint64_t seed;
} tb_args;

/* Parses argv (argv[0] is the program name). On error returns false and
 * writes a message to err. */
bool tb_args_parse(tb_args *out, int argc, const char *const *argv, char *err, size_t err_len);
/* Validates and parses "YYYY-MM-DD" (proleptic Gregorian, years 1970-9999). */
bool tb_parse_date(const char *s, tb_date *out);
bool tb_date_is_valid(tb_date d);
const char *tb_args_usage(void);

#endif
