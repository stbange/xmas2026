/* SPDX-License-Identifier: MIT */
#include "game/script.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/log.h"

struct tb_script {
    lua_State *L;
    tb_sim *sim;
    size_t memory;
    bool failed;
    bool say_new;
    char error[TB_SCRIPT_TEXT];
    char say[TB_SCRIPT_TEXT];
    char objective[TB_SCRIPT_TEXT];
};

/* ---- sandbox ------------------------------------------------------------- */

static void *script_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    tb_script *s = ud;
    size_t old = ptr ? osize : 0;
    if (nsize == 0) {
        free(ptr);
        s->memory -= old;
        return NULL;
    }
    if (s->memory - old + nsize > TB_SCRIPT_MEMORY_MAX) {
        return NULL; /* Lua raises "not enough memory" */
    }
    void *p = realloc(ptr, nsize);
    if (p) {
        s->memory = s->memory - old + nsize;
    }
    return p;
}

static void budget_hook(lua_State *L, lua_Debug *ar)
{
    (void)ar;
    luaL_error(L, "instruction budget exceeded");
}

static tb_script *self(lua_State *L)
{
    return *(tb_script **)lua_getextraspace(L);
}

/* ---- the tb API ---------------------------------------------------------- */

static int l_say(lua_State *L)
{
    tb_script *s = self(L);
    snprintf(s->say, sizeof(s->say), "%s", luaL_checkstring(L, 1));
    s->say_new = true;
    return 0;
}

static int l_objective(lua_State *L)
{
    tb_script *s = self(L);
    snprintf(s->objective, sizeof(s->objective), "%s", luaL_checkstring(L, 1));
    return 0;
}

static int l_fire(lua_State *L)
{
    lua_pushinteger(L, tb_sim_fire(self(L)->sim, luaL_checkstring(L, 1)));
    return 1;
}

static tb_vec3 check_vec(lua_State *L, int arg)
{
    return tb_v3((float)luaL_checknumber(L, arg), (float)luaL_checknumber(L, arg + 1),
                 (float)luaL_checknumber(L, arg + 2));
}

static int l_spawn_enemy(lua_State *L)
{
    static const char *const kinds[] = {"trooper", "lizard", "brain", "battlelord", NULL};
    int kind = luaL_checkoption(L, 1, NULL, kinds);
    tb_vec3 at = check_vec(L, 2);
    const char *name = luaL_optstring(L, 5, NULL);
    lua_pushboolean(L, tb_sim_add_enemy(self(L)->sim, (tb_enemy_kind)kind, at, name) != 0);
    return 1;
}

static int l_spawn_pickup(lua_State *L)
{
    static const char *const kinds[] = {"health", "armour", "weapon", "ammo",
                                        "item",   "bauble", "floppy", NULL};
    int kind = luaL_checkoption(L, 1, NULL, kinds);
    tb_vec3 at = check_vec(L, 2);
    int32_t value = (int32_t)luaL_optinteger(L, 5, 0);
    int32_t amount = (int32_t)luaL_optinteger(L, 6, 0);
    bool flag = lua_toboolean(L, 7);
    lua_pushboolean(L, tb_sim_add_pickup(self(L)->sim, (tb_pickup_kind)kind, at, value, amount, flag) != 0);
    return 1;
}

static int l_shake(lua_State *L)
{
    lua_Integer ticks = luaL_checkinteger(L, 1);
    tb_sim *sim = self(L)->sim;
    if (ticks > 0 && (uint32_t)ticks > sim->shake_ticks) {
        sim->shake_ticks = (uint32_t)ticks;
    }
    return 0;
}

static int l_challenge_done(lua_State *L)
{
    tb_sim_complete_challenge(self(L)->sim);
    return 0;
}

static int l_light(lua_State *L)
{
    lua_Integer group = luaL_checkinteger(L, 1);
    luaL_argcheck(L, group >= 0 && group < TB_LIGHT_GROUPS, 1, "light group out of range");
    tb_sim_set_light(self(L)->sim, (int)group, lua_toboolean(L, 2));
    return 0;
}

static int l_tick(lua_State *L)
{
    lua_pushinteger(L, (lua_Integer)self(L)->sim->tick);
    return 1;
}

static int l_enemies(lua_State *L)
{
    lua_pushinteger(L, tb_sim_enemy_count(self(L)->sim));
    return 1;
}

static int l_random(lua_State *L)
{
    tb_rng *rng = &self(L)->sim->rng;
    int n = lua_gettop(L);
    if (n == 0) {
        lua_pushnumber(L, (lua_Number)tb_rng_float01(rng));
        return 1;
    }
    lua_Integer lo = 1, hi;
    if (n == 1) {
        hi = luaL_checkinteger(L, 1);
    } else {
        lo = luaL_checkinteger(L, 1);
        hi = luaL_checkinteger(L, 2);
    }
    luaL_argcheck(L, lo <= hi && hi - lo < 0x7fffffff, n, "interval is empty or too large");
    lua_pushinteger(L, lo + tb_rng_range(rng, 0, (int32_t)(hi - lo)));
    return 1;
}

static int l_print(lua_State *L)
{
    tb_log_info("script: %s", luaL_tolstring(L, 1, NULL));
    return 0;
}

static const luaL_Reg k_tb_api[] = {
    {"say", l_say},
    {"objective", l_objective},
    {"fire", l_fire},
    {"spawn_enemy", l_spawn_enemy},
    {"spawn_pickup", l_spawn_pickup},
    {"shake", l_shake},
    {"challenge_done", l_challenge_done},
    {"light", l_light},
    {"tick", l_tick},
    {"enemies", l_enemies},
    {NULL, NULL},
};

static void open_sandbox(lua_State *L)
{
    luaL_requiref(L, LUA_GNAME, luaopen_base, 1);
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
    luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(L, 4);
    static const char *const removed[] = {"dofile", "loadfile", "load",    "collectgarbage", "require",
                                          "io",     "os",       "package", "debug"};
    for (size_t i = 0; i < sizeof(removed) / sizeof(removed[0]); i++) {
        lua_pushnil(L);
        lua_setglobal(L, removed[i]);
    }
    lua_pushcfunction(L, l_print);
    lua_setglobal(L, "print");
    lua_getglobal(L, LUA_MATHLIBNAME);
    lua_pushcfunction(L, l_random);
    lua_setfield(L, -2, "random");
    lua_pushnil(L);
    lua_setfield(L, -2, "randomseed");
    lua_pop(L, 1);
    luaL_newlib(L, k_tb_api);
    lua_setglobal(L, "tb");
    lua_newtable(L);
    lua_setglobal(L, "flags");
}

/* ---- calls --------------------------------------------------------------- */

static bool protected_call(tb_script *s, int nargs)
{
    lua_sethook(s->L, budget_hook, LUA_MASKCOUNT, TB_SCRIPT_INSTR_LIMIT);
    int rc = lua_pcall(s->L, nargs, 0, 0);
    lua_sethook(s->L, NULL, 0, 0);
    if (rc != LUA_OK) {
        const char *msg = lua_tostring(s->L, -1);
        snprintf(s->error, sizeof(s->error), "%s", msg ? msg : "error object is not a string");
        lua_pop(s->L, 1);
        s->failed = true;
        tb_log_error("level script: %s", s->error);
        return false;
    }
    return true;
}

tb_script *tb_script_create(const char *source, size_t len, const char *chunk_name, tb_sim *sim, char *err,
                            size_t err_len)
{
    tb_script *s = calloc(1, sizeof(*s));
    if (!s) {
        snprintf(err, err_len, "out of memory");
        return NULL;
    }
    s->sim = sim;
    s->L = lua_newstate(script_alloc, s);
    if (!s->L) {
        snprintf(err, err_len, "out of memory");
        free(s);
        return NULL;
    }
    *(tb_script **)lua_getextraspace(s->L) = s;
    open_sandbox(s->L);
    if (luaL_loadbufferx(s->L, source, len, chunk_name, "t") != LUA_OK || !protected_call(s, 0)) {
        snprintf(err, err_len, "%s", s->failed ? s->error : lua_tostring(s->L, -1));
        tb_script_destroy(s);
        return NULL;
    }
    return s;
}

void tb_script_destroy(tb_script *s)
{
    if (s) {
        lua_close(s->L);
        free(s);
    }
}

/* Pushes the global function `name`; false (nothing pushed) if absent. */
static bool push_callback(tb_script *s, const char *name)
{
    if (lua_getglobal(s->L, name) == LUA_TFUNCTION) {
        return true;
    }
    lua_pop(s->L, 1);
    return false;
}

bool tb_script_start(tb_script *s)
{
    if (s->failed) {
        return false;
    }
    if (push_callback(s, "on_start")) {
        return protected_call(s, 0);
    }
    return true;
}

bool tb_script_tick(tb_script *s)
{
    static const char *const callbacks[] = {"on_trigger", "on_used", "on_killed", "on_pickup"};
    for (int i = 0; i < s->sim->event_count && !s->failed; i++) {
        const tb_event *e = &s->sim->events[i];
        if (!push_callback(s, callbacks[e->type])) {
            continue;
        }
        lua_pushstring(s->L, e->name);
        lua_pushinteger(s->L, e->value);
        lua_pushinteger(s->L, e->player);
        protected_call(s, 3);
    }
    return !s->failed;
}

bool tb_script_failed(const tb_script *s)
{
    return s->failed;
}

const char *tb_script_error(const tb_script *s)
{
    return s->error;
}

const char *tb_script_take_say(tb_script *s)
{
    if (!s->say_new) {
        return NULL;
    }
    s->say_new = false;
    return s->say;
}

const char *tb_script_objective(const tb_script *s)
{
    return s->objective;
}

double tb_script_global_number(tb_script *s, const char *name, double fallback)
{
    lua_getglobal(s->L, name);
    int ok = 0;
    lua_Number v = lua_tonumberx(s->L, -1, &ok);
    lua_pop(s->L, 1);
    return ok ? (double)v : fallback;
}
