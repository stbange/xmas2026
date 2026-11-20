/* SPDX-License-Identifier: MIT */
#include "sim/weapons.h"

#include <string.h>

/* 60 Hz ticks. */
static const tb_weapon_def g_defs[TB_WEAPON_COUNT] = {
    [TB_WEAPON_NONE] = {.name = "none", .kind = TB_WK_MELEE, .refire_ticks = 1},
    [TB_WEAPON_CANDY_CANE] = {.name = "Candy Cane",
                              .kind = TB_WK_MELEE,
                              .damage = 25,
                              .pellets = 1,
                              .range = 1.5f,
                              .refire_ticks = 24},
    [TB_WEAPON_SNOWBALL_BLASTER] = {.name = "Snowball Blaster",
                                    .kind = TB_WK_PROJECTILE,
                                    .damage = 10,
                                    .pellets = 1,
                                    .refire_ticks = 12,
                                    .projectile_speed = 30.0f,
                                    .charge_ticks = 60,
                                    .charged_damage = 40,
                                    .charged_stun_ticks = 90},
    [TB_WEAPON_TEST_SHOTGUN] = {.name = "Pump Shotgun",
                                .kind = TB_WK_HITSCAN,
                                .ammo = TB_AMMO_SHELLS,
                                .damage = 8,
                                .pellets = 7,
                                .spread = 0.08f,
                                .range = 60.0f,
                                .refire_ticks = 50,
                                .loaner = true},
    [TB_WEAPON_TEST_LAUNCHER] = {.name = "Present Launcher",
                                 .kind = TB_WK_PROJECTILE,
                                 .ammo = TB_AMMO_ROCKETS,
                                 .damage = 60,
                                 .pellets = 1,
                                 .refire_ticks = 48,
                                 .projectile_speed = 18.0f,
                                 .splash_radius = 3.0f,
                                 .splash_damage = 50,
                                 .loaner = true},
    [TB_WEAPON_MIGHTY_BOOT] = {.name = "Mighty Boot",
                               .kind = TB_WK_MELEE,
                               .damage = 30,
                               .pellets = 1,
                               .range = 1.7f,
                               .refire_ticks = 18,
                               .loaner = true},
    [TB_WEAPON_PIPE_BAUBLES] = {.name = "Pipe Baubles",
                                .kind = TB_WK_PROJECTILE,
                                .ammo = TB_AMMO_PIPES,
                                .pellets = 1,
                                .refire_ticks = 30,
                                .projectile_speed = 13.0f,
                                .splash_radius = 3.5f,
                                .splash_damage = 120,
                                .loaner = true,
                                .gravity = true,
                                .bounce = true,
                                .remote = true},
    [TB_WEAPON_GIFT_SHRINKER] = {.name = "Gift-Shrinker",
                                 .kind = TB_WK_HITSCAN,
                                 .ammo = TB_AMMO_CELLS,
                                 .pellets = 1,
                                 .range = 45.0f,
                                 .refire_ticks = 36,
                                 .loaner = true,
                                 .shrink_ticks = 360},
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
