/* SPDX-License-Identifier: MIT */
#include "core/timestep.h"

void tb_timestep_init(tb_timestep *ts, uint32_t hz, uint32_t max_steps_per_frame)
{
    ts->step = 1.0 / (double)(hz ? hz : 60u);
    ts->accumulator = 0.0;
    ts->last_time = -1.0;
    ts->max_steps = max_steps_per_frame ? max_steps_per_frame : 1u;
    ts->tick = 0;
}

uint32_t tb_timestep_advance(tb_timestep *ts, double now_seconds)
{
    if (ts->last_time < 0.0) {
        ts->last_time = now_seconds;
        return 0;
    }
    double dt = now_seconds - ts->last_time;
    ts->last_time = now_seconds;
    if (dt > 0.0) {
        ts->accumulator += dt;
    }
    uint32_t steps = 0;
    while (ts->accumulator >= ts->step && steps < ts->max_steps) {
        ts->accumulator -= ts->step;
        steps++;
    }
    if (steps == ts->max_steps && ts->accumulator >= ts->step) {
        /* Too far behind: drop the backlog instead of catching up forever. */
        ts->accumulator = 0.0;
    }
    ts->tick += steps;
    return steps;
}

float tb_timestep_alpha(const tb_timestep *ts)
{
    float a = (float)(ts->accumulator / ts->step);
    return a < 0.0f ? 0.0f : (a >= 1.0f ? 0.999999f : a);
}
