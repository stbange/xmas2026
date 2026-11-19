/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/hash.h"
#include "core/rng.h"
#include "sim/replay.h"
#include "support/test_alloc.h"

#include <stdlib.h>
#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;

static void make_sim(uint64_t seed)
{
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = seed;
    cfg.difficulty = TB_DIFF_COLA;
    cfg.spawn[0] = tb_v3(0, 0, 0.01f);
    cfg.spawn[1] = tb_v3(0, -3, 0.01f);
    tb_sim_init(&g_sim, &cfg);
    tb_sim_add_target(&g_sim, tb_v3(6, 4, 0.9f), 60);
    tb_sim_add_target(&g_sim, tb_v3(-8, 3, 0.9f), 60);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_WEAPON, tb_v3(3, 1, 0.5f), TB_WEAPON_TEST_SHOTGUN, 20, false);
}

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
}
void tearDown(void) {}

/* Records a pseudo-random but plausible play session of `ticks` ticks. */
static void record(tb_replay *r, int ticks, uint8_t mask)
{
    tb_rng rng;
    tb_rng_seed(&rng, 1234, 1);
    r->seed = 77;
    r->player_mask = mask;
    make_sim(r->seed);
    for (int p = 0; p < TB_MAX_PLAYERS; p++) {
        if ((mask >> p) & 1) {
            tb_sim_add_player(&g_sim, p);
        }
    }
    int16_t yaw[TB_MAX_PLAYERS] = {0};
    for (int t = 0; t < ticks; t++) {
        tb_input in[TB_MAX_PLAYERS];
        memset(in, 0, sizeof(in));
        for (int p = 0; p < TB_MAX_PLAYERS; p++) {
            if (!((mask >> p) & 1)) {
                continue;
            }
            yaw[p] = (int16_t)(yaw[p] + tb_rng_range(&rng, -400, 400));
            in[p].yaw = yaw[p];
            in[p].forward = (int8_t)tb_rng_range(&rng, -40, 127);
            in[p].side = (int8_t)tb_rng_range(&rng, -127, 127);
            in[p].buttons = (uint16_t)(tb_rng_chance(&rng, 0.2f) ? TB_BTN_FIRE : 0) |
                            (uint16_t)(tb_rng_chance(&rng, 0.05f) ? TB_BTN_JUMP : 0) |
                            (uint16_t)(tb_rng_chance(&rng, 0.02f) ? TB_BTN_CROUCH : 0);
            in[p].weapon = (uint8_t)(tb_rng_chance(&rng, 0.01f) ? tb_rng_range(&rng, 1, 3) : 0);
        }
        TEST_ASSERT_TRUE(tb_replay_push(r, in));
        tb_sim_step(&g_sim, in);
    }
    r->final_checksum = tb_sim_checksum(&g_sim);
}

static uint64_t replay(const tb_replay *r)
{
    make_sim(r->seed);
    return tb_replay_play(r, &g_sim);
}

/* @scenario quality-gates: Deterministic replay */
static void test_replay_reproduces_checksum_twice(void)
{
    tb_replay r;
    tb_replay_init(&r, NULL);
    record(&r, 60 * 30, 0x3); /* 30 seconds, 2 players */
    TEST_ASSERT_EQUAL_UINT(1800, tb_replay_length(&r));
    TEST_ASSERT_EQUAL_HEX64(r.final_checksum, replay(&r));
    TEST_ASSERT_EQUAL_HEX64(r.final_checksum, replay(&r));
    tb_replay_free(&r);
}

static void test_encode_decode_roundtrip(void)
{
    tb_replay r, back;
    tb_replay_init(&r, NULL);
    tb_replay_init(&back, NULL);
    record(&r, 600, 0x5);
    size_t cap = tb_replay_encoded_size(&r);
    uint8_t *buf = malloc(cap);
    size_t n = tb_replay_encode(&r, buf, cap);
    TEST_ASSERT_EQUAL_UINT(cap, n);
    TEST_ASSERT_TRUE(tb_replay_decode(&back, buf, n));
    TEST_ASSERT_EQUAL_UINT(600, tb_replay_length(&back));
    TEST_ASSERT_EQUAL_HEX8(0x5, back.player_mask);
    TEST_ASSERT_EQUAL_HEX64(r.final_checksum, back.final_checksum);
    TEST_ASSERT_EQUAL_HEX64(back.final_checksum, replay(&back));
    TEST_ASSERT_EQUAL_UINT(0, tb_replay_encode(&r, buf, n - 1));
    TEST_ASSERT_EQUAL_UINT(0, tb_replay_encode(&r, buf, 10));
    free(buf);
    tb_replay_free(&r);
    tb_replay_free(&back);
}

static void test_decode_rejects_bad_data(void)
{
    tb_replay r, back;
    tb_replay_init(&r, NULL);
    tb_replay_init(&back, NULL);
    record(&r, 10, 0x1);
    uint8_t buf[512];
    size_t n = tb_replay_encode(&r, buf, sizeof(buf));
    TEST_ASSERT_TRUE(n > 0);

    uint8_t bad[512];
    memcpy(bad, buf, n);
    bad[30] ^= 1; /* corrupt: CRC mismatch */
    TEST_ASSERT_FALSE(tb_replay_decode(&back, bad, n));
    TEST_ASSERT_FALSE(tb_replay_decode(&back, buf, 3));
    TEST_ASSERT_FALSE(tb_replay_decode(&back, NULL, 0));

    /* Valid CRC but wrong magic / player mask / length: re-sign after editing. */
    const size_t edits[][2] = {{0, 'X'}, {15, 0x00}, {15, 0x30}, {20, 0x7F}};
    for (size_t i = 0; i < sizeof(edits) / sizeof(edits[0]); i++) {
        memcpy(bad, buf, n);
        bad[edits[i][0]] = (uint8_t)edits[i][1];
        uint32_t crc = tb_crc32(bad, n - 4);
        for (int k = 0; k < 4; k++) {
            bad[n - 4 + (size_t)k] = (uint8_t)(crc >> (8 * k));
        }
        TEST_ASSERT_FALSE(tb_replay_decode(&back, bad, n));
        TEST_ASSERT_EQUAL_UINT(0, tb_replay_length(&back));
    }
    tb_replay_free(&r);
    tb_replay_free(&back);
}

static void test_decode_out_of_memory(void)
{
    tb_replay r, back;
    tb_replay_init(&r, NULL);
    record(&r, 10, 0x1);
    uint8_t buf[512];
    size_t n = tb_replay_encode(&r, buf, sizeof(buf));
    tb_test_alloc ta;
    tb_test_alloc_init(&ta, 0);
    tb_replay_init(&back, &ta.iface);
    TEST_ASSERT_FALSE(tb_replay_decode(&back, buf, n));
    tb_replay_free(&r);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_replay_reproduces_checksum_twice);
    RUN_TEST(test_encode_decode_roundtrip);
    RUN_TEST(test_decode_rejects_bad_data);
    RUN_TEST(test_decode_out_of_memory);
    return UNITY_END();
}
