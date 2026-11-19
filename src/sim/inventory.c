/* SPDX-License-Identifier: MIT */
#include "sim/inventory.h"

#include <string.h>

#define AMMO_MAX 200

static bool is_base_weapon(tb_weapon_id w)
{
    return w == TB_WEAPON_CANDY_CANE || w == TB_WEAPON_SNOWBALL_BLASTER;
}

void tb_inventory_init(tb_inventory *inv, bool body_replaced)
{
    memset(inv, 0, sizeof(*inv));
    inv->body_replaced = body_replaced;
    inv->slots[0] = TB_WEAPON_CANDY_CANE;
    inv->slots[1] = TB_WEAPON_SNOWBALL_BLASTER;
    inv->items[TB_ITEM_NOEL_PAD] = true;
    inv->current = 1;
}

bool tb_inventory_has_weapon(const tb_inventory *inv, tb_weapon_id w)
{
    for (int i = 0; i < TB_WEAPON_SLOTS; i++) {
        if (inv->slots[i] == w && w != TB_WEAPON_NONE) {
            return !(inv->body_replaced && is_base_weapon(w));
        }
    }
    return false;
}

bool tb_inventory_give_weapon(tb_inventory *inv, tb_weapon_id w)
{
    const tb_weapon_def *def = tb_weapon_get(w);
    if (!def) {
        return false;
    }
    for (int i = 0; i < TB_WEAPON_SLOTS; i++) {
        if (inv->slots[i] == w) {
            return true; /* already carried */
        }
    }
    for (int i = 2; i < TB_WEAPON_SLOTS; i++) {
        if (inv->slots[i] == TB_WEAPON_NONE) {
            inv->slots[i] = w;
            inv->loaner_slot[i] = def->loaner;
            return true;
        }
    }
    return false;
}

void tb_inventory_give_item(tb_inventory *inv, tb_item_id item, bool loaner)
{
    if (item > TB_ITEM_NONE && item < TB_ITEM_COUNT) {
        inv->items[item] = true;
        inv->loaner_item[item] = loaner;
    }
}

void tb_inventory_add_ammo(tb_inventory *inv, tb_ammo_type type, int32_t amount)
{
    if (type <= TB_AMMO_NONE || type >= TB_AMMO_COUNT) {
        return;
    }
    int32_t v = inv->ammo[type] + amount;
    inv->ammo[type] = v > AMMO_MAX ? AMMO_MAX : (v < 0 ? 0 : v);
}

bool tb_inventory_select(tb_inventory *inv, int slot)
{
    if (slot < 1 || slot > TB_WEAPON_SLOTS) {
        return false;
    }
    tb_weapon_id w = inv->slots[slot - 1];
    if (!tb_inventory_has_weapon(inv, w)) {
        return false;
    }
    inv->current = slot - 1;
    return true;
}

tb_weapon_id tb_inventory_current(const tb_inventory *inv)
{
    tb_weapon_id w = inv->slots[inv->current];
    return tb_inventory_has_weapon(inv, w) ? w : TB_WEAPON_NONE;
}

void tb_inventory_leave_level(tb_inventory *inv)
{
    for (int i = 0; i < TB_WEAPON_SLOTS; i++) {
        if (inv->loaner_slot[i]) {
            inv->slots[i] = TB_WEAPON_NONE;
            inv->loaner_slot[i] = false;
        }
    }
    for (int i = 0; i < TB_ITEM_COUNT; i++) {
        if (inv->loaner_item[i]) {
            inv->items[i] = false;
            inv->loaner_item[i] = false;
        }
    }
    memset(inv->ammo, 0, sizeof(inv->ammo));
    inv->body_replaced = false;
    if (inv->slots[inv->current] == TB_WEAPON_NONE) {
        inv->current = 1;
    }
}
