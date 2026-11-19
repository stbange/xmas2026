/* SPDX-License-Identifier: MIT */
#include "app/app.h"

#include "core/hash.h"

#include <string.h>

#define TB_MAX_STEPS_PER_FRAME 8u

static void sim_tick(tb_app *app)
{
    const float dt = 1.0f / (float)TB_SIM_HZ;
    for (int i = 0; i < TB_APP_SNOWFLAKES; i++) {
        tb_snowflake *s = &app->snow[i];
        s->vx += (tb_rng_float01(&app->rng) - 0.5f) * 0.02f;
        s->x += s->vx * dt;
        s->y -= s->vy * dt;
        if (s->y < 0.0f) {
            s->y += 1.0f;
            s->x = tb_rng_float01(&app->rng);
        }
    }
}

void tb_app_init(tb_app *app, const tb_args *args)
{
    memset(app, 0, sizeof(*app));
    app->args = *args;
    tb_timestep_init(&app->ts, TB_SIM_HZ, TB_MAX_STEPS_PER_FRAME);
    tb_rng_seed(&app->rng, args->seed, 0x5eed);
    for (int i = 0; i < TB_APP_SNOWFLAKES; i++) {
        app->snow[i].x = tb_rng_float01(&app->rng);
        app->snow[i].y = tb_rng_float01(&app->rng);
        app->snow[i].vy = 0.05f + 0.1f * tb_rng_float01(&app->rng);
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
        sim_tick(app);
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
    app->quit_requested = 1;
}

uint64_t tb_app_tick(const tb_app *app)
{
    return app->ts.tick;
}

uint64_t tb_app_checksum(const tb_app *app)
{
    uint64_t h = tb_fnv1a64(&app->ts.tick, sizeof(app->ts.tick));
    h = tb_fnv1a64_update(h, &app->rng, sizeof(app->rng));
    return tb_fnv1a64_update(h, app->snow, sizeof(app->snow));
}
