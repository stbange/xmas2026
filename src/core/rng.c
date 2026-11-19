/* SPDX-License-Identifier: MIT */
#include "core/rng.h"

void tb_rng_seed(tb_rng *r, uint64_t seed, uint64_t stream)
{
    r->state = 0u;
    r->inc = (stream << 1u) | 1u;
    (void)tb_rng_u32(r);
    r->state += seed;
    (void)tb_rng_u32(r);
}

uint32_t tb_rng_u32(tb_rng *r)
{
    uint64_t old = r->state;
    r->state = old * 6364136223846793005ull + r->inc;
    uint32_t xorshifted = (uint32_t)(((old >> 18u) ^ old) >> 27u);
    uint32_t rot = (uint32_t)(old >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((0u - rot) & 31u));
}

int32_t tb_rng_range(tb_rng *r, int32_t lo, int32_t hi)
{
    if (hi <= lo) {
        return lo;
    }
    uint32_t span = (uint32_t)((int64_t)hi - (int64_t)lo) + 1u;
    if (span == 0u) { /* full 32-bit range */
        return (int32_t)tb_rng_u32(r);
    }
    uint32_t threshold = (0u - span) % span;
    uint32_t v;
    do {
        v = tb_rng_u32(r);
    } while (v < threshold);
    return (int32_t)((int64_t)lo + (int64_t)(v % span));
}

float tb_rng_float01(tb_rng *r)
{
    return (float)(tb_rng_u32(r) >> 8) * (1.0f / 16777216.0f);
}

bool tb_rng_chance(tb_rng *r, float p)
{
    if (p <= 0.0f) {
        return false;
    }
    if (p >= 1.0f) {
        return true;
    }
    return tb_rng_float01(r) < p;
}
