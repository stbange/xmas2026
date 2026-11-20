/* SPDX-License-Identifier: MIT */
/* Command-line options (the web shell turns URL parameters into the same
 * arguments: ?playtest=1 -> --playtest, ?date=YYYY-MM-DD -> --date,
 * ?level=N -> --level N, ?bot=1 -> --bot). */
#ifndef TB_APP_ARGS_H
#define TB_APP_ARGS_H

#include "core/date.h"
#include "core/types.h"

typedef struct tb_args {
    bool headless; /* no window/GPU/audio, simulate as fast as possible */
    bool playtest; /* open all portals + debug overlay */
    bool help;     /* print usage and exit */
    bool has_date; /* --date given (only honoured if the build allows it) */
    int level;     /* --level N: start straight in level N (playtest builds) */
    bool bot;      /* --bot: the golden-path bot plays the level */
    tb_date date;
    uint64_t max_ticks; /* 0 = run until quit */
    uint64_t seed;
    char data_dir[512]; /* saves/settings directory override ("" = default) */
} tb_args;

/* Parses argv (argv[0] is the program name). On error returns false and
 * writes a message to err. */
bool tb_args_parse(tb_args *out, int argc, const char *const *argv, char *err, size_t err_len);
const char *tb_args_usage(void);

#endif
