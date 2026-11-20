/* SPDX-License-Identifier: MIT */
/* The level framework: what every one of the 24 levels provides. Until the
 * map compiler exists (task group 6) levels are built in code; the
 * manifest, script, lights and golden path already follow the final
 * contract (openspec level-framework). */
#ifndef TB_GAME_LEVEL_H
#define TB_GAME_LEVEL_H

#include "game/bot.h"
#include "game/script.h"
#include "sim/sim.h"

#define TB_LEVEL_MAX_BRUSHES 512
#define TB_LEVEL_MAX_LIGHTS  24
#define TB_MANIFEST_MAX      8

typedef enum tb_comfort { TB_COMFORT_COMFORTABLE = 0, TB_COMFORT_MODERATE, TB_COMFORT_INTENSE } tb_comfort;

typedef struct tb_level_manifest {
    int day;
    const char *mechanics[TB_MANIFEST_MAX]; /* 2-3 signature mechanics */
    int mechanic_count;
    const char *loaners[TB_MANIFEST_MAX];
    int loaner_count;
    const char *trophy;
    const char *bars[TB_MANIFEST_MAX]; /* extra resource bars (at most 2) */
    int bar_count;
    const char *actions[TB_MANIFEST_MAX]; /* level input actions (touch/XR buttons) */
    int action_count;
    tb_comfort comfort;
    const char *palette;
    const char *music;
    int minutes; /* expected play time on Cola */
} tb_level_manifest;

typedef struct tb_level_light {
    tb_vec3 pos;
    tb_vec3 color;
    float radius;
    int group; /* light group switched by the simulation (0..31) */
} tb_level_light;

/* Decoration: a coloured box without collision (signs, machines, neon). */
typedef struct tb_level_prop {
    tb_vec3 mins, maxs;
    uint8_t color[4];
    bool emissive;
} tb_level_prop;

typedef struct tb_level_def {
    tb_level_manifest manifest;
    bool (*build_geometry)(tb_geometry *g);
    void (*populate)(tb_sim *sim);
    const tb_level_light *lights;
    const tb_level_prop *props;
    const char *script; /* Lua source (embedded at build time) */
    size_t script_len;
    const tb_bot_step *golden_path;
    const char *objective; /* first objective line */
    int light_count;
    int prop_count;
    int golden_count;
    uint32_t golden_ticks; /* the bot must finish within this many ticks */
    float spawn_yaw;
    float fog_start, fog_end;
    tb_vec3 spawn;
    tb_vec3 ambient, sun_color, fog_color;
    uint8_t floor[4], wall[4], ceiling[4];
    bool placeholder; /* the test course standing in for an unfinished level */
} tb_level_def;

/* The definition for day 1..24 (a placeholder course where the real level
 * is not built yet), or NULL. */
const tb_level_def *tb_level_def_get(int day);
/* Checks a manifest; on failure writes "level N: <field> ..." to err. */
bool tb_level_manifest_validate(const tb_level_manifest *m, char *err, size_t err_len);
/* Builds the geometry into caller storage (TB_LEVEL_MAX_BRUSHES) and
 * populates a freshly initialised simulation. */
bool tb_level_build(const tb_level_def *def, tb_geometry *g, tb_brush *storage);

/* Simulation config for the level: spawn points around def->spawn. */
void tb_level_sim_config(const tb_level_def *def, tb_geometry *g, uint64_t seed, tb_difficulty difficulty,
                         tb_sim_config *out);
/* Populates an initialised simulation (players already added), gives every
 * player the level's spawn state, then loads and starts the level script.
 * Returns the script (NULL if the level has none or it failed; the error
 * goes to err). */
tb_script *tb_level_begin(const tb_level_def *def, tb_sim *sim, char *err, size_t err_len);

typedef struct tb_golden_result {
    bool completed;
    uint32_t ticks;
    int step; /* bot step reached */
    char reason[128];
    uint8_t floppies;
    bool bauble;
    bool challenge;
    uint32_t deaths;
    uint64_t checksum;
} tb_golden_result;

/* Runs the level's golden path with the test bot, headless. */
bool tb_level_run_golden(const tb_level_def *def, uint64_t seed, tb_difficulty difficulty,
                         tb_golden_result *out);

/* Day 1 (src/game/levels/day01.c). */
const tb_level_def *tb_level_day01(void);

#endif
