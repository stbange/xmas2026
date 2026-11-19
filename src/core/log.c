/* SPDX-License-Identifier: MIT */
#include "core/log.h"

#include <stdio.h>

#define TB_LOG_MAX 1024

static void default_sink(void *user, tb_log_level level, const char *message)
{
    (void)user;
    fprintf(stderr, "[%s] %s\n", tb_log_level_name(level), message);
}

/* Process-wide logging configuration (the one sanctioned global: a logger). */
static tb_log_sink_fn g_sink = default_sink;
static void *g_user;
static tb_log_level g_min_level = TB_LOG_INFO;

void tb_log_set_sink(tb_log_sink_fn sink, void *user)
{
    g_sink = sink ? sink : default_sink;
    g_user = sink ? user : NULL;
}

void tb_log_set_min_level(tb_log_level level)
{
    g_min_level = level;
}

tb_log_level tb_log_min_level(void)
{
    return g_min_level;
}

const char *tb_log_level_name(tb_log_level level)
{
    switch (level) {
    case TB_LOG_DEBUG:
        return "debug";
    case TB_LOG_INFO:
        return "info";
    case TB_LOG_WARN:
        return "warn";
    case TB_LOG_ERROR:
        return "error";
    default:
        return "?";
    }
}

void tb_logv(tb_log_level level, const char *fmt, va_list args)
{
    if (level < g_min_level || level >= TB_LOG_OFF) {
        return;
    }
    char buf[TB_LOG_MAX];
    /* `args` is initialised by the caller (tb_log or an external va_start). */
    vsnprintf(buf, sizeof(buf), fmt, args); /* NOLINT(clang-analyzer-valist.Uninitialized) */
    g_sink(g_user, level, buf);
}

void tb_log(tb_log_level level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    tb_logv(level, fmt, args);
    va_end(args);
}
