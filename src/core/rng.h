/* SPDX-License-Identifier: MIT */
/* PCG32 random number generator: deterministic and seedable. All game
 * randomness must come from a tb_rng owned by the simulation. */
#ifndef TB_CORE_RNG_H
#define TB_CORE_RNG_H

#include "core/types.h"

typedef struct tb_rng {
    uint64_t state;
    uint64_t inc;
} tb_rng;

void tb_rng_seed(tb_rng *r, uint64_t seed, uint64_t stream);
uint32_t tb_rng_u32(tb_rng *r);
/* Uniform integer in [lo, hi] (inclusive), unbiased. Returns lo if hi < lo. */
int32_t tb_rng_range(tb_rng *r, int32_t lo, int32_t hi);
/* Uniform float in [0, 1). */
float tb_rng_float01(tb_rng *r);
/* True with probability p (clamped to [0, 1]). */
bool tb_rng_chance(tb_rng *r, float p);

#endif
