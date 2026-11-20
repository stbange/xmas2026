/* SPDX-License-Identifier: MIT */
/* Level scripts: a sandboxed Lua 5.4 state per level (openspec
 * level-framework). Scripts react to simulation events and act on the
 * simulation only through the `tb` API, so a level stays deterministic
 * (replays, network, bots). No io, os, package or load*; math.random draws
 * from the simulation's RNG; each callback has an instruction budget and
 * the whole state a memory cap. */
#ifndef TB_GAME_SCRIPT_H
#define TB_GAME_SCRIPT_H

#include "sim/sim.h"

#define TB_SCRIPT_MEMORY_MAX  ((size_t)2 * 1024 * 1024)
#define TB_SCRIPT_INSTR_LIMIT 200000
#define TB_SCRIPT_TEXT        160

typedef struct tb_script tb_script;

/* Loads and runs the chunk (top-level definitions). On failure returns NULL
 * and writes the Lua error to err. */
tb_script *tb_script_create(const char *source, size_t len, const char *chunk_name, tb_sim *sim, char *err,
                            size_t err_len);
void tb_script_destroy(tb_script *s);
/* Calls on_start(). */
bool tb_script_start(tb_script *s);
/* Dispatches this tick's simulation events to on_trigger / on_used /
 * on_killed / on_pickup. Returns false once the script has failed. */
bool tb_script_tick(tb_script *s);
/* True once a callback raised an error; the script then stays silent. */
bool tb_script_failed(const tb_script *s);
const char *tb_script_error(const tb_script *s);
/* The last tb.say() line, if one was said since the previous call. */
const char *tb_script_take_say(tb_script *s);
/* The current objective (tb.objective), "" if none. */
const char *tb_script_objective(const tb_script *s);
/* Reads a global number (tests, debugging); `fallback` if absent. */
double tb_script_global_number(tb_script *s, const char *name, double fallback);

#endif
