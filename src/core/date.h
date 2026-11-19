/* SPDX-License-Identifier: MIT */
/* Calendar dates (proleptic Gregorian). */
#ifndef TB_CORE_DATE_H
#define TB_CORE_DATE_H

#include "core/types.h"

typedef struct tb_date {
    int year, month, day;
} tb_date;

/* Validates and parses "YYYY-MM-DD" (years 1970-9999). */
bool tb_parse_date(const char *s, tb_date *out);
bool tb_date_is_valid(tb_date d);

#endif
