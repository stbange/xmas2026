/* SPDX-License-Identifier: MIT */
#include "platform/host.h"

#include "core/log.h"
#include "core/version.h"
#include "platform/fs.h"
#include "app/hud.h"
#include "app/world_view.h"
#include "render/view.h"

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
#ifdef TB_PLAYTEST_BUILD
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
    tb_app_set_progress(&host->app, &host->save, &host->calendar);
}

/* The session finished a level: store its updated progress. */
static void store_app_progress(tb_host *host)
{
    if (!host->app.save_dirty) {
        return;
    }
    host->app.save_dirty = false;
    host->save = host->app.save;
    if (!tb_save_store(&host->storage.io, &host->save)) {
        tb_log_warn("could not save progress");
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

static bool create_renderer(tb_host *host)
{
#ifdef SDL_PLATFORM_EMSCRIPTEN
    const bool gles = true;
#else
    const bool gles = false;
#endif
    if (!tb_renderer_create(&host->renderer, tb_window_gl_proc, gles)) {
        tb_log_error("renderer: %s", host->renderer.error);
        tb_renderer_destroy(&host->renderer);
        return false;
    }
    host->has_renderer = true;
    host->app.world_changed = true;
    return true;
}

static bool upload_world(tb_host *host)
{
    tb_mesh mesh;
    tb_mesh_init(&mesh, NULL);
    bool ok = tb_world_mesh_build(&host->app, &mesh) && tb_renderer_upload_world(&host->renderer, &mesh);
    tb_mesh_free(&mesh);
    if (!ok) {
        tb_log_error("renderer: could not upload the world");
    }
    host->app.world_changed = false;
    return ok;
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
#ifndef TB_PLAYTEST_BUILD
    /* Release builds (the friends' site) ignore the playtest switches. */
    if (args.has_date) {
        tb_log_warn("--date is ignored in this build");
        args.has_date = false;
    }
    if (args.playtest) {
        tb_log_warn("--playtest is ignored in this build");
        args.playtest = false;
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

    tb_app_init(&host->app, &args);
    tb_controls_init(&host->controls, host->app.sim.players[0].move.yaw);
    if (!args.headless) {
        if (!tb_window_create(&host->window, tb_version_banner(), 1280, 720)) {
            return TB_HOST_FAILURE;
        }
        host->has_window = true;
        if (!create_renderer(host)) {
            return TB_HOST_FAILURE;
        }
    }
    if (tb_fs_persistent_ready() && !tb_host_load_progress(host)) {
        return TB_HOST_FAILURE;
    }
    return TB_HOST_CONTINUE;
}

int tb_host_key_for_scancode(int scancode)
{
    switch ((SDL_Scancode)scancode) {
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
        return TB_KEY_FORWARD;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        return TB_KEY_BACK;
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
        return TB_KEY_LEFT;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
        return TB_KEY_RIGHT;
    case SDL_SCANCODE_SPACE:
        return TB_KEY_JUMP;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_C:
        return TB_KEY_CROUCH;
    case SDL_SCANCODE_LSHIFT:
        return TB_KEY_SPRINT;
    case SDL_SCANCODE_E:
        return TB_KEY_USE;
    case SDL_SCANCODE_V:
    case SDL_SCANCODE_F:
        return TB_KEY_MELEE;
    default:
        if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
            return TB_KEY_WEAPON1 + (scancode - SDL_SCANCODE_1);
        }
        return -1;
    }
}

static void set_mouse_captured(tb_host *host, bool captured)
{
    if (host->has_window) {
        SDL_SetWindowRelativeMouseMode(host->window.window, captured);
    }
    host->mouse_captured = captured;
}

tb_host_result tb_host_event(tb_host *host, const void *event)
{
    const SDL_Event *e = event;
    switch (e->type) {
    case SDL_EVENT_QUIT:
        return TB_HOST_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
        bool down = e->type == SDL_EVENT_KEY_DOWN;
        if (down && e->key.scancode == SDL_SCANCODE_ESCAPE) {
            set_mouse_captured(host, false); /* Escape frees the mouse; it never quits */
            break;
        }
        int key = tb_host_key_for_scancode((int)e->key.scancode);
        if (key >= 0) {
            tb_controls_key(&host->controls, (tb_key)key, down);
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        bool down = e->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        if (down && !host->mouse_captured) {
            set_mouse_captured(host, true); /* first click captures, does not fire */
            break;
        }
        if (e->button.button == SDL_BUTTON_LEFT) {
            tb_controls_key(&host->controls, TB_KEY_FIRE, down);
        } else if (e->button.button == SDL_BUTTON_RIGHT) {
            tb_controls_key(&host->controls, TB_KEY_ALT_FIRE, down);
        }
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
        if (host->mouse_captured) {
            tb_controls_mouse(&host->controls, e->motion.xrel, e->motion.yrel,
                              host->settings.mouse_sensitivity);
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        tb_controls_release_all(&host->controls);
        set_mouse_captured(host, false);
        break;
    default:
        break;
    }
    return TB_HOST_CONTINUE;
}

static bool draw_frame(tb_host *host)
{
    if (host->app.world_changed && !upload_world(host)) {
        return false;
    }
    int w = 0, h = 0;
    tb_window_pixel_size(&host->window, &w, &h);
    tb_view_layout layout = tb_view_layout_for(w, h, (tb_res_preset)host->settings.resolution_preset);
    host->renderer.options.palette_quantize = host->settings.palette_quantize;
    host->renderer.options.dithering = host->settings.dithering;
    float aspect = (float)layout.internal_w / (float)layout.internal_h;
    tb_world_scene(&host->app, &host->scene, host->settings.fov * TB_PI / 180.0f, aspect);
    tb_renderer_draw(&host->renderer, &host->scene, &layout, w, h);
    tb_ui_begin(&host->ui, layout.ui_w, layout.ui_h);
    tb_hud_build(&host->app, &host->scene, aspect, &host->ui);
    tb_renderer_draw_ui(&host->renderer, &host->ui, &layout);
    tb_window_swap(&host->window);
    return true;
}

tb_host_result tb_host_iterate(tb_host *host)
{
    if (!host->progress_loaded && tb_fs_persistent_ready() && !tb_host_load_progress(host)) {
        return TB_HOST_FAILURE;
    }
    tb_input in = tb_controls_input(&host->controls);
    tb_app_set_input(&host->app, 0, &in);
    tb_app_status status = tb_app_frame(&host->app, tb_host_now(host), NULL);
    store_app_progress(host);
    /* Re-check the calendar once a minute so doors open at midnight. */
    if (host->progress_loaded && tb_app_tick(&host->app) % ((uint64_t)60u * TB_SIM_HZ) == 0) {
        tb_host_update_calendar(host);
    }
    if (host->has_renderer && !draw_frame(host)) {
        return TB_HOST_FAILURE;
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
    if (host->has_renderer) {
        tb_renderer_destroy(&host->renderer);
        host->has_renderer = false;
    }
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
