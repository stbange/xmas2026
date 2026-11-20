/* SPDX-License-Identifier: MIT */
#include "game/level.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- the placeholder course (test arena) ------------------------------- */

static bool placeholder_geometry(tb_geometry *g)
{
    tb_brush *storage = g->brushes;
    int cap = g->cap;
    tb_geometry arena;
    tb_geometry_build_test_arena(&arena, storage);
    *g = arena;
    g->cap = cap;
    return true;
}

static void placeholder_populate(tb_sim *s)
{
    tb_sim_add_pickup(s, TB_PICKUP_BAUBLE, tb_v3(0, 18, 3.5f), 0, 0, false);
    tb_sim_add_pickup(s, TB_PICKUP_FLOPPY, tb_v3(6.5f, 0, 0.9f), 0, 0, false);
    tb_sim_add_pickup(s, TB_PICKUP_FLOPPY, tb_v3(-10, -10, 0.5f), 1, 0, false);
    tb_sim_add_pickup(s, TB_PICKUP_FLOPPY, tb_v3(11, 0, 1.5f), 2, 0, false);
    tb_sim_add_pickup(s, TB_PICKUP_WEAPON, tb_v3(-3, -12, 0.5f), TB_WEAPON_TEST_SHOTGUN, 16, false);
    for (int i = 0; i < 4; i++) {
        tb_sim_add_target(s, tb_v3(-6.0f + 4.0f * (float)i, 4.0f, 0.9f), 40);
    }
    tb_sim_add_trigger(s, TB_TRIGGER_CHECKPOINT, tb_v3(-4, 8, 0), tb_v3(4, 10, 3), 1);
    tb_sim_add_trigger(s, TB_TRIGGER_EXIT, tb_v3(-3, -20, 0), tb_v3(3, -17.5f, 3), 0);
}

static const tb_level_light k_placeholder_lights[] = {
    {{-10, 0, 4}, {2.0f, 0.3f, 0.2f}, 14.0f, 0},
    {{10, 10, 4}, {0.3f, 1.8f, 0.4f}, 14.0f, 0},
    {{0, -12, 3}, {1.8f, 1.4f, 0.4f}, 12.0f, 0},
};

static tb_level_def g_placeholders[24];

static const tb_level_def *placeholder(int day)
{
    tb_level_def *d = &g_placeholders[day - 1];
    if (d->manifest.day == 0) {
        d->manifest.day = day;
        d->placeholder = true;
        d->build_geometry = placeholder_geometry;
        d->populate = placeholder_populate;
        d->lights = k_placeholder_lights;
        d->light_count = (int)(sizeof(k_placeholder_lights) / sizeof(k_placeholder_lights[0]));
        d->spawn = tb_v3(0, -16, 0.01f);
        d->spawn_yaw = TB_PI / 2.0f;
        const uint8_t f[4] = {120, 110, 100, 255}, w[4] = {150, 140, 170, 255}, c[4] = {70, 70, 90, 255};
        memcpy(d->floor, f, 4);
        memcpy(d->wall, w, 4);
        memcpy(d->ceiling, c, 4);
        d->ambient = tb_v3(0.20f, 0.22f, 0.32f);
        d->sun_color = tb_v3(0.55f, 0.6f, 0.85f);
        d->fog_color = tb_v3(0.04f, 0.05f, 0.10f);
        d->fog_start = 12.0f;
        d->fog_end = 45.0f;
        d->objective = "FIND THE MEMORY BAUBLE, THEN THE EXIT";
    }
    return d;
}

const tb_level_def *tb_level_def_get(int day)
{
    if (day < 1 || day > 24) {
        return NULL;
    }
    if (day == 1) {
        return tb_level_day01();
    }
    return placeholder(day);
}

/* ---- manifests ----------------------------------------------------------- */

bool tb_level_manifest_validate(const tb_level_manifest *m, char *err, size_t err_len)
{
    const char *field = NULL;
    if (m->day < 1 || m->day > 24) {
        field = "day must be 1-24";
    } else if (m->mechanic_count < 2 || m->mechanic_count > 3) {
        field = "mechanics: needs 2 or 3 signature mechanics";
    } else if (!m->trophy || !m->trophy[0]) {
        field = "trophy is missing";
    } else if (m->loaner_count < 1) {
        field = "loaners: at least one loaner weapon or item";
    } else if (m->bar_count > 2) {
        field = "bars: at most 2 extra resource bars";
    } else if (!m->palette || !m->music) {
        field = "palette and music are required";
    } else if (m->minutes < 10 || m->minutes > 30) {
        field = "minutes: expected play time out of range";
    }
    if (field) {
        snprintf(err, err_len, "level %d: %s", m->day, field);
        return false;
    }
    return true;
}

bool tb_level_build(const tb_level_def *def, tb_geometry *g, tb_brush *storage)
{
    tb_geometry_init(g, storage, TB_LEVEL_MAX_BRUSHES);
    return def->build_geometry(g);
}

void tb_level_sim_config(const tb_level_def *def, tb_geometry *g, uint64_t seed, tb_difficulty difficulty,
                         tb_sim_config *out)
{
    memset(out, 0, sizeof(*out));
    out->geometry = g;
    out->seed = seed;
    out->difficulty = difficulty;
    out->spawn_yaw = def->spawn_yaw;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        /* Line up sideways to the view direction. */
        float side = 1.2f * (float)i;
        out->spawn[i] =
            tb_v3_add(def->spawn, tb_v3(-sinf(def->spawn_yaw) * side, cosf(def->spawn_yaw) * side, 0));
    }
}

tb_script *tb_level_begin(const tb_level_def *def, tb_sim *sim, char *err, size_t err_len)
{
    err[0] = '\0';
    def->populate(sim);
    if (!def->script) {
        return NULL;
    }
    char name[32];
    snprintf(name, sizeof(name), "=level%02d.lua", def->manifest.day);
    tb_script *script = tb_script_create(def->script, def->script_len, name, sim, err, err_len);
    if (script && !tb_script_start(script)) {
        snprintf(err, err_len, "%s", tb_script_error(script));
    }
    return script;
}

typedef struct golden_run {
    tb_brush brushes[TB_LEVEL_MAX_BRUSHES];
    tb_geometry geometry;
    tb_sim sim;
    tb_bot bot;
} golden_run;

bool tb_level_run_golden(const tb_level_def *def, uint64_t seed, tb_difficulty difficulty,
                         tb_golden_result *out)
{
    memset(out, 0, sizeof(*out));
    golden_run *r = calloc(1, sizeof(*r));
    if (!r) {
        snprintf(out->reason, sizeof(out->reason), "out of memory");
        return false;
    }
    if (!tb_level_build(def, &r->geometry, r->brushes)) {
        snprintf(out->reason, sizeof(out->reason), "geometry does not fit");
        free(r);
        return false;
    }
    tb_sim_config cfg;
    tb_level_sim_config(def, &r->geometry, seed, difficulty, &cfg);
    tb_sim_init(&r->sim, &cfg);
    tb_sim_add_player(&r->sim, 0);
    char err[128];
    tb_script *script = tb_level_begin(def, &r->sim, err, sizeof(err));
    if (err[0]) {
        snprintf(out->reason, sizeof(out->reason), "script: %.100s", err);
        tb_script_destroy(script);
        free(r);
        return false;
    }
    tb_bot_init(&r->bot, def->golden_path, def->golden_count);
    tb_input inputs[TB_MAX_PLAYERS];
    memset(inputs, 0, sizeof(inputs));
    while (!r->sim.level_complete && !r->bot.failed && out->ticks < def->golden_ticks) {
        inputs[0] = tb_bot_think(&r->bot, &r->sim, 0);
        tb_sim_step(&r->sim, inputs);
        if (script) {
            tb_script_tick(script);
        }
        out->ticks++;
    }
    const tb_player *p = &r->sim.players[0];
    out->completed = r->sim.level_complete;
    out->step = r->bot.index;
    out->floppies = p->floppies;
    out->bauble = r->sim.bauble_collected;
    out->challenge = r->sim.challenge_done;
    out->deaths = p->deaths;
    out->checksum = tb_sim_checksum(&r->sim);
    if (!out->completed) {
        snprintf(out->reason, sizeof(out->reason), "%s at (%.1f %.1f %.1f) after %u ticks%s%s",
                 r->bot.failed ? r->bot.reason : "out of time", (double)p->move.origin.x,
                 (double)p->move.origin.y, (double)p->move.origin.z, out->ticks,
                 script && tb_script_failed(script) ? "; script: " : "",
                 script && tb_script_failed(script) ? tb_script_error(script) : "");
    }
    tb_script_destroy(script);
    free(r);
    return out->completed;
}
