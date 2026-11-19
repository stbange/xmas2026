/* SPDX-License-Identifier: MIT */
/* Weapon definitions and the per-player firing state machine (fire rate,
 * ammo, charge-up). The simulation turns fire events into hitscan traces,
 * projectiles or melee sweeps. */
#ifndef TB_SIM_WEAPONS_H
#define TB_SIM_WEAPONS_H

#include "core/types.h"

typedef enum tb_weapon_kind { TB_WK_HITSCAN = 0, TB_WK_PROJECTILE = 1, TB_WK_MELEE = 2 } tb_weapon_kind;

typedef enum tb_ammo_type {
    TB_AMMO_NONE = 0, /* infinite */
    TB_AMMO_SHELLS,
    TB_AMMO_BULLETS,
    TB_AMMO_ROCKETS,
    TB_AMMO_CELLS,
    TB_AMMO_COUNT
} tb_ammo_type;

typedef enum tb_weapon_id {
    TB_WEAPON_NONE = 0,
    TB_WEAPON_CANDY_CANE,       /* base kit: melee */
    TB_WEAPON_SNOWBALL_BLASTER, /* base kit: infinite projectile, charge shot */
    TB_WEAPON_TEST_SHOTGUN,     /* test loaner: hitscan, pellets */
    TB_WEAPON_TEST_LAUNCHER,    /* test loaner: rockets with splash */
    TB_WEAPON_COUNT
} tb_weapon_id;

typedef struct tb_weapon_def {
    const char *name;
    tb_weapon_kind kind;
    tb_ammo_type ammo;
    int32_t damage;         /* per hit / pellet / direct impact */
    int32_t pellets;        /* hitscan pellets per shot */
    float spread;           /* radians (hitscan) */
    float range;            /* metres (hitscan, melee) */
    uint32_t refire_ticks;  /* ticks between shots */
    float projectile_speed; /* m/s */
    float splash_radius;    /* m, 0 = none */
    int32_t splash_damage;
    /* Charge: hold fire to charge; release fires. 0 = no charge. */
    uint32_t charge_ticks;  /* ticks to full charge */
    int32_t charged_damage; /* damage at full charge */
    uint32_t charged_stun_ticks;
    bool loaner; /* lives only in its level */
} tb_weapon_def;

const tb_weapon_def *tb_weapon_get(tb_weapon_id id);

typedef struct tb_weapon_state {
    uint32_t cooldown; /* ticks until the next shot */
    uint32_t charge;   /* ticks charged */
    bool trigger_was_down;
} tb_weapon_state;

typedef struct tb_fire_event {
    bool fired;
    float charge_fraction; /* 0..1 for charge weapons, 1 otherwise */
    int32_t damage;
    uint32_t stun_ticks;
} tb_fire_event;

/* Advances the weapon by one tick. `ammo` may be NULL for infinite weapons;
 * a shot consumes one unit. */
tb_fire_event tb_weapon_tick(tb_weapon_state *ws, const tb_weapon_def *def, bool trigger_down, int32_t *ammo);

#endif
