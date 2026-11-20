/* SPDX-License-Identifier: MIT */
/* Weapons, ammo and items a player carries. The base kit (Candy Cane,
 * Snowball Blaster, NOEL-Pad) is always there; loaners only exist in their
 * level and are removed when the player leaves it. */
#ifndef TB_SIM_INVENTORY_H
#define TB_SIM_INVENTORY_H

#include "sim/weapons.h"

#define TB_WEAPON_SLOTS 9

typedef enum tb_item_id {
    TB_ITEM_NONE = 0,
    TB_ITEM_NOEL_PAD,   /* base kit: map, objectives, hints */
    TB_ITEM_JETPACK,    /* day 1: fuel-limited jetpack (ability 1) */
    TB_ITEM_HOLO_SANTA, /* day 1: decoy hologram (ability 2) */
    TB_ITEM_COUNT
} tb_item_id;

typedef struct tb_inventory {
    tb_weapon_id slots[TB_WEAPON_SLOTS]; /* slot 1..9 -> index 0..8 */
    bool loaner_slot[TB_WEAPON_SLOTS];
    int32_t ammo[TB_AMMO_COUNT];
    bool items[TB_ITEM_COUNT];
    bool loaner_item[TB_ITEM_COUNT];
    int current;        /* selected slot index */
    bool body_replaced; /* level puts the player in a mech/ship: base kit hidden */
} tb_inventory;

/* Starting inventory for a level: just the base kit. */
void tb_inventory_init(tb_inventory *inv, bool body_replaced);
/* Gives a weapon (into its own slot); loaner weapons are flagged as such.
 * Returns false if the weapon is invalid or no slot is free. */
bool tb_inventory_give_weapon(tb_inventory *inv, tb_weapon_id w);
bool tb_inventory_has_weapon(const tb_inventory *inv, tb_weapon_id w);
void tb_inventory_give_item(tb_inventory *inv, tb_item_id item, bool loaner);
void tb_inventory_add_ammo(tb_inventory *inv, tb_ammo_type type, int32_t amount);
/* Selects slot 1..9 if it holds a usable weapon. */
bool tb_inventory_select(tb_inventory *inv, int slot);
/* The weapon in hand (base kit hidden if body_replaced). */
tb_weapon_id tb_inventory_current(const tb_inventory *inv);
/* Removes all loaners (called when leaving a level). */
void tb_inventory_leave_level(tb_inventory *inv);

#endif
