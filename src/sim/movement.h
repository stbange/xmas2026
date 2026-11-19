/* SPDX-License-Identifier: MIT */
/* Player movement (Quake-style): ground friction and acceleration, air
 * control, jumping, crouching, sprinting, stepping up stairs and sliding
 * along walls. Deterministic given the same inputs. */
#ifndef TB_SIM_MOVEMENT_H
#define TB_SIM_MOVEMENT_H

#include "sim/geometry.h"
#include "sim/input.h"

typedef struct tb_move_params {
    float walk_speed;     /* m/s */
    float sprint_factor;  /* x walk speed */
    float crouch_factor;  /* x walk speed */
    float accelerate;     /* ground acceleration (1/s) */
    float air_accelerate; /* air acceleration (1/s) */
    float air_wish_cap;   /* max wish speed for air acceleration (m/s): enables strafe jumping */
    float friction;
    float stop_speed;
    float gravity;     /* m/s^2 */
    float jump_speed;  /* m/s */
    float step_height; /* m */
    float max_fall_speed;
} tb_move_params;

#define TB_PLAYER_WIDTH         0.6f
#define TB_PLAYER_HEIGHT        1.8f
#define TB_PLAYER_CROUCH_HEIGHT 1.0f
#define TB_PLAYER_EYE           1.6f
#define TB_PLAYER_CROUCH_EYE    0.8f
#define TB_MIN_WALK_NORMAL      0.7f

typedef struct tb_move_state {
    tb_vec3 origin; /* feet centre */
    tb_vec3 velocity;
    float yaw, pitch; /* radians */
    bool on_ground;
    bool crouched;
    bool jump_held; /* jump must be released before the next jump */
    bool jumping;   /* rising from a jump (not from walking up a slope) */
    tb_vec3 ground_normal;
} tb_move_state;

void tb_move_default_params(tb_move_params *p);
void tb_move_hull(bool crouched, tb_vec3 *mins, tb_vec3 *maxs);
float tb_move_eye_height(const tb_move_state *s);
/* Advances the state by dt seconds using one tick of input. */
void tb_move_step(tb_move_state *s, const tb_input *in, const tb_move_params *p, const tb_geometry *g,
                  float dt);
/* Horizontal speed (m/s). */
float tb_move_ground_speed(const tb_move_state *s);

#endif
