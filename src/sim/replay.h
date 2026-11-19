/* SPDX-License-Identifier: MIT */
/* Replays: the inputs of every tick plus the expected final state
 * checksum. Playing a replay on the same build must reproduce the checksum
 * exactly (determinism tests, golden-path regression tests). */
#ifndef TB_SIM_REPLAY_H
#define TB_SIM_REPLAY_H

#include "core/dynarr.h"
#include "sim/sim.h"

#define TB_REPLAY_MAGIC   0x50524254u /* "TBRP" */
#define TB_REPLAY_VERSION 1u

typedef struct tb_replay {
    uint64_t seed;
    uint8_t difficulty;
    uint8_t player_mask; /* bit n = player slot n active */
    uint32_t level;      /* level number (0 = test arena) */
    uint64_t final_checksum;
    tb_dynarr ticks; /* elements: tb_input[TB_MAX_PLAYERS] */
} tb_replay;

typedef struct tb_replay_frame {
    tb_input inputs[TB_MAX_PLAYERS];
} tb_replay_frame;

void tb_replay_init(tb_replay *r, const tb_allocator *alloc);
void tb_replay_free(tb_replay *r);
bool tb_replay_push(tb_replay *r, const tb_input inputs[TB_MAX_PLAYERS]);
size_t tb_replay_length(const tb_replay *r);

/* Encoded size upper bound for a replay. */
size_t tb_replay_encoded_size(const tb_replay *r);
/* Returns bytes written or 0 if the buffer is too small. */
size_t tb_replay_encode(const tb_replay *r, void *buf, size_t cap);
/* Returns false on malformed input (r is left empty). */
bool tb_replay_decode(tb_replay *r, const void *buf, size_t len);

/* Plays the replay on a freshly initialised sim (with players added
 * according to player_mask); returns the resulting checksum. */
uint64_t tb_replay_play(const tb_replay *r, tb_sim *sim);

#endif
