/* SPDX-License-Identifier: MIT */
#include "platform/host.h"

#include "core/log.h"
#include "core/version.h"
#include "platform/fs.h"

#include <SDL3/SDL.h>
#include <string.h>

static void sdl_log_sink(void *user, tb_log_level level, const char *message)
{
    (void)user;
    static const SDL_LogPriority prio[] = {SDL_LOG_PRIORITY_DEBUG, SDL_LOG_PRIORITY_INFO,
                                           SDL_LOG_PRIORITY_WARN, SDL_LOG_PRIORITY_ERROR};
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, prio[level], "%s", message);
}

double tb_host_now(const tb_host *host)
{
    return (double)(SDL_GetPerformanceCounter() - host->start_counter) /
           (double)SDL_GetPerformanceFrequency();
}

void tb_host_update_calendar(tb_host *host)
{
    SDL_Time now_ns = 0;
    SDL_DateTime local;
    SDL_zero(local);
    SDL_GetCurrentTime(&now_ns);
    SDL_TimeToDateTime(now_ns, &local, true);

    tb_calendar_input in;
    SDL_zero(in);
    in.local_clock_utc =
        host->clock_override_utc ? host->clock_override_utc : (int64_t)(now_ns / SDL_NS_PER_SECOND);
    in.utc_offset = local.utc_offset;
    in.persisted = host->save.unlocked;
    in.playtest = host->app.args.playtest;
#ifdef TB_ALLOW_DATE_OVERRIDE
    in.allow_date_override = true;
#endif
    in.has_date_override = host->app.args.has_date;
    in.date_override = host->app.args.date;
    tb_calendar_evaluate(&in, &host->calendar);

    if (host->calendar.persist != host->save.unlocked) {
        host->save.unlocked = host->calendar.persist;
        if (!tb_save_store(&host->storage.io, &host->save)) {
            tb_log_warn("could not save progress");
        }
    }
}

bool tb_host_load_progress(tb_host *host)
{
    host->progress_loaded = true;
    if (!tb_storage_init(&host->storage, host->app.args.data_dir)) {
        return false;
    }
    tb_save_status st = tb_save_load(&host->storage.io, &host->save);
    tb_log_info("%s", tb_save_status_message(st));
    tb_storage_load_settings(&host->storage, &host->settings);
    tb_host_update_calendar(host);

    char open[96], next[48] = "";
    tb_portal_mask_text(host->calendar.open, open, sizeof(open));
    for (int n = 1; n <= TB_PORTAL_COUNT; n++) {
        if (!tb_portal_is_open(host->calendar.open, n)) {
            char cd[40];
            tb_portal_countdown_text(&host->calendar, n, cd, sizeof(cd));
            SDL_snprintf(next, sizeof(next), "; door %d %s", n, cd);
            break;
        }
    }
    tb_log_info("calendar: portals open: %s%s", open, next);
    if (host->calendar.clock_corrected) {
        tb_log_info("NOEL: %s", tb_calendar_time_travel_message());
    }
    return true;
}

tb_host_result tb_host_init(tb_host *host, int argc, const char *const *argv)
{
    memset(host, 0, sizeof(*host));
    tb_log_set_sink(sdl_log_sink, NULL);

    tb_args args;
    char err[128];
    if (!tb_args_parse(&args, argc, argv, err, sizeof(err))) {
        tb_log_error("%s\n%s", err, tb_args_usage());
        return TB_HOST_FAILURE;
    }
    if (args.help) {
        tb_log_info("%s", tb_args_usage());
        return TB_HOST_SUCCESS;
    }
#ifndef TB_ALLOW_DATE_OVERRIDE
    if (args.has_date) {
        tb_log_warn("--date is ignored in this build");
        args.has_date = false;
    }
#endif
    tb_log_info("%s%s%s", tb_version_banner(), args.headless ? " [headless]" : "",
                args.playtest ? " [playtest]" : "");

    SDL_SetAppMetadata("Advent of LAN", tb_version(), "io.github.stbange.xmas2026");
    if (!SDL_Init(args.headless ? SDL_INIT_EVENTS : (SDL_INIT_VIDEO | SDL_INIT_EVENTS))) {
        tb_log_error("SDL_Init: %s", SDL_GetError());
        return TB_HOST_FAILURE;
    }
    host->sdl_initialised = true;
    host->start_counter = SDL_GetPerformanceCounter();
    tb_fs_init_persistent();

    if (!args.headless) {
        if (!tb_window_create(&host->window, tb_version_banner(), 1280, 720)) {
            return TB_HOST_FAILURE;
        }
        host->has_window = true;
    }
    tb_app_init(&host->app, &args);
    if (tb_fs_persistent_ready() && !tb_host_load_progress(host)) {
        return TB_HOST_FAILURE;
    }
    return TB_HOST_CONTINUE;
}

tb_host_result tb_host_event(tb_host *host, const void *event)
{
    const SDL_Event *e = event;
    (void)host;
    if (e->type == SDL_EVENT_QUIT) {
        return TB_HOST_SUCCESS;
    }
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_ESCAPE) {
        return TB_HOST_SUCCESS;
    }
    return TB_HOST_CONTINUE;
}

tb_host_result tb_host_iterate(tb_host *host)
{
    if (!host->progress_loaded && tb_fs_persistent_ready() && !tb_host_load_progress(host)) {
        return TB_HOST_FAILURE;
    }
    tb_app_status status = tb_app_frame(&host->app, tb_host_now(host), NULL);
    if (host->has_window) {
        /* Placeholder frame until the renderer exists: midnight blue that
         * brightens with the drifting snow. */
        float glow = host->app.snow[0].y;
        tb_window_present_clear(&host->window, 0.05f, 0.06f + 0.04f * glow, 0.13f + 0.08f * glow);
    }
    if (status == TB_APP_DONE) {
        tb_log_info("ticks=%llu checksum=%016llx", (unsigned long long)tb_app_tick(&host->app),
                    (unsigned long long)tb_app_checksum(&host->app));
        return TB_HOST_SUCCESS;
    }
    return TB_HOST_CONTINUE;
}

void tb_host_quit(tb_host *host)
{
    if (host->has_window) {
        tb_window_destroy(&host->window);
        host->has_window = false;
    }
    if (host->sdl_initialised) {
        SDL_Quit();
        host->sdl_initialised = false;
    }
    tb_log_set_sink(NULL, NULL);
}
