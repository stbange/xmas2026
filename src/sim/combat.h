/* SPDX-License-Identifier: MIT */
/* Health, armour, damage and difficulty. */
#ifndef TB_SIM_COMBAT_H
#define TB_SIM_COMBAT_H

#include "core/types.h"

typedef enum tb_difficulty {
    TB_DIFF_PIZZA = 0,    /* 50 % damage taken */
    TB_DIFF_COLA = 1,     /* 100 % (default) */
    TB_DIFF_COFFEE = 2,   /* 150 % */
    TB_DIFF_NIGHTMARE = 3 /* 200 %, unlocked after level 24 */
} tb_difficulty;

#define TB_HEALTH_START      100
#define TB_HEALTH_MAX        100
#define TB_HEALTH_OVERCHARGE 200
#define TB_ARMOUR_MAX        100

typedef struct tb_vitals {
    int32_t health;
    int32_t armour;
    /* Fractional overcharge drain accumulator (ticks). */
    int32_t drain_ticks;
} tb_vitals;

/* Damage a player takes for `amount` raw damage on this difficulty. */
int32_t tb_difficulty_scale_damage(tb_difficulty d, int32_t amount);
const char *tb_difficulty_name(tb_difficulty d);

void tb_vitals_init(tb_vitals *v);
/* Applies damage (already scaled): armour absorbs half, as far as it lasts.
 * Returns the health actually lost. Health never goes below 0. */
int32_t tb_vitals_damage(tb_vitals *v, int32_t amount);
/* Heals up to `limit` (TB_HEALTH_MAX for normal pickups, TB_HEALTH_OVERCHARGE
 * for overcharge pickups). Returns the amount healed. */
int32_t tb_vitals_heal(tb_vitals *v, int32_t amount, int32_t limit);
int32_t tb_vitals_add_armour(tb_vitals *v, int32_t amount);
/* Per tick: health above 100 drains by 1 per second. */
void tb_vitals_tick(tb_vitals *v, uint32_t tick_hz);
bool tb_vitals_dead(const tb_vitals *v);

#endif
