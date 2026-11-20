/* SPDX-License-Identifier: MIT */
/* The game session: fixed 60 Hz simulation and the flow between the hall,
 * a level's briefing, the level and its results screen. Platform
 * independent; the host feeds input, time and progress, and reads back the
 * state to render and to save. */
#ifndef TB_APP_APP_H
#define TB_APP_APP_H

#include "app/args.h"
#include "core/timestep.h"
#include "game/bot.h"
#include "game/hall.h"
#include "game/level.h"
#include "game/save.h"
#include "sim/sim.h"

#define TB_SIM_HZ         TB_SIM_HZ_TICKS
#define TB_BRIEFING_TICKS (6u * TB_SIM_HZ_TICKS) /* auto-continue after 6 s */
#define TB_RESULTS_TICKS  (10u * TB_SIM_HZ_TICKS)
#define TB_MESSAGE_TICKS  (5u * TB_SIM_HZ_TICKS)

typedef enum tb_app_status { TB_APP_RUNNING = 0, TB_APP_DONE = 1 } tb_app_status;

typedef enum tb_flow_state {
    TB_FLOW_HALL = 0,
    TB_FLOW_BRIEFING,
    TB_FLOW_LEVEL,
    TB_FLOW_RESULTS
} tb_flow_state;

typedef struct tb_app {
    tb_args args;
    tb_timestep ts;
    tb_hall hall;
    tb_brush brushes[TB_LEVEL_MAX_BRUSHES]; /* hall or level */
    tb_geometry geometry;
    tb_sim sim;
    tb_input input[TB_MAX_PLAYERS]; /* latest input per player, applied each tick */
    uint16_t prev_buttons;

    /* Progress (owned by the host's save; copied in, changes flagged). */
    tb_save save;
    tb_calendar_state calendar;
    tb_portal_mask open; /* = calendar.open */
    int calendar_day;    /* 0 before December, 1..24, 25 after */
    bool save_dirty;     /* host should store `save` */
    bool world_changed;  /* renderer should rebuild the world mesh */

    tb_flow_state flow;
    int level_day; /* level being briefed / played / scored */
    const tb_level_def *level;
    tb_script *script; /* the running level's script (NULL in the hall) */
    tb_bot bot;        /* --bot: plays the golden path */
    char objective[TB_SCRIPT_TEXT];
    uint32_t flow_ticks;  /* ticks in the current flow state */
    uint64_t level_start; /* sim tick when the level started */
    char results[192];
    char message[160]; /* NOEL's line in the HUD */
    uint32_t message_ticks;
    bool quit_requested;
} tb_app;

void tb_app_init(tb_app *app, const tb_args *args);
/* Frees the level script. */
void tb_app_shutdown(tb_app *app);
/* Sets the player's progress and the calendar; rebuilds the hall's doors
 * if the open set changed. */
void tb_app_set_progress(tb_app *app, const tb_save *save, const tb_calendar_state *calendar);
/* Sets the input used for the next ticks for a player slot. */
void tb_app_set_input(tb_app *app, int slot, const tb_input *in);
/* Advances the app for a frame at real time `now_seconds`; returns how many
 * ticks were simulated via *ticks_out (may be NULL). */
tb_app_status tb_app_frame(tb_app *app, double now_seconds, uint32_t *ticks_out);
void tb_app_request_quit(tb_app *app);
uint64_t tb_app_tick(const tb_app *app);
/* Checksum of the simulation state (replays and determinism tests). */
uint64_t tb_app_checksum(const tb_app *app);
/* Shows a NOEL message in the HUD for a few seconds. */
void tb_app_say(tb_app *app, const char *message);

#endif
