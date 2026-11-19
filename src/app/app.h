/* SPDX-License-Identifier: MIT */
/* Platform-independent application loop: fixed 60 Hz simulation with
 * interpolated rendering, or (headless) one tick per frame as fast as
 * possible with deterministic results. */
#ifndef TB_APP_APP_H
#define TB_APP_APP_H

#include "app/args.h"
#include "core/timestep.h"
#include "sim/sim.h"

#define TB_SIM_HZ TB_SIM_HZ_TICKS

typedef enum tb_app_status { TB_APP_RUNNING = 0, TB_APP_DONE = 1 } tb_app_status;

typedef struct tb_app {
    tb_args args;
    tb_timestep ts;
    /* Until levels are loaded from content packages the app runs the
     * built-in test arena with a few targets. */
    tb_brush brushes[TB_TEST_ARENA_BRUSHES];
    tb_geometry geometry;
    tb_sim sim;
    tb_input input[TB_MAX_PLAYERS]; /* latest input per player, applied each tick */
    bool quit_requested;
} tb_app;

void tb_app_init(tb_app *app, const tb_args *args);
/* Sets the input used for the next ticks for a player slot. */
void tb_app_set_input(tb_app *app, int slot, const tb_input *in);
/* Advances the app for a frame at real time `now_seconds`; returns how many
 * ticks were simulated via *ticks_out (may be NULL). */
tb_app_status tb_app_frame(tb_app *app, double now_seconds, uint32_t *ticks_out);
void tb_app_request_quit(tb_app *app);
uint64_t tb_app_tick(const tb_app *app);
/* Checksum of the simulation state (replays and determinism tests). */
uint64_t tb_app_checksum(const tb_app *app);

#endif
