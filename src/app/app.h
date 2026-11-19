/* SPDX-License-Identifier: MIT */
/* Platform-independent application loop: fixed 60 Hz simulation with
 * interpolated rendering, or (headless) one tick per frame as fast as
 * possible with deterministic results. */
#ifndef TB_APP_APP_H
#define TB_APP_APP_H

#include "app/args.h"
#include "core/rng.h"
#include "core/timestep.h"

#define TB_SIM_HZ 60u

typedef enum tb_app_status { TB_APP_RUNNING = 0, TB_APP_DONE = 1 } tb_app_status;

/* Placeholder world until the simulation module exists: a few drifting
 * snowflakes, enough to prove determinism end to end. */
#define TB_APP_SNOWFLAKES 64
typedef struct tb_snowflake {
    float x, y, vx, vy;
} tb_snowflake;

typedef struct tb_app {
    tb_args args;
    tb_timestep ts;
    tb_rng rng;
    tb_snowflake snow[TB_APP_SNOWFLAKES];
    uint64_t quit_requested;
} tb_app;

void tb_app_init(tb_app *app, const tb_args *args);
/* Advances the app for a frame at real time `now_seconds`; returns how many
 * ticks were simulated via *ticks_out (may be NULL). */
tb_app_status tb_app_frame(tb_app *app, double now_seconds, uint32_t *ticks_out);
void tb_app_request_quit(tb_app *app);
uint64_t tb_app_tick(const tb_app *app);
/* Checksum of the simulation state (replays and determinism tests). */
uint64_t tb_app_checksum(const tb_app *app);

#endif
