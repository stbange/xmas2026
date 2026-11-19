/* SPDX-License-Identifier: MIT */
#include "render/scene.h"

#include <string.h>

void tb_scene_init_test_arena(tb_scene *s)
{
    memset(s, 0, sizeof(*s));
    s->ambient = tb_v3(0.20f, 0.22f, 0.32f);
    s->sun_dir = tb_v3_normalize(tb_v3(0.4f, 0.6f, -1.0f));
    s->sun_color = tb_v3(0.55f, 0.6f, 0.85f); /* moonlight */
    s->fog_color = tb_v3(0.04f, 0.05f, 0.10f);
    s->fog_start = 12.0f;
    s->fog_end = 45.0f;
    s->camera.fov_y = 1.2f;
    s->camera.near_z = 0.05f;
    s->camera.far_z = 200.0f;
    /* Tree lights: red, green, gold. */
    tb_scene_add_light(s, tb_v3(-10, 0, 4), tb_v3(2.0f, 0.3f, 0.2f), 14.0f);
    tb_scene_add_light(s, tb_v3(10, 10, 4), tb_v3(0.3f, 1.8f, 0.4f), 14.0f);
    tb_scene_add_light(s, tb_v3(0, -12, 3), tb_v3(1.8f, 1.4f, 0.4f), 12.0f);
}

bool tb_scene_add_light(tb_scene *s, tb_vec3 pos, tb_vec3 color, float radius)
{
    if (s->light_count >= TB_SCENE_MAX_LIGHTS) {
        return false;
    }
    tb_light *l = &s->lights[s->light_count++];
    l->pos = pos;
    l->color = color;
    l->radius = radius;
    return true;
}

bool tb_scene_add_box(tb_scene *s, tb_vec3 mins, tb_vec3 maxs, const uint8_t color[4], bool emissive)
{
    if (s->box_count >= TB_SCENE_MAX_BOXES) {
        return false;
    }
    tb_box *b = &s->boxes[s->box_count++];
    b->mins = mins;
    b->maxs = maxs;
    memcpy(b->color, color, 4);
    b->emissive = emissive;
    return true;
}

void tb_scene_reset_dynamic(tb_scene *s, int static_lights)
{
    s->box_count = 0;
    if (static_lights < s->light_count) {
        s->light_count = static_lights < 0 ? 0 : static_lights;
    }
}

void tb_scene_from_sim(tb_scene *s, const tb_sim *sim, int player, float fov_h, float aspect)
{
    static const uint8_t c_target[4] = {230, 230, 240, 255};  /* snowman white */
    static const uint8_t c_stunned[4] = {120, 200, 255, 255}; /* frozen blue */
    static const uint8_t c_pickup[4] = {255, 210, 60, 255};   /* gold */
    static const uint8_t c_bauble[4] = {255, 40, 60, 255};    /* bauble red */
    static const uint8_t c_shot[4] = {255, 255, 255, 255};
    static const uint8_t c_player[4] = {60, 160, 90, 255}; /* teammates: green */

    const tb_player *p = &sim->players[player];
    s->camera.eye = tb_v3_add(p->move.origin, tb_v3(0, 0, tb_move_eye_height(&p->move)));
    s->camera.yaw = p->move.yaw;
    s->camera.pitch = p->move.pitch;
    s->camera.fov_y = tb_fov_horizontal_to_vertical(fov_h, aspect);

    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        const tb_player *q = &sim->players[i];
        if (i == player || !q->active) {
            continue;
        }
        tb_vec3 mins, maxs;
        tb_move_hull(q->move.crouched, &mins, &maxs);
        tb_scene_add_box(s, tb_v3_add(q->move.origin, mins), tb_v3_add(q->move.origin, maxs), c_player,
                         false);
    }
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        const tb_entity *e = &sim->entities[i];
        tb_vec3 lo = tb_v3_add(e->origin, e->mins), hi = tb_v3_add(e->origin, e->maxs);
        switch ((tb_entity_type)e->type) {
        case TB_ENT_TARGET:
            tb_scene_add_box(s, lo, hi, e->stun_ticks ? c_stunned : c_target, false);
            break;
        case TB_ENT_PICKUP:
            tb_scene_add_box(s, lo, hi, e->kind == TB_PICKUP_BAUBLE ? c_bauble : c_pickup, true);
            break;
        case TB_ENT_PROJECTILE:
            tb_scene_add_box(s, lo, hi, c_shot, true);
            tb_scene_add_light(s, e->origin, tb_v3(0.8f, 0.9f, 1.2f), 4.0f);
            break;
        default:
            break; /* triggers are invisible */
        }
    }
}
