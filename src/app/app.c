/* SPDX-License-Identifier: MIT */
#include "app/app.h"

#include <stdio.h>
#include <string.h>

#include "core/log.h"

#define TB_MAX_STEPS_PER_FRAME 8u
#define CONFIRM_BUTTONS        (TB_BTN_FIRE | TB_BTN_USE | TB_BTN_JUMP)

static void end_level(tb_app *app)
{
    tb_script_destroy(app->script);
    app->script = NULL;
    app->level = NULL;
    app->objective[0] = '\0';
}

static void reset_sim(tb_app *app, tb_vec3 spawn, float yaw)
{
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &app->geometry;
    cfg.seed = app->args.seed + (uint64_t)app->level_day;
    cfg.difficulty = TB_DIFF_COLA;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        cfg.spawn[i] = tb_v3_add(spawn, tb_v3(1.2f * (float)i, 0, 0));
    }
    cfg.spawn_yaw = yaw;
    tb_sim_init(&app->sim, &cfg);
    tb_sim_add_player(&app->sim, 0);
    app->world_changed = true;
}

static void load_hall(tb_app *app, tb_vec3 spawn, float yaw)
{
    end_level(app);
    tb_hall_build_geometry(&app->hall, app->open, &app->geometry, app->brushes);
    reset_sim(app, spawn, yaw);
    tb_hall_add_portals(&app->hall, app->open, &app->sim);
    app->flow = TB_FLOW_HALL;
    app->flow_ticks = 0;
}

static void load_level(tb_app *app, int day)
{
    end_level(app);
    const tb_level_def *def = tb_level_def_get(day);
    app->level = def;
    app->level_day = day;
    tb_level_build(def, &app->geometry, app->brushes);
    tb_sim_config cfg;
    tb_level_sim_config(def, &app->geometry, app->args.seed + (uint64_t)day, TB_DIFF_COLA, &cfg);
    tb_sim_init(&app->sim, &cfg);
    tb_sim_add_player(&app->sim, 0);
    app->world_changed = true;
    char err[TB_SCRIPT_TEXT];
    app->script = tb_level_begin(def, &app->sim, err, sizeof(err));
    snprintf(app->objective, sizeof(app->objective), "%s", def->objective ? def->objective : "");
    if (err[0]) {
        tb_log_error("level %d script: %s", day, err);
        tb_app_say(app, "NOEL: My level script crashed. Just find the exit!");
    }
    tb_bot_init(&app->bot, def->golden_path, def->golden_count);
    const tb_level_info *info = tb_level_info_get(day);
    tb_log_info("level %d loaded: %s%s%s", day, info->title, def->placeholder ? " (test course)" : "",
                app->args.bot ? " [bot]" : "");
    app->level_start = app->sim.tick;
    app->flow = TB_FLOW_LEVEL;
    app->flow_ticks = 0;
}

static void level_tick(tb_app *app)
{
    if (app->args.bot) {
        app->input[0] = tb_bot_think(&app->bot, &app->sim, 0);
    }
    tb_sim_step(&app->sim, app->input);
    if (app->script) {
        tb_script_tick(app->script);
        const char *say = tb_script_take_say(app->script);
        if (say) {
            tb_app_say(app, say);
        }
        if (tb_script_objective(app->script)[0]) {
            snprintf(app->objective, sizeof(app->objective), "%s", tb_script_objective(app->script));
        }
    }
}

void tb_app_say(tb_app *app, const char *message)
{
    snprintf(app->message, sizeof(app->message), "%s", message);
    app->message_ticks = TB_MESSAGE_TICKS;
}

void tb_app_init(tb_app *app, const tb_args *args)
{
    memset(app, 0, sizeof(*app));
    app->args = *args;
    tb_timestep_init(&app->ts, TB_SIM_HZ, TB_MAX_STEPS_PER_FRAME);
    tb_save_defaults(&app->save);
    tb_hall_layout(&app->hall);
    load_hall(app, app->hall.spawn, app->hall.spawn_yaw);
    if (args->level) {
        load_level(app, args->level); /* playtest: straight into the level */
    }
}

void tb_app_shutdown(tb_app *app)
{
    end_level(app);
}

void tb_app_set_progress(tb_app *app, const tb_save *save, const tb_calendar_state *calendar)
{
    tb_portal_mask open = calendar->open;
    app->save = *save;
    app->calendar = *calendar;
    app->calendar_day = tb_calendar_day(calendar);
    if (open != app->open) {
        app->open = open;
        if (app->flow == TB_FLOW_HALL) {
            /* Rebuild the doors, keep the player where they are. */
            const tb_player *p = &app->sim.players[0];
            load_hall(app, p->move.origin, p->move.yaw);
        }
    }
}

void tb_app_set_input(tb_app *app, int slot, const tb_input *in)
{
    if (slot >= 0 && slot < TB_MAX_PLAYERS) {
        app->input[slot] = *in;
    }
}

static void finish_level(tb_app *app)
{
    const tb_player *p = &app->sim.players[0];
    int i = app->level_day - 1;
    uint32_t ms = (uint32_t)((app->sim.tick - app->level_start) * 1000u / TB_SIM_HZ);
    bool first = !(app->save.completed & (1u << i));
    app->save.completed |= 1u << i;
    app->save.floppies[i] |= p->floppies;
    if (app->sim.challenge_done) {
        app->save.challenges |= 1u << i;
    }
    if (app->save.best_time_ms[i] == 0 || ms < app->save.best_time_ms[i]) {
        app->save.best_time_ms[i] = ms;
    }
    app->save_dirty = true;
    char summary[128];
    tb_sim_results_text(&app->sim, 0, summary, sizeof(summary));
    const tb_level_info *info = tb_level_info_get(app->level_day);
    snprintf(app->results, sizeof(app->results), "DAY %d - %s\n%s\nTime %u:%02u%s", app->level_day,
             info ? info->title : "?", summary, ms / 60000u, (ms / 1000u) % 60u, first ? "" : " (replay)");
    tb_log_info("level %d complete in %u ms: floppies %d/3, bauble %s, challenge %s", app->level_day, ms,
                (p->floppies & 1) + ((p->floppies >> 1) & 1) + ((p->floppies >> 2) & 1),
                app->sim.bauble_collected ? "yes" : "no", app->sim.challenge_done ? "won" : "no");
    app->flow = TB_FLOW_RESULTS;
    app->flow_ticks = 0;
}

static void return_to_hall(tb_app *app)
{
    const tb_hall_door *d = &app->hall.doors[app->level_day - 1];
    load_hall(app, d->spawn, d->spawn_yaw);
    char line[96];
    snprintf(line, sizeof(line), "Bauble %d hangs on the tree. %d of 24.", app->level_day,
             tb_hall_tree_baubles(app->save.completed));
    tb_app_say(app, line);
}

static void tick(tb_app *app)
{
    uint16_t buttons = app->input[0].buttons;
    bool confirm = (buttons & CONFIRM_BUTTONS) && !(app->prev_buttons & CONFIRM_BUTTONS);
    app->prev_buttons = buttons;
    app->flow_ticks++;
    if (app->message_ticks > 0) {
        app->message_ticks--;
    }
    switch (app->flow) {
    case TB_FLOW_HALL: {
        tb_sim_step(&app->sim, app->input);
        int day = app->sim.players[0].portal;
        if (day && tb_portal_is_open(app->open, day)) {
            app->level_day = day;
            app->flow = TB_FLOW_BRIEFING;
            app->flow_ticks = 0;
        }
        break;
    }
    case TB_FLOW_BRIEFING:
        if (confirm || app->flow_ticks >= TB_BRIEFING_TICKS) {
            load_level(app, app->level_day);
        }
        break;
    case TB_FLOW_LEVEL:
        level_tick(app);
        if (app->sim.level_complete) {
            finish_level(app);
        }
        break;
    default: /* TB_FLOW_RESULTS */
        if (confirm || app->flow_ticks >= TB_RESULTS_TICKS) {
            return_to_hall(app);
        }
        break;
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
        tick(app);
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
