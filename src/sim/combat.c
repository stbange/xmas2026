/* SPDX-License-Identifier: MIT */
#include "sim/combat.h"

int32_t tb_difficulty_scale_damage(tb_difficulty d, int32_t amount)
{
    /* Percentages keep the maths integral and deterministic. */
    static const int32_t pct[] = {50, 100, 150, 200};
    int idx = (d >= TB_DIFF_PIZZA && d <= TB_DIFF_NIGHTMARE) ? (int)d : (int)TB_DIFF_COLA;
    return (amount * pct[idx] + 50) / 100;
}

const char *tb_difficulty_name(tb_difficulty d)
{
    switch (d) {
    case TB_DIFF_PIZZA:
        return "Pizza";
    case TB_DIFF_COLA:
        return "Cola";
    case TB_DIFF_COFFEE:
        return "Coffee at 4 AM";
    case TB_DIFF_NIGHTMARE:
        return "Nightmare LAN";
    default:
        return "?";
    }
}

void tb_vitals_init(tb_vitals *v)
{
    v->health = TB_HEALTH_START;
    v->armour = 0;
    v->drain_ticks = 0;
}

int32_t tb_vitals_damage(tb_vitals *v, int32_t amount)
{
    if (amount <= 0) {
        return 0;
    }
    int32_t absorbed = amount / 2;
    if (absorbed > v->armour) {
        absorbed = v->armour;
    }
    v->armour -= absorbed;
    int32_t lost = amount - absorbed;
    if (lost > v->health) {
        lost = v->health;
    }
    v->health -= lost;
    return lost;
}

int32_t tb_vitals_heal(tb_vitals *v, int32_t amount, int32_t limit)
{
    if (amount <= 0 || v->health >= limit || v->health <= 0) {
        return 0;
    }
    int32_t healed = v->health + amount > limit ? limit - v->health : amount;
    v->health += healed;
    return healed;
}

int32_t tb_vitals_add_armour(tb_vitals *v, int32_t amount)
{
    if (amount <= 0 || v->armour >= TB_ARMOUR_MAX) {
        return 0;
    }
    int32_t added = v->armour + amount > TB_ARMOUR_MAX ? TB_ARMOUR_MAX - v->armour : amount;
    v->armour += added;
    return added;
}

void tb_vitals_tick(tb_vitals *v, uint32_t tick_hz)
{
    if (v->health <= TB_HEALTH_MAX) {
        v->drain_ticks = 0;
        return;
    }
    if (++v->drain_ticks >= (int32_t)tick_hz) {
        v->drain_ticks = 0;
        v->health--;
    }
}

bool tb_vitals_dead(const tb_vitals *v)
{
    return v->health <= 0;
}
