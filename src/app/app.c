/* SPDX-License-Identifier: MIT */
#include "app/app.h"

#include <string.h>

#define TB_MAX_STEPS_PER_FRAME 8u

void tb_app_init(tb_app *app, const tb_args *args)
{
    memset(app, 0, sizeof(*app));
    app->args = *args;
    tb_timestep_init(&app->ts, TB_SIM_HZ, TB_MAX_STEPS_PER_FRAME);
    tb_geometry_build_test_arena(&app->geometry, app->brushes);

    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &app->geometry;
    cfg.seed = args->seed;
    cfg.difficulty = TB_DIFF_COLA;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        cfg.spawn[i] = tb_v3(-2.0f * (float)i, -15.0f, 0.01f);
    }
    cfg.spawn_yaw = TB_PI / 2.0f;
    tb_sim_init(&app->sim, &cfg);
    tb_sim_add_player(&app->sim, 0);
    for (int i = 0; i < 3; i++) {
        tb_sim_add_target(&app->sim, tb_v3(-4.0f + 4.0f * (float)i, 0.0f, 0.9f), 50);
    }
}

void tb_app_set_input(tb_app *app, int slot, const tb_input *in)
{
    if (slot >= 0 && slot < TB_MAX_PLAYERS) {
        app->input[slot] = *in;
    }
}

tb_app_status tb_app_frame(tb_app *app, double now_seconds, uint32_t *ticks_out)
{
    uint32_t steps;
    if (app->args.headless) {
        steps = 1;
        app->ts.tick++;
    } else {
        steps = tb_timestep_advance(&app->ts, now_seconds);
    }
    for (uint32_t i = 0; i < steps; i++) {
        tb_sim_step(&app->sim, app->input);
    }
    if (ticks_out) {
        *ticks_out = steps;
    }
    if (app->quit_requested || (app->args.max_ticks && app->ts.tick >= app->args.max_ticks)) {
        return TB_APP_DONE;
    }
    return TB_APP_RUNNING;
}

void tb_app_request_quit(tb_app *app)
{
    app->quit_requested = true;
}

uint64_t tb_app_tick(const tb_app *app)
{
    return app->ts.tick;
}

uint64_t tb_app_checksum(const tb_app *app)
{
    return tb_sim_checksum(&app->sim);
}
