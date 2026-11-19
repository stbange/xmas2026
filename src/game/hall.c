/* SPDX-License-Identifier: MIT */
#include "game/hall.h"

#include <math.h>
#include <string.h>

#define HALL_X        30.0f /* hall spans -30..30 */
#define HALL_Y        20.0f /* hall spans -20..20 */
#define HALL_H        14.0f
#define WALL          1.0f
#define BALCONY_DEPTH 6.0f
#define DOOR_W        2.4f
#define POST_W        0.4f
#define DOOR_DEPTH    0.6f

static void door_at(tb_hall_door *d, int day, float x, float y, float z, float ny)
{
    d->day = day;
    d->normal = tb_v3(0, ny, 0);
    d->centre = tb_v3(x, y, z);
    float inner = y + ny * DOOR_DEPTH;
    float lo = y < inner ? y : inner, hi = y < inner ? inner : y;
    d->vol_min = tb_v3(x - DOOR_W * 0.5f, lo, z);
    d->vol_max = tb_v3(x + DOOR_W * 0.5f, hi, z + TB_HALL_DOOR_HEIGHT);
    d->spawn = tb_v3(x, y + ny * 2.5f, z + 0.01f);
    d->spawn_yaw = ny > 0 ? TB_PI / 2.0f : -TB_PI / 2.0f; /* facing into the hall */
}

static void prop(tb_hall *h, tb_prop_kind kind, tb_vec3 mins, tb_vec3 maxs, uint8_t r, uint8_t g, uint8_t b,
                 bool solid, bool emissive)
{
    tb_hall_prop *p = &h->props[h->prop_count++];
    p->kind = kind;
    p->mins = mins;
    p->maxs = maxs;
    p->color[0] = r;
    p->color[1] = g;
    p->color[2] = b;
    p->color[3] = 255;
    p->solid = solid;
    p->emissive = emissive;
}

void tb_hall_layout(tb_hall *h)
{
    memset(h, 0, sizeof(*h));
    /* Doors 1-12: ground floor, south wall, with a gap in the middle for
     * the BIOS gate. Doors 13-24: balcony, north wall. */
    for (int i = 0; i < 12; i++) {
        float x = i < 6 ? -26.5f + 3.9f * (float)i : 6.0f + 3.9f * (float)(i - 6);
        door_at(&h->doors[i], i + 1, x, -HALL_Y, 0.0f, 1.0f);
        float xu = -25.3f + 4.6f * (float)i;
        door_at(&h->doors[12 + i], 13 + i, xu, HALL_Y, TB_HALL_BALCONY_Z, -1.0f);
    }
    h->spawn = tb_v3(0, -14.0f, 0.01f);
    h->spawn_yaw = TB_PI / 2.0f;
    h->tree_base = tb_v3(0, 1.0f, 0);
    h->tree_height = 11.0f;
    h->tree_radius = 3.6f;
    h->arena_min = tb_v3(-52, -12, 0);
    h->arena_max = tb_v3(-31, 12, 8);

    /* Props (colour: festive, readable at 480x270). */
    prop(h, TB_PROP_BIOS_GATE, tb_v3(-3, -HALL_Y, 0), tb_v3(3, -HALL_Y + 0.5f, 5), 90, 200, 255, false, true);
    prop(h, TB_PROP_NOEL_DESK, tb_v3(6, -9, 0), tb_v3(10, -7.8f, 1.1f), 120, 80, 50, true, false);
    prop(h, TB_PROP_CRT, tb_v3(7.6f, -8.8f, 1.1f), tb_v3(8.6f, -8.0f, 1.9f), 80, 255, 140, false, true);
    prop(h, TB_PROP_TROPHY_WALL, tb_v3(-29.9f, -16, 1.0f), tb_v3(-29.4f, -6, 1.3f), 200, 170, 90, true,
         false);
    prop(h, TB_PROP_TROPHY_WALL, tb_v3(-29.9f, -16, 2.4f), tb_v3(-29.4f, -6, 2.7f), 200, 170, 90, true,
         false);
    prop(h, TB_PROP_JUKEBOX, tb_v3(27.5f, -4, 0), tb_v3(29, -2, 2.2f), 255, 90, 200, true, true);
    prop(h, TB_PROP_COCOA_BAR, tb_v3(27.6f, -13, 0), tb_v3(29.5f, -7, 1.1f), 150, 90, 60, true, false);
    for (int t = 0; t < 3; t++) {
        float ty = -6.0f + 3.2f * (float)t;
        prop(h, TB_PROP_LAN_TABLE, tb_v3(-24, ty, 0), tb_v3(-16, ty + 1.2f, 0.8f), 110, 100, 90, true, false);
        for (int c = 0; c < 4; c++) {
            float cx = -23.5f + 2.0f * (float)c;
            prop(h, TB_PROP_CRT, tb_v3(cx, ty + 0.2f, 0.8f), tb_v3(cx + 0.8f, ty + 0.9f, 1.5f), 70, 120, 255,
                 false, true);
        }
    }
    prop(h, TB_PROP_ARENA, tb_v3(-29.95f, -2.2f, 4.1f), tb_v3(-29.75f, 2.2f, 4.7f), 255, 60, 60, false, true);
}

static void add_box(tb_geometry *g, float x0, float y0, float z0, float x1, float y1, float z1, uint32_t c)
{
    tb_geometry_add_box(g, tb_v3(x0, y0, z0), tb_v3(x1, y1, z1), c);
}

static void add_door_frame(tb_geometry *g, const tb_hall_door *d)
{
    const uint32_t S = TB_CONTENTS_SOLID;
    float x = d->centre.x, z = d->centre.z;
    float y0 = d->vol_min.y, y1 = d->vol_max.y;
    float half = DOOR_W * 0.5f;
    add_box(g, x - half - POST_W, y0, z, x - half, y1, z + TB_HALL_DOOR_HEIGHT + 0.6f, S);
    add_box(g, x + half, y0, z, x + half + POST_W, y1, z + TB_HALL_DOOR_HEIGHT + 0.6f, S);
    add_box(g, x - half, y0, z + TB_HALL_DOOR_HEIGHT, x + half, y1, z + TB_HALL_DOOR_HEIGHT + 0.6f, S);
}

bool tb_hall_build_geometry(const tb_hall *h, tb_portal_mask open, tb_geometry *g, tb_brush *storage)
{
    const uint32_t S = TB_CONTENTS_SOLID, CLIP = TB_CONTENTS_PLAYERCLIP;
    tb_geometry_init(g, storage, TB_HALL_MAX_BRUSHES);
    /* Shell: floor, ceiling, walls. The west wall has the arena opening. */
    add_box(g, -HALL_X, -HALL_Y, -1, HALL_X, HALL_Y, 0, S);
    add_box(g, -HALL_X, -HALL_Y, HALL_H, HALL_X, HALL_Y, HALL_H + 1, S);
    add_box(g, -HALL_X, -HALL_Y - WALL, 0, HALL_X, -HALL_Y, HALL_H, S);
    add_box(g, -HALL_X, HALL_Y, 0, HALL_X, HALL_Y + WALL, HALL_H, S);
    add_box(g, HALL_X, -HALL_Y, 0, HALL_X + WALL, HALL_Y, HALL_H, S);
    add_box(g, -HALL_X - WALL, -HALL_Y, 0, -HALL_X, -2.2f, HALL_H, S);
    add_box(g, -HALL_X - WALL, 2.2f, 0, -HALL_X, HALL_Y, HALL_H, S);
    add_box(g, -HALL_X - WALL, -2.2f, 4.0f, -HALL_X, 2.2f, HALL_H, S);

    /* Balcony slab along the north wall with a railing, and two stairs. */
    const float by = HALL_Y - BALCONY_DEPTH, bz = TB_HALL_BALCONY_Z;
    add_box(g, -HALL_X, by, bz - 0.4f, HALL_X, HALL_Y, bz, S);
    add_box(g, -24.0f, by, bz, 24.0f, by + 0.2f, bz + 1.0f, S); /* railing between the stairs */
    for (int k = 0; k < 13; k++) {
        float top = (float)(k + 1) * (bz / 13.0f);
        float y0 = 3.0f + (float)k * ((by - 3.0f) / 13.0f);     /* clear of the arena opening */
        add_box(g, -HALL_X, y0, 0, -HALL_X + 3.0f, by, top, S); /* west stairs */
        add_box(g, HALL_X - 3.0f, y0, 0, HALL_X, by, top, S);   /* east stairs */
    }
    /* Door frames, and a player clip in every closed doorway. */
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        const tb_hall_door *d = &h->doors[i];
        add_door_frame(g, d);
        if (!tb_portal_is_open(open, d->day)) {
            tb_geometry_add_box(g, d->vol_min, d->vol_max, CLIP | S);
        }
    }
    /* The tree (a solid core you walk around) and solid props. */
    tb_vec3 t = h->tree_base;
    add_box(g, t.x - 2.0f, t.y - 2.0f, 0, t.x + 2.0f, t.y + 2.0f, h->tree_height, CLIP);
    for (int i = 0; i < h->prop_count; i++) {
        if (h->props[i].solid) {
            tb_geometry_add_box(g, h->props[i].mins, h->props[i].maxs, S);
        }
    }
    /* Snowball Arena: a walled room behind the west opening, with cover. */
    tb_vec3 a0 = h->arena_min, a1 = h->arena_max;
    add_box(g, a0.x, a0.y, -1, a1.x + 1, a1.y, 0, S);
    add_box(g, a0.x, a0.y, a1.z, a1.x + 1, a1.y, a1.z + 1, S);
    add_box(g, a0.x - 1, a0.y, 0, a0.x, a1.y, a1.z, S);
    add_box(g, a0.x, a0.y - 1, 0, a1.x + 1, a0.y, a1.z, S);
    add_box(g, a0.x, a1.y, 0, a1.x + 1, a1.y + 1, a1.z, S);
    add_box(g, -45, -6, 0, -43, -4, 1.4f, S);
    add_box(g, -45, 4, 0, -43, 6, 1.4f, S);
    add_box(g, -40, -1, 0, -38, 1, 2.0f, S);
    return g->count < TB_HALL_MAX_BRUSHES; /* false if it ran out of space */
}

void tb_hall_add_portals(const tb_hall *h, tb_portal_mask open, tb_sim *sim)
{
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        const tb_hall_door *d = &h->doors[i];
        if (tb_portal_is_open(open, d->day)) {
            /* Slightly larger than the doorway so walking in is enough. */
            tb_vec3 grow = tb_v3(0, d->normal.y * 0.4f, 0);
            tb_vec3 lo = d->vol_min, hi = d->vol_max;
            if (grow.y > 0) {
                hi = tb_v3_add(hi, grow);
            } else {
                lo = tb_v3_add(lo, grow);
            }
            tb_sim_add_trigger(sim, TB_TRIGGER_PORTAL, lo, hi, d->day);
        }
    }
}

const tb_hall_door *tb_hall_nearest_door(const tb_hall *h, tb_vec3 pos, float range)
{
    const tb_hall_door *best = NULL;
    float best_d = range * range;
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        const tb_hall_door *d = &h->doors[i];
        if (fabsf(pos.z - d->centre.z) > 2.5f) {
            continue; /* other floor */
        }
        float dx = pos.x - d->centre.x, dy = pos.y - d->centre.y;
        float dd = dx * dx + dy * dy;
        if (dd <= best_d) {
            best_d = dd;
            best = d;
        }
    }
    return best;
}

int tb_hall_tree_baubles(uint32_t completed_mask)
{
    int n = 0;
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        n += (int)((completed_mask >> i) & 1u);
    }
    return n;
}

tb_vec3 tb_hall_bauble_pos(const tb_hall *h, int n)
{
    /* A spiral around the cone: ring n/4, four baubles per ring. */
    int ring = n / 4;
    float t = 0.12f + 0.13f * (float)ring;          /* height fraction */
    float r = h->tree_radius * (1.0f - t) + 0.15f;  /* cone radius there */
    float a = (float)n * 1.7f + (float)ring * 0.6f; /* angle */
    return tb_v3(h->tree_base.x + cosf(a) * r, h->tree_base.y + sinf(a) * r, h->tree_height * t);
}

tb_vec3 tb_hall_ring_bulb_pos(const tb_hall *h, int ring, int bulb)
{
    float t = 0.08f + 0.85f * (float)ring / (float)TB_PORTAL_COUNT;
    float r = h->tree_radius * (1.0f - t) * 0.92f + 0.1f;
    float a = (float)bulb * (2.0f * TB_PI / TB_TREE_RING_BULBS) + (float)ring * 0.5f;
    return tb_v3(h->tree_base.x + cosf(a) * r, h->tree_base.y + sinf(a) * r,
                 h->tree_base.z + h->tree_height * t);
}

tb_hall_mood tb_hall_mood_for(tb_story_act act, int baubles)
{
    tb_hall_mood m;
    memset(&m, 0, sizeof(m));
    m.tree_light = 0.15f + 0.85f * (float)baubles / (float)TB_PORTAL_COUNT;
    switch (act) {
    case TB_ACT_DEFRAG:
        m.ambient = tb_v3(0.16f, 0.17f, 0.24f);
        m.sun_color = tb_v3(0.45f, 0.5f, 0.75f);
        m.fog_color = tb_v3(0.05f, 0.05f, 0.09f);
        break;
    case TB_ACT_SHUTDOWN:
        m.ambient = tb_v3(0.22f, 0.08f, 0.08f);
        m.sun_color = tb_v3(0.35f, 0.25f, 0.4f);
        m.fog_color = tb_v3(0.08f, 0.02f, 0.03f);
        m.emergency = true;
        break;
    case TB_ACT_RESTORED:
        m.ambient = tb_v3(0.35f, 0.3f, 0.25f);
        m.sun_color = tb_v3(0.8f, 0.75f, 0.65f);
        m.fog_color = tb_v3(0.12f, 0.09f, 0.07f);
        m.tree_light = 1.0f;
        break;
    default: /* BOOT */
        m.ambient = tb_v3(0.20f, 0.22f, 0.32f);
        m.sun_color = tb_v3(0.55f, 0.6f, 0.85f);
        m.fog_color = tb_v3(0.04f, 0.05f, 0.10f);
        break;
    }
    return m;
}
