/* SPDX-License-Identifier: MIT */
#include "app/world_view.h"

#include <math.h>
#include <string.h>

#define TREE_X     32
#define TREE_Y     32
#define TREE_Z     48
#define TREE_VOXEL 0.25f

static uint8_t g_tree[TREE_Z * TREE_Y * TREE_X];
static uint8_t g_tree_palette[256][4];
static bool g_tree_built;

static void build_tree(void)
{
    const uint8_t colors[5][4] = {{0, 0, 0, 0},
                                  {96, 60, 30, 255},  /* 1 trunk */
                                  {20, 90, 45, 255},  /* 2 dark needles */
                                  {40, 130, 60, 255}, /* 3 needles */
                                  {235, 240, 255, 255} /* 4 snow */};
    memset(g_tree_palette, 0, sizeof(g_tree_palette));
    memcpy(g_tree_palette, colors, sizeof(colors));
    const float cx = (TREE_X - 1) * 0.5f, cy = (TREE_Y - 1) * 0.5f;
    for (int z = 0; z < TREE_Z; z++) {
        /* Four tiers, each wider at the bottom: the classic sawtooth tree. */
        float t = (float)z / (float)TREE_Z;
        float tier = fmodf(t * 4.0f, 1.0f);
        float radius = (1.0f - t) * 15.0f * (0.65f + 0.35f * (1.0f - tier));
        for (int y = 0; y < TREE_Y; y++) {
            for (int x = 0; x < TREE_X; x++) {
                float dx = (float)x - cx, dy = (float)y - cy;
                float d = sqrtf(dx * dx + dy * dy);
                uint8_t v = 0;
                if (z < 5 && d < 2.0f) {
                    v = 1;
                } else if (z >= 4 && d <= radius) {
                    bool rim = d > radius - 1.5f;
                    bool top = tier > 0.8f && rim;
                    v = top ? 4 : (((x + y + z) % 3 == 0) ? 2 : 3);
                }
                g_tree[(z * TREE_Y + y) * TREE_X + x] = v;
            }
        }
    }
    g_tree_built = true;
}

void tb_world_tree_voxels(tb_voxels *out)
{
    if (!g_tree_built) {
        build_tree();
    }
    out->sx = TREE_X;
    out->sy = TREE_Y;
    out->sz = TREE_Z;
    out->data = g_tree;
    out->voxel_size = TREE_VOXEL;
    out->origin = tb_v3(0, 0, 0);
    out->palette = (const uint8_t(*)[4])g_tree_palette;
}

static bool add_box(tb_mesh *m, tb_vec3 lo, tb_vec3 hi, const uint8_t c[4])
{
    tb_brush b;
    tb_geometry g;
    tb_geometry_init(&g, &b, 1);
    tb_geometry_add_box(&g, lo, hi, TB_CONTENTS_SOLID);
    return tb_mesh_add_brush(m, &b, c);
}

static bool hall_mesh(const tb_app *app, tb_mesh *m)
{
    static const uint8_t floor[4] = {120, 85, 55, 255}, wall[4] = {150, 135, 165, 255},
                         ceiling[4] = {60, 55, 80, 255};
    static const uint8_t door_wood[4] = {120, 50, 35, 255}, door_frost[4] = {200, 225, 255, 255};
    const tb_hall *h = &app->hall;
    bool ok = true;
    /* Everything except player clips (closed doorways, the tree's core). */
    for (int i = 0; i < app->geometry.count && ok; i++) {
        const tb_brush *b = &app->geometry.brushes[i];
        if (b->contents & TB_CONTENTS_PLAYERCLIP) {
            continue;
        }
        tb_geometry one = {(tb_brush *)b, 1, 1};
        ok = tb_mesh_add_geometry(m, &one, floor, wall, ceiling);
    }
    for (int i = 0; i < h->prop_count && ok; i++) {
        const tb_hall_prop *p = &h->props[i];
        uint8_t c[4] = {p->color[0], p->color[1], p->color[2], p->emissive ? 128 : 255};
        ok = add_box(m, p->mins, p->maxs, c);
    }
    /* Closed advent doors: wooden panels with a frosted top. */
    for (int i = 0; i < TB_PORTAL_COUNT && ok; i++) {
        const tb_hall_door *d = &h->doors[i];
        if (tb_portal_is_open(app->open, d->day)) {
            continue;
        }
        tb_vec3 lo = d->vol_min, hi = d->vol_max;
        float mid = lo.y + (hi.y - lo.y) * 0.5f;
        tb_vec3 plo = tb_v3(lo.x, mid - 0.1f, lo.z), phi = tb_v3(hi.x, mid + 0.1f, hi.z);
        ok = add_box(m, plo, tb_v3(phi.x, phi.y, lo.z + 2.4f), door_wood) &&
             add_box(m, tb_v3(plo.x, plo.y, lo.z + 2.4f), phi, door_frost);
    }
    if (ok) {
        tb_voxels v;
        tb_world_tree_voxels(&v);
        v.voxel_size = h->tree_height / (float)TREE_Z;
        v.origin = tb_v3(h->tree_base.x - (float)TREE_X * v.voxel_size * 0.5f,
                         h->tree_base.y - (float)TREE_Y * v.voxel_size * 0.5f, h->tree_base.z);
        ok = tb_mesh_add_voxels(m, &v);
    }
    return ok;
}

static bool is_mover_brush(const tb_sim *sim, int brush)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (sim->entities[i].type == TB_ENT_MOVER && sim->entities[i].value == brush) {
            return true;
        }
    }
    return false;
}

static bool level_mesh(const tb_app *app, tb_mesh *m)
{
    const tb_level_def *def = app->level;
    bool ok = true;
    /* Static brushes: movers are drawn per frame, player clips not at all. */
    for (int i = 0; i < app->geometry.count && ok; i++) {
        const tb_brush *b = &app->geometry.brushes[i];
        if (!(b->contents & TB_CONTENTS_SOLID) || is_mover_brush(&app->sim, i)) {
            continue;
        }
        tb_geometry one = {(tb_brush *)b, 1, 1};
        ok = tb_mesh_add_geometry(m, &one, def->floor, def->wall, def->ceiling);
    }
    for (int i = 0; i < def->prop_count && ok; i++) {
        const tb_level_prop *p = &def->props[i];
        uint8_t c[4] = {p->color[0], p->color[1], p->color[2], p->emissive ? 128 : 255};
        ok = add_box(m, p->mins, p->maxs, c);
    }
    return ok;
}

bool tb_world_mesh_build(const tb_app *app, tb_mesh *mesh)
{
    tb_dynarr_clear(&mesh->vertices);
    if (app->flow == TB_FLOW_HALL || app->flow == TB_FLOW_BRIEFING || !app->level) {
        return hall_mesh(app, mesh);
    }
    return level_mesh(app, mesh);
}

static void hall_scene(const tb_app *app, tb_scene *s)
{
    const tb_hall *h = &app->hall;
    int baubles = tb_hall_tree_baubles(app->save.completed);
    tb_hall_mood mood = tb_hall_mood_for(tb_hall_act(app->calendar_day, app->save.completed), baubles);
    s->ambient = mood.ambient;
    s->sun_color = mood.sun_color;
    s->fog_color = mood.fog_color;
    s->fog_start = 25.0f;
    s->fog_end = 90.0f;

    /* Tree lights cycle slowly through red, green, gold and blue. */
    static const tb_vec3 colors[4] = {
        {2.0f, 0.3f, 0.2f}, {0.3f, 1.8f, 0.4f}, {1.8f, 1.4f, 0.4f}, {0.4f, 0.7f, 2.0f}};
    uint32_t phase = (uint32_t)(app->sim.tick / 90u);
    for (int i = 0; i < 4; i++) {
        float a = (float)i * (TB_PI / 2.0f);
        tb_vec3 pos = tb_v3_add(h->tree_base, tb_v3(cosf(a) * 4.5f, sinf(a) * 4.5f, 2.0f + 2.0f * (float)i));
        tb_scene_add_light(s, pos, tb_v3_scale(colors[(i + phase) % 4], mood.tree_light), 16.0f);
    }
    tb_scene_add_light(s, tb_v3(-20, -3, 2.5f), tb_v3(0.4f, 0.6f, 1.6f), 10.0f); /* LAN corner CRTs */
    tb_scene_add_light(s, tb_v3(27, -10, 2.0f), tb_v3(1.6f, 1.0f, 0.5f), 9.0f);  /* cocoa bar */
    tb_scene_add_light(s, tb_v3(0, -18, 3.0f), tb_v3(0.4f, 1.0f, 1.6f), 8.0f);   /* BIOS gate */
    tb_scene_add_light(s, tb_v3(0, 17, 7.0f), tb_v3(0.8f, 0.8f, 1.2f), 22.0f);   /* balcony */
    if (mood.emergency) {
        tb_scene_add_light(s, tb_v3(0, 0, 12), tb_v3(2.5f, 0.2f, 0.2f), 40.0f);
    }

    /* Open portals glow; today's door is gold. */
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        const tb_hall_door *d = &h->doors[i];
        if (!tb_portal_is_open(app->open, d->day)) {
            continue;
        }
        bool today = d->day == app->calendar_day;
        float pulse = 0.5f + 0.5f * sinf((float)app->sim.tick * 0.05f + (float)i);
        uint8_t c[4] = {(uint8_t)(today ? 255 : 80 + 60 * pulse), (uint8_t)(today ? 200 : 60 + 40 * pulse),
                        (uint8_t)(today ? 60 : 200 + 55 * pulse), 255};
        tb_vec3 lo = d->vol_min, hi = d->vol_max;
        float mid = lo.y + (hi.y - lo.y) * 0.5f;
        tb_scene_add_box(s, tb_v3(lo.x, mid - 0.05f, lo.z), tb_v3(hi.x, mid + 0.05f, hi.z), c, true);
    }
    /* 24 rings of bulbs: one ring lights up per bauble. */
    static const uint8_t lit[4][4] = {
        {255, 70, 60, 255}, {90, 255, 110, 255}, {255, 220, 90, 255}, {110, 170, 255, 255}};
    static const uint8_t dark[4] = {40, 45, 60, 255};
    for (int ring = 0; ring < TB_PORTAL_COUNT; ring++) {
        bool on = ring < baubles;
        for (int b = 0; b < TB_TREE_RING_BULBS; b++) {
            tb_vec3 p = tb_hall_ring_bulb_pos(h, ring, b);
            tb_scene_add_box(s, tb_v3_sub(p, tb_v3(0.07f, 0.07f, 0.07f)),
                             tb_v3_add(p, tb_v3(0.07f, 0.07f, 0.07f)),
                             on ? lit[(ring + b + (int)phase) % 4] : dark, on);
        }
    }
    if (tb_hall_act(app->calendar_day, app->save.completed) == TB_ACT_RESTORED) {
        static const uint8_t star[4] = {255, 230, 120, 255};
        tb_vec3 top = tb_v3_add(h->tree_base, tb_v3(0, 0, h->tree_height + 0.4f));
        tb_scene_add_box(s, tb_v3_sub(top, tb_v3(0.5f, 0.5f, 0.5f)), tb_v3_add(top, tb_v3(0.5f, 0.5f, 0.5f)),
                         star, true);
        tb_scene_add_light(s, top, tb_v3(2.0f, 1.8f, 1.0f), 18.0f);
    }
    /* Baubles on the tree, one per finished level. */
    static const uint8_t bc[4][4] = {
        {230, 30, 50, 255}, {255, 185, 40, 255}, {210, 220, 240, 255}, {60, 120, 255, 255}};
    for (int n = 0; n < TB_PORTAL_COUNT; n++) {
        if (app->save.completed & (1u << n)) {
            tb_vec3 p = tb_hall_bauble_pos(h, n);
            tb_scene_add_box(s, tb_v3_sub(p, tb_v3(0.22f, 0.22f, 0.22f)),
                             tb_v3_add(p, tb_v3(0.22f, 0.22f, 0.22f)), bc[n % 4], true);
        }
    }
}

#define LEVEL_LIGHTS 12 /* leaves room for muzzle and projectile lights */

/* The level's lights whose group is on, nearest to the camera first. */
static void level_scene(const tb_app *app, tb_scene *s)
{
    const tb_level_def *def = app->level;
    const tb_player *p = &app->sim.players[0];
    s->ambient = def->ambient;
    s->sun_color = def->sun_color;
    s->fog_color = def->fog_color;
    s->fog_start = def->fog_start;
    s->fog_end = def->fog_end;
    bool taken[TB_LEVEL_MAX_LIGHTS] = {false};
    for (int n = 0; n < LEVEL_LIGHTS; n++) {
        int best = -1;
        float best_d = 1e30f;
        for (int i = 0; i < def->light_count && i < TB_LEVEL_MAX_LIGHTS; i++) {
            const tb_level_light *l = &def->lights[i];
            float d = tb_v3_len(tb_v3_sub(l->pos, p->move.origin)) - l->radius;
            if (!taken[i] && tb_sim_light_on(&app->sim, l->group) && d < best_d) {
                best_d = d;
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        taken[best] = true;
        tb_scene_add_light(s, def->lights[best].pos, def->lights[best].color, def->lights[best].radius);
    }
}

void tb_world_scene(const tb_app *app, tb_scene *scene, float fov_h, float aspect)
{
    tb_scene_init_test_arena(scene);
    if (app->flow == TB_FLOW_HALL || app->flow == TB_FLOW_BRIEFING || !app->level) {
        tb_scene_reset_dynamic(scene, 0); /* the hall has its own lights */
        hall_scene(app, scene);
    } else {
        tb_scene_reset_dynamic(scene, 0);
        level_scene(app, scene);
    }
    tb_scene_from_sim(scene, &app->sim, 0, fov_h, aspect);
}
