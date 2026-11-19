/* SPDX-License-Identifier: MIT */
/* Fixed-time-step accumulator: the simulation always advances in steps of
 * 1/hz seconds; rendering interpolates with `alpha`. */
#ifndef TB_CORE_TIMESTEP_H
#define TB_CORE_TIMESTEP_H

#include "core/types.h"

typedef struct tb_timestep {
    double step;        /* seconds per tick */
    double accumulator; /* unsimulated time */
    double last_time;   /* last real time seen, < 0 before the first frame */
    uint32_t max_steps; /* per frame, prevents the spiral of death */
    uint64_t tick;      /* total ticks simulated */
} tb_timestep;

void tb_timestep_init(tb_timestep *ts, uint32_t hz, uint32_t max_steps_per_frame);
/* Feeds the current real time (seconds); returns how many ticks to simulate
 * now. The first call returns 0. Time going backwards counts as no time;
 * time beyond max_steps is dropped. */
uint32_t tb_timestep_advance(tb_timestep *ts, double now_seconds);
/* Interpolation factor between the previous and current tick, in [0, 1). */
float tb_timestep_alpha(const tb_timestep *ts);

#endif
