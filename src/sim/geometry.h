/* SPDX-License-Identifier: MIT */
/* Static level geometry as convex brushes (Quake-style) and swept-box
 * traces against it. Coordinates: metres, Z up. */
#ifndef TB_SIM_GEOMETRY_H
#define TB_SIM_GEOMETRY_H

#include "core/mathx.h"
#include "core/types.h"

#define TB_BRUSH_MAX_PLANES 16
#define TB_TRACE_EPSILON    0.001f

/* Points p with dot(n, p) <= d are behind (inside) the plane. */
typedef struct tb_plane {
    tb_vec3 n;
    float d;
} tb_plane;

typedef struct tb_brush {
    tb_plane planes[TB_BRUSH_MAX_PLANES];
    int count;
    uint32_t contents; /* TB_CONTENTS_* */
} tb_brush;

#define TB_CONTENTS_SOLID      1u
#define TB_CONTENTS_PLAYERCLIP 2u /* blocks players, not shots */

typedef struct tb_geometry {
    tb_brush *brushes;
    int count;
    int cap;
} tb_geometry;

typedef struct tb_trace {
    float fraction;   /* 1 = no hit */
    tb_vec3 end;      /* final position */
    tb_vec3 normal;   /* surface normal of the hit plane */
    bool start_solid; /* started inside a brush */
    bool all_solid;   /* never left solid */
} tb_trace;

/* Uses caller storage; returns false when full. */
void tb_geometry_init(tb_geometry *g, tb_brush *storage, int cap);
/* Adds an axis-aligned box brush. */
bool tb_geometry_add_box(tb_geometry *g, tb_vec3 mins, tb_vec3 maxs, uint32_t contents);
/* Adds a convex brush from planes; axial bevel planes from `bounds_min/max`
 * are added so box traces against sloped brushes stay correct. */
bool tb_geometry_add_brush(tb_geometry *g, const tb_plane *planes, int count, tb_vec3 bounds_min,
                           tb_vec3 bounds_max, uint32_t contents);

/* Sweeps an axis-aligned box (mins/maxs relative to the origin) from start
 * to end against all brushes whose contents intersect `mask`. */
tb_trace tb_trace_box(const tb_geometry *g, tb_vec3 start, tb_vec3 end, tb_vec3 mins, tb_vec3 maxs,
                      uint32_t mask);
/* Ray (zero-size box). */
tb_trace tb_trace_ray(const tb_geometry *g, tb_vec3 start, tb_vec3 end, uint32_t mask);

/* Ray against an axis-aligned box; returns the entry fraction in [0, 1] or
 * a value > 1 if missed. */
float tb_ray_box(tb_vec3 start, tb_vec3 end, tb_vec3 box_min, tb_vec3 box_max);
bool tb_boxes_overlap(tb_vec3 amin, tb_vec3 amax, tb_vec3 bmin, tb_vec3 bmax);

/* A small test arena (floor, walls, a stair step, a ramp, a pillar, a low
 * ceiling) used by tests, the headless smoke run and the placeholder app. */
#define TB_TEST_ARENA_BRUSHES 12
void tb_geometry_build_test_arena(tb_geometry *g, tb_brush storage[TB_TEST_ARENA_BRUSHES]);

#endif
