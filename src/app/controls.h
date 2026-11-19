/* SPDX-License-Identifier: MIT */
/* Keyboard + mouse controls turned into one tb_input per tick. The
 * platform maps physical keys to tb_key; this module owns the bindings and
 * the view angles. (Gamepad, touch and XR follow in the input group.) */
#ifndef TB_APP_CONTROLS_H
#define TB_APP_CONTROLS_H

#include "sim/input.h"

typedef enum tb_key {
    TB_KEY_FORWARD = 0,
    TB_KEY_BACK,
    TB_KEY_LEFT,
    TB_KEY_RIGHT,
    TB_KEY_JUMP,
    TB_KEY_CROUCH,
    TB_KEY_SPRINT,
    TB_KEY_FIRE,
    TB_KEY_ALT_FIRE,
    TB_KEY_USE,
    TB_KEY_MELEE,
    TB_KEY_WEAPON1, /* ... TB_KEY_WEAPON9 follow */
    TB_KEY_WEAPON9 = TB_KEY_WEAPON1 + 8,
    TB_KEY_COUNT
} tb_key;

typedef struct tb_controls {
    bool down[TB_KEY_COUNT];
    float yaw, pitch;       /* radians */
    float sensitivity;      /* radians per mouse count at sensitivity 1 */
    uint8_t pending_weapon; /* 1..9, sent once */
} tb_controls;

void tb_controls_init(tb_controls *c, float yaw);
void tb_controls_key(tb_controls *c, tb_key key, bool down);
/* Mouse motion in counts; `scale` is the player's sensitivity setting. */
void tb_controls_mouse(tb_controls *c, float dx, float dy, float scale);
/* Releases all keys (e.g. when the window loses focus). */
void tb_controls_release_all(tb_controls *c);
/* Builds this tick's input. */
tb_input tb_controls_input(tb_controls *c);

#endif
