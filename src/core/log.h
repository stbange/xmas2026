/* SPDX-License-Identifier: MIT */
/* Logging with a replaceable sink (the platform routes it to SDL_Log, tests
 * capture it). */
#ifndef TB_CORE_LOG_H
#define TB_CORE_LOG_H

#include <stdarg.h>

typedef enum tb_log_level {
    TB_LOG_DEBUG = 0,
    TB_LOG_INFO = 1,
    TB_LOG_WARN = 2,
    TB_LOG_ERROR = 3,
    TB_LOG_OFF = 4
} tb_log_level;

typedef void (*tb_log_sink_fn)(void *user, tb_log_level level, const char *message);

/* NULL restores the default sink (stderr). */
void tb_log_set_sink(tb_log_sink_fn sink, void *user);
void tb_log_set_min_level(tb_log_level level);
tb_log_level tb_log_min_level(void);
const char *tb_log_level_name(tb_log_level level);

#if defined(__GNUC__) || defined(__clang__)
#define TB_PRINTF_LIKE(f, a) __attribute__((format(printf, f, a)))
#else
#define TB_PRINTF_LIKE(f, a)
#endif

void tb_log(tb_log_level level, const char *fmt, ...) TB_PRINTF_LIKE(2, 3);
void tb_logv(tb_log_level level, const char *fmt, va_list args);

#define tb_log_debug(...) tb_log(TB_LOG_DEBUG, __VA_ARGS__)
#define tb_log_info(...)  tb_log(TB_LOG_INFO, __VA_ARGS__)
#define tb_log_warn(...)  tb_log(TB_LOG_WARN, __VA_ARGS__)
#define tb_log_error(...) tb_log(TB_LOG_ERROR, __VA_ARGS__)

#endif
