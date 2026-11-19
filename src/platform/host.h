/* SPDX-License-Identifier: MIT */
/* The host owns SDL, the window and the app loop. The SDL entry point in
 * platform/entry/main.c only forwards SDL's callbacks to these functions. */
#ifndef TB_PLATFORM_HOST_H
#define TB_PLATFORM_HOST_H

#include "app/app.h"
#include "game/calendar.h"
#include "game/save.h"
#include "game/settings.h"
#include "platform/storage.h"
#include "platform/window.h"

typedef enum tb_host_result { TB_HOST_CONTINUE = 0, TB_HOST_SUCCESS = 1, TB_HOST_FAILURE = 2 } tb_host_result;

typedef struct tb_host {
    tb_app app;
    tb_window window;
    bool sdl_initialised;
    bool has_window;
    bool progress_loaded;
    uint64_t start_counter;
    tb_storage storage;
    tb_save save;
    tb_settings settings;
    tb_calendar_state calendar;
    int64_t clock_override_utc; /* tests: fixed "now" in UTC seconds (0 = real clock) */
} tb_host;

/* Loads saves/settings and evaluates the calendar (done once storage is
 * ready: immediately on desktop, after IndexedDB loaded on the web).
 * Returns false if no storage directory is available. */
bool tb_host_load_progress(tb_host *host);
/* Re-evaluates the calendar for the current clock; persists new unlocks. */
void tb_host_update_calendar(tb_host *host);

/* Parses arguments, initialises SDL (video only when not headless) and
 * creates the window. */
tb_host_result tb_host_init(tb_host *host, int argc, const char *const *argv);
/* Handles one SDL event (quit, Escape). `event` is an SDL_Event*. */
tb_host_result tb_host_event(tb_host *host, const void *event);
/* Runs one frame. */
tb_host_result tb_host_iterate(tb_host *host);
void tb_host_quit(tb_host *host);
/* Seconds since tb_host_init. */
double tb_host_now(const tb_host *host);

#endif
