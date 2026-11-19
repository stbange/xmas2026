/* SPDX-License-Identifier: MIT */
#include "sim/weapons.h"

#include <string.h>

/* 60 Hz ticks. */
static const tb_weapon_def g_defs[TB_WEAPON_COUNT] = {
    [TB_WEAPON_NONE] = {"none", TB_WK_MELEE, TB_AMMO_NONE, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, false},
    [TB_WEAPON_CANDY_CANE] = {"Candy Cane", TB_WK_MELEE, TB_AMMO_NONE, 25, 1, 0.0f, 1.5f, 24, 0.0f, 0.0f, 0,
                              0, 0, 0, false},
    [TB_WEAPON_SNOWBALL_BLASTER] = {"Snowball Blaster", TB_WK_PROJECTILE, TB_AMMO_NONE, 10, 1, 0.0f, 0.0f, 12,
                                    30.0f, 0.0f, 0, 60, 40, 90, false},
    [TB_WEAPON_TEST_SHOTGUN] = {"Pump Shotgun", TB_WK_HITSCAN, TB_AMMO_SHELLS, 8, 7, 0.08f, 60.0f, 50, 0.0f,
                                0.0f, 0, 0, 0, 0, true},
    [TB_WEAPON_TEST_LAUNCHER] = {"Present Launcher", TB_WK_PROJECTILE, TB_AMMO_ROCKETS, 60, 1, 0.0f, 0.0f, 48,
                                 18.0f, 3.0f, 50, 0, 0, 0, true},
};

const tb_weapon_def *tb_weapon_get(tb_weapon_id id)
{
    return (id > TB_WEAPON_NONE && id < TB_WEAPON_COUNT) ? &g_defs[id] : NULL;
}

static bool take_ammo(const tb_weapon_def *def, int32_t *ammo)
{
    if (def->ammo == TB_AMMO_NONE || !ammo) {
        return true;
    }
    if (*ammo <= 0) {
        return false;
    }
    (*ammo)--;
    return true;
}

tb_fire_event tb_weapon_tick(tb_weapon_state *ws, const tb_weapon_def *def, bool trigger_down, int32_t *ammo)
{
    tb_fire_event ev;
    memset(&ev, 0, sizeof(ev));
    bool released = ws->trigger_was_down && !trigger_down;
    ws->trigger_was_down = trigger_down;
    if (ws->cooldown > 0) {
        ws->cooldown--;
    }

    if (def->charge_ticks > 0) {
        /* Hold to charge, fire on release. A tap fires an uncharged shot. */
        if (trigger_down && ws->cooldown == 0) {
            if (ws->charge < def->charge_ticks) {
                ws->charge++;
            }
            return ev;
        }
        if (!released || ws->charge == 0) {
            ws->charge = 0;
            return ev;
        }
        if (!take_ammo(def, ammo)) {
            ws->charge = 0;
            return ev;
        }
        float frac = (float)ws->charge / (float)def->charge_ticks;
        bool full = ws->charge >= def->charge_ticks;
        ev.fired = true;
        ev.charge_fraction = frac;
        ev.damage = full ? def->charged_damage : def->damage;
        ev.stun_ticks = full ? def->charged_stun_ticks : 0;
        ws->charge = 0;
        ws->cooldown = def->refire_ticks;
        return ev;
    }

    if (!trigger_down || ws->cooldown > 0 || !take_ammo(def, ammo)) {
        return ev;
    }
    ev.fired = true;
    ev.charge_fraction = 1.0f;
    ev.damage = def->damage;
    ws->cooldown = def->refire_ticks;
    return ev;
}
