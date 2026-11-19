/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/inventory.h"

void setUp(void) {}
void tearDown(void) {}

/* @scenario player-core: Base kit in a new level */
static void test_base_kit_in_new_level(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, false);
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_CANDY_CANE, inv.slots[0]);
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_SNOWBALL_BLASTER, inv.slots[1]);
    TEST_ASSERT_TRUE(inv.items[TB_ITEM_NOEL_PAD]);
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_SNOWBALL_BLASTER, tb_inventory_current(&inv));
}

/* @scenario player-core: Leaving with a loaner */
static void test_loaners_removed_on_leaving(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, false);
    TEST_ASSERT_TRUE(tb_inventory_give_weapon(&inv, TB_WEAPON_TEST_SHOTGUN));
    tb_inventory_give_item(&inv, TB_ITEM_JETPACK, true);
    tb_inventory_add_ammo(&inv, TB_AMMO_SHELLS, 12);
    TEST_ASSERT_TRUE(tb_inventory_select(&inv, 3));
    tb_inventory_leave_level(&inv);
    TEST_ASSERT_FALSE(tb_inventory_has_weapon(&inv, TB_WEAPON_TEST_SHOTGUN));
    TEST_ASSERT_FALSE(inv.items[TB_ITEM_JETPACK]);
    TEST_ASSERT_TRUE(inv.items[TB_ITEM_NOEL_PAD]);
    TEST_ASSERT_EQUAL_INT32(0, inv.ammo[TB_AMMO_SHELLS]);
    TEST_ASSERT_TRUE(tb_inventory_has_weapon(&inv, TB_WEAPON_CANDY_CANE));
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_SNOWBALL_BLASTER, tb_inventory_current(&inv));
}

static void test_body_replaced_hides_base_kit(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, true);
    TEST_ASSERT_FALSE(tb_inventory_has_weapon(&inv, TB_WEAPON_CANDY_CANE));
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_NONE, tb_inventory_current(&inv));
    TEST_ASSERT_FALSE(tb_inventory_select(&inv, 1));
    tb_inventory_give_weapon(&inv, TB_WEAPON_TEST_LAUNCHER);
    TEST_ASSERT_TRUE(tb_inventory_select(&inv, 3));
    tb_inventory_leave_level(&inv); /* back on foot in the hall */
    TEST_ASSERT_TRUE(tb_inventory_has_weapon(&inv, TB_WEAPON_CANDY_CANE));
}

static void test_give_and_select_edge_cases(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, false);
    TEST_ASSERT_FALSE(tb_inventory_give_weapon(&inv, TB_WEAPON_NONE));
    TEST_ASSERT_TRUE(tb_inventory_give_weapon(&inv, TB_WEAPON_CANDY_CANE)); /* already carried */
    for (int i = 2; i < TB_WEAPON_SLOTS; i++) {
        inv.slots[i] = TB_WEAPON_TEST_LAUNCHER; /* fill slots */
    }
    TEST_ASSERT_FALSE(tb_inventory_give_weapon(&inv, TB_WEAPON_TEST_SHOTGUN));
    TEST_ASSERT_FALSE(tb_inventory_select(&inv, 0));
    TEST_ASSERT_FALSE(tb_inventory_select(&inv, 10));
    tb_inventory_init(&inv, false);
    TEST_ASSERT_FALSE(tb_inventory_select(&inv, 5)); /* empty slot */
    TEST_ASSERT_TRUE(tb_inventory_select(&inv, 1));
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_CANDY_CANE, tb_inventory_current(&inv));
    TEST_ASSERT_FALSE(tb_inventory_has_weapon(&inv, TB_WEAPON_NONE));
}

static void test_items_and_ammo_limits(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, false);
    tb_inventory_give_item(&inv, TB_ITEM_NONE, false);
    tb_inventory_give_item(&inv, TB_ITEM_COUNT, false);
    tb_inventory_give_item(&inv, TB_ITEM_JETPACK, false);
    tb_inventory_leave_level(&inv);
    TEST_ASSERT_TRUE(inv.items[TB_ITEM_JETPACK]); /* not a loaner: kept */
    tb_inventory_add_ammo(&inv, TB_AMMO_BULLETS, 500);
    TEST_ASSERT_EQUAL_INT32(200, inv.ammo[TB_AMMO_BULLETS]);
    tb_inventory_add_ammo(&inv, TB_AMMO_BULLETS, -900);
    TEST_ASSERT_EQUAL_INT32(0, inv.ammo[TB_AMMO_BULLETS]);
    tb_inventory_add_ammo(&inv, TB_AMMO_NONE, 5);
    tb_inventory_add_ammo(&inv, TB_AMMO_COUNT, 5);
    TEST_ASSERT_EQUAL_INT32(0, inv.ammo[TB_AMMO_NONE]);
}

static void test_leaving_keeps_current_base_weapon(void)
{
    tb_inventory inv;
    tb_inventory_init(&inv, false);
    tb_inventory_select(&inv, 1);
    tb_inventory_give_weapon(&inv, TB_WEAPON_TEST_SHOTGUN);
    tb_inventory_leave_level(&inv);
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_CANDY_CANE, tb_inventory_current(&inv));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_base_kit_in_new_level);
    RUN_TEST(test_loaners_removed_on_leaving);
    RUN_TEST(test_body_replaced_hides_base_kit);
    RUN_TEST(test_give_and_select_edge_cases);
    RUN_TEST(test_items_and_ammo_limits);
    RUN_TEST(test_leaving_keeps_current_base_weapon);
    return UNITY_END();
}
