/* SPDX-License-Identifier: MIT */
#include "sim/geometry.h"

#include <math.h>
#include <string.h>

void tb_geometry_init(tb_geometry *g, tb_brush *storage, int cap)
{
    g->brushes = storage;
    g->count = 0;
    g->cap = storage ? cap : 0;
}

static tb_brush *new_brush(tb_geometry *g, uint32_t contents)
{
    if (g->count >= g->cap) {
        return NULL;
    }
    tb_brush *b = &g->brushes[g->count++];
    memset(b, 0, sizeof(*b));
    b->contents = contents;
    return b;
}

static void add_plane(tb_brush *b, tb_vec3 n, float d)
{
    b->planes[b->count].n = n;
    b->planes[b->count].d = d;
    b->count++;
}

static void add_axial_planes(tb_brush *b, tb_vec3 mins, tb_vec3 maxs)
{
    add_plane(b, tb_v3(1, 0, 0), maxs.x);
    add_plane(b, tb_v3(-1, 0, 0), -mins.x);
    add_plane(b, tb_v3(0, 1, 0), maxs.y);
    add_plane(b, tb_v3(0, -1, 0), -mins.y);
    add_plane(b, tb_v3(0, 0, 1), maxs.z);
    add_plane(b, tb_v3(0, 0, -1), -mins.z);
}

bool tb_geometry_add_box(tb_geometry *g, tb_vec3 mins, tb_vec3 maxs, uint32_t contents)
{
    tb_brush *b = new_brush(g, contents);
    if (!b) {
        return false;
    }
    add_axial_planes(b, mins, maxs);
    return true;
}

bool tb_geometry_add_brush(tb_geometry *g, const tb_plane *planes, int count, tb_vec3 bounds_min,
                           tb_vec3 bounds_max, uint32_t contents)
{
    if (count < 1 || count + 6 > TB_BRUSH_MAX_PLANES) {
        return false;
    }
    tb_brush *b = new_brush(g, contents);
    if (!b) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        add_plane(b, tb_v3_normalize(planes[i].n), planes[i].d / tb_v3_len(planes[i].n));
    }
    add_axial_planes(b, bounds_min, bounds_max);
    return true;
}

/* Quake's CM_ClipBoxToBrush, adapted. */
static void clip_box_to_brush(const tb_brush *b, tb_vec3 start, tb_vec3 end, tb_vec3 mins, tb_vec3 maxs,
                              tb_trace *tr)
{
    float enter = -1.0f, leave = 1.0f;
    bool start_out = false, get_out = false;
    tb_vec3 clip_normal = tb_v3(0, 0, 0);

    for (int i = 0; i < b->count; i++) {
        const tb_plane *p = &b->planes[i];
        tb_vec3 ofs =
            tb_v3(p->n.x < 0 ? maxs.x : mins.x, p->n.y < 0 ? maxs.y : mins.y, p->n.z < 0 ? maxs.z : mins.z);
        float dist = p->d - tb_v3_dot(ofs, p->n);
        float d1 = tb_v3_dot(start, p->n) - dist;
        float d2 = tb_v3_dot(end, p->n) - dist;
        if (d2 > 0) {
            get_out = true;
        }
        if (d1 > 0) {
            start_out = true;
        }
        if (d1 > 0 && (d2 >= TB_TRACE_EPSILON || d2 >= d1)) {
            return; /* completely in front of this plane: no contact */
        }
        if (d1 <= 0 && d2 <= 0) {
            continue;
        }
        if (d1 > d2) { /* entering */
            float f = (d1 - TB_TRACE_EPSILON) / (d1 - d2);
            if (f > enter) {
                enter = f;
                clip_normal = p->n;
            }
        } else { /* leaving */
            float f = (d1 + TB_TRACE_EPSILON) / (d1 - d2);
            if (f < leave) {
                leave = f;
            }
        }
    }

    if (!start_out) {
        tr->start_solid = true;
        if (!get_out) {
            tr->all_solid = true;
            tr->fraction = 0.0f;
        }
        return;
    }
    if (enter < leave && enter > -1.0f && enter < tr->fraction) {
        tr->fraction = enter < 0.0f ? 0.0f : enter;
        tr->normal = clip_normal;
    }
}

tb_trace tb_trace_box(const tb_geometry *g, tb_vec3 start, tb_vec3 end, tb_vec3 mins, tb_vec3 maxs,
                      uint32_t mask)
{
    tb_trace tr;
    memset(&tr, 0, sizeof(tr));
    tr.fraction = 1.0f;
    for (int i = 0; i < g->count; i++) {
        if (g->brushes[i].contents & mask) {
            clip_box_to_brush(&g->brushes[i], start, end, mins, maxs, &tr);
        }
    }
    tr.end = tb_v3_lerp(start, end, tr.fraction);
    return tr;
}

tb_trace tb_trace_ray(const tb_geometry *g, tb_vec3 start, tb_vec3 end, uint32_t mask)
{
    return tb_trace_box(g, start, end, tb_v3(0, 0, 0), tb_v3(0, 0, 0), mask);
}

float tb_ray_box(tb_vec3 start, tb_vec3 end, tb_vec3 box_min, tb_vec3 box_max)
{
    const float s[3] = {start.x, start.y, start.z};
    const float e[3] = {end.x, end.y, end.z};
    const float lo[3] = {box_min.x, box_min.y, box_min.z};
    const float hi[3] = {box_max.x, box_max.y, box_max.z};
    float t0 = 0.0f, t1 = 1.0f;
    for (int a = 0; a < 3; a++) {
        float d = e[a] - s[a];
        if (fabsf(d) < 1e-9f) {
            if (s[a] < lo[a] || s[a] > hi[a]) {
                return 2.0f;
            }
            continue;
        }
        float ta = (lo[a] - s[a]) / d, tb = (hi[a] - s[a]) / d;
        if (ta > tb) {
            float tmp = ta;
            ta = tb;
            tb = tmp;
        }
        t0 = ta > t0 ? ta : t0;
        t1 = tb < t1 ? tb : t1;
        if (t0 > t1) {
            return 2.0f;
        }
    }
    return t0;
}

bool tb_boxes_overlap(tb_vec3 amin, tb_vec3 amax, tb_vec3 bmin, tb_vec3 bmax)
{
    return amin.x <= bmax.x && amax.x >= bmin.x && amin.y <= bmax.y && amax.y >= bmin.y && amin.z <= bmax.z &&
           amax.z >= bmin.z;
}

void tb_geometry_build_test_arena(tb_geometry *g, tb_brush storage[TB_TEST_ARENA_BRUSHES])
{
    tb_geometry_init(g, storage, TB_TEST_ARENA_BRUSHES);
    const uint32_t S = TB_CONTENTS_SOLID;
    /* 40 x 40 m room, floor top at z = 0, walls 6 m high. */
    tb_geometry_add_box(g, tb_v3(-20, -20, -1), tb_v3(20, 20, 0), S);   /* floor */
    tb_geometry_add_box(g, tb_v3(-20, -20, 6), tb_v3(20, 20, 7), S);    /* ceiling */
    tb_geometry_add_box(g, tb_v3(-21, -20, 0), tb_v3(-20, 20, 6), S);   /* west wall */
    tb_geometry_add_box(g, tb_v3(20, -20, 0), tb_v3(21, 20, 6), S);     /* east wall */
    tb_geometry_add_box(g, tb_v3(-20, -21, 0), tb_v3(20, -20, 6), S);   /* south wall */
    tb_geometry_add_box(g, tb_v3(-20, 20, 0), tb_v3(20, 21, 6), S);     /* north wall */
    tb_geometry_add_box(g, tb_v3(5, -2, 0), tb_v3(8, 2, 0.4f), S);      /* stair step 0.4 m */
    tb_geometry_add_box(g, tb_v3(10, -2, 0), tb_v3(12, 2, 1.0f), S);    /* ledge 1 m (too high to step) */
    tb_geometry_add_box(g, tb_v3(-6, 5, 0), tb_v3(-4, 7, 6), S);        /* pillar */
    tb_geometry_add_box(g, tb_v3(-12, -12, 1.3f), tb_v3(-8, -8, 6), S); /* low ceiling (crawl space) */
    /* Ramp rising along +y from (y=10, z=0) to (y=16, z=3): 26.6 degrees. */
    tb_plane ramp[] = {{{0, -1, 2}, -10.0f}}; /* 2z - y <= -10: below z = (y - 10) / 2 */
    tb_geometry_add_brush(g, ramp, 1, tb_v3(-4, 10, 0), tb_v3(4, 16, 3), S);
    tb_geometry_add_box(g, tb_v3(-4, 16, 0), tb_v3(4, 20, 3), S); /* ramp top platform */
}
