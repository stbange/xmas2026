/* SPDX-License-Identifier: MIT */
/* One player's input for one simulation tick. Device-independent: the
 * input layer (keyboard, gamepad, touch, XR) produces this; replays and the
 * network transmit it. */
#ifndef TB_SIM_INPUT_H
#define TB_SIM_INPUT_H

#include "core/types.h"

enum {
    TB_BTN_JUMP = 1u << 0,
    TB_BTN_CROUCH = 1u << 1,
    TB_BTN_SPRINT = 1u << 2,
    TB_BTN_FIRE = 1u << 3,
    TB_BTN_ALT_FIRE = 1u << 4,
    TB_BTN_USE = 1u << 5,
    TB_BTN_MELEE = 1u << 6,
    TB_BTN_ABILITY1 = 1u << 7,
    TB_BTN_ABILITY2 = 1u << 8
};

typedef struct tb_input {
    int8_t forward;   /* -127..127 */
    int8_t side;      /* -127..127, positive = right */
    int8_t up;        /* flight/swim, -127..127 */
    uint8_t weapon;   /* 0 = keep, 1..9 = select slot */
    uint16_t buttons; /* TB_BTN_* */
    int16_t yaw;      /* absolute view yaw, 65536 = full turn */
    int16_t pitch;    /* absolute view pitch, 65536 = full turn */
} tb_input;

/* Angle conversions between radians and the 16-bit network/replay format. */
int16_t tb_angle_to_short(float radians);
float tb_short_to_angle(int16_t a);

#endif
