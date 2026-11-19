/* SPDX-License-Identifier: MIT */
/* What to draw this frame, built from the simulation without touching the
 * GPU: camera, lights and dynamic boxes (targets, pickups, projectiles). */
#ifndef TB_RENDER_SCENE_H
#define TB_RENDER_SCENE_H

#include "render/camera.h"
#include "sim/sim.h"

#define TB_SCENE_MAX_LIGHTS 16
#define TB_SCENE_MAX_BOXES  256

typedef struct tb_light {
    tb_vec3 pos;
    tb_vec3 color; /* linear, may exceed 1 */
    float radius;  /* metres; light reaches zero here */
} tb_light;

typedef struct tb_box {
    tb_vec3 mins, maxs;
    uint8_t color[4];
    bool emissive; /* ignores lighting ("fullbright") */
} tb_box;

typedef struct tb_scene {
    tb_camera camera;
    tb_vec3 ambient;
    tb_vec3 sun_dir; /* direction the sunlight travels (normalised) */
    tb_vec3 sun_color;
    tb_vec3 fog_color;
    float fog_start, fog_end;
    tb_light lights[TB_SCENE_MAX_LIGHTS];
    int light_count;
    tb_box boxes[TB_SCENE_MAX_BOXES];
    int box_count;
} tb_scene;

/* Default lighting for the test arena: moonlight, cold ambient, coloured
 * festive point lights. */
void tb_scene_init_test_arena(tb_scene *s);
bool tb_scene_add_light(tb_scene *s, tb_vec3 pos, tb_vec3 color, float radius);
bool tb_scene_add_box(tb_scene *s, tb_vec3 mins, tb_vec3 maxs, const uint8_t color[4], bool emissive);
/* Fills the camera from a player's view and adds boxes and lights for the
 * entities. Static lights already in the scene are kept; call
 * tb_scene_reset_dynamic first each frame. */
void tb_scene_from_sim(tb_scene *s, const tb_sim *sim, int player, float fov_h_radians, float aspect);
/* Removes dynamic boxes and lights added after `static_lights`. */
void tb_scene_reset_dynamic(tb_scene *s, int static_lights);

#endif
