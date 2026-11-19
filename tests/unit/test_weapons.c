/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/weapons.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_definitions(void)
{
    TEST_ASSERT_NULL(tb_weapon_get(TB_WEAPON_NONE));
    TEST_ASSERT_NULL(tb_weapon_get(TB_WEAPON_COUNT));
    const tb_weapon_def *cane = tb_weapon_get(TB_WEAPON_CANDY_CANE);
    TEST_ASSERT_EQUAL_INT(TB_WK_MELEE, cane->kind);
    TEST_ASSERT_FALSE(cane->loaner);
    const tb_weapon_def *blaster = tb_weapon_get(TB_WEAPON_SNOWBALL_BLASTER);
    TEST_ASSERT_EQUAL_INT(TB_AMMO_NONE, blaster->ammo); /* infinite */
    TEST_ASSERT_TRUE(blaster->charge_ticks > 0);
    TEST_ASSERT_TRUE(tb_weapon_get(TB_WEAPON_TEST_SHOTGUN)->loaner);
}

static void test_refire_rate_and_ammo(void)
{
    const tb_weapon_def *def = tb_weapon_get(TB_WEAPON_TEST_SHOTGUN);
    tb_weapon_state ws;
    memset(&ws, 0, sizeof(ws));
    int32_t ammo = 2;
    int shots = 0;
    for (int t = 0; t < 300; t++) {
        shots += tb_weapon_tick(&ws, def, true, &ammo).fired;
    }
    TEST_ASSERT_EQUAL_INT(2, shots); /* out of ammo after two */
    TEST_ASSERT_EQUAL_INT32(0, ammo);

    memset(&ws, 0, sizeof(ws));
    ammo = 100;
    shots = 0;
    for (int t = 0; t < 120; t++) {
        shots += tb_weapon_tick(&ws, def, true, &ammo).fired;
    }
    TEST_ASSERT_EQUAL_INT(3, shots); /* 50-tick refire: ticks 0, 50, 100 */
    TEST_ASSERT_FALSE(tb_weapon_tick(&ws, def, false, &ammo).fired);
}

static void test_charge_tap_and_full(void)
{
    const tb_weapon_def *def = tb_weapon_get(TB_WEAPON_SNOWBALL_BLASTER);
    tb_weapon_state ws;
    memset(&ws, 0, sizeof(ws));
    TEST_ASSERT_FALSE(tb_weapon_tick(&ws, def, true, NULL).fired); /* charging */
    tb_fire_event tap = tb_weapon_tick(&ws, def, false, NULL);
    TEST_ASSERT_TRUE(tap.fired);
    TEST_ASSERT_EQUAL_INT32(def->damage, tap.damage);
    TEST_ASSERT_EQUAL_UINT32(0, tap.stun_ticks);

    for (uint32_t t = 0; t < def->refire_ticks; t++) {
        tb_weapon_tick(&ws, def, false, NULL); /* cool down */
    }
    for (uint32_t t = 0; t < def->charge_ticks + 30; t++) {
        TEST_ASSERT_FALSE(tb_weapon_tick(&ws, def, true, NULL).fired);
    }
    tb_fire_event full = tb_weapon_tick(&ws, def, false, NULL);
    TEST_ASSERT_TRUE(full.fired);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, full.charge_fraction);
    TEST_ASSERT_EQUAL_INT32(def->charged_damage, full.damage);
    TEST_ASSERT_EQUAL_UINT32(def->charged_stun_ticks, full.stun_ticks);
}

static void test_charge_waits_for_cooldown(void)
{
    const tb_weapon_def *def = tb_weapon_get(TB_WEAPON_SNOWBALL_BLASTER);
    tb_weapon_state ws;
    memset(&ws, 0, sizeof(ws));
    tb_weapon_tick(&ws, def, true, NULL);
    tb_weapon_tick(&ws, def, false, NULL); /* fires, cooldown starts */
    tb_weapon_tick(&ws, def, true, NULL);  /* still cooling: no charge */
    TEST_ASSERT_EQUAL_UINT32(0, ws.charge);
    TEST_ASSERT_FALSE(tb_weapon_tick(&ws, def, false, NULL).fired); /* release without charge */
}

/* A charge weapon with ammo (none exist yet in the base set). */
static void test_charge_weapon_without_ammo(void)
{
    tb_weapon_def def = *tb_weapon_get(TB_WEAPON_SNOWBALL_BLASTER);
    def.ammo = TB_AMMO_CELLS;
    tb_weapon_state ws;
    memset(&ws, 0, sizeof(ws));
    int32_t ammo = 0;
    tb_weapon_tick(&ws, &def, true, &ammo);
    TEST_ASSERT_FALSE(tb_weapon_tick(&ws, &def, false, &ammo).fired);
    TEST_ASSERT_EQUAL_UINT32(0, ws.charge);
    /* Weapon with ammo type but no counter supplied: treated as infinite. */
    tb_weapon_tick(&ws, &def, true, NULL);
    TEST_ASSERT_TRUE(tb_weapon_tick(&ws, &def, false, NULL).fired);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_definitions);
    RUN_TEST(test_refire_rate_and_ammo);
    RUN_TEST(test_charge_tap_and_full);
    RUN_TEST(test_charge_waits_for_cooldown);
    RUN_TEST(test_charge_weapon_without_ammo);
    return UNITY_END();
}
