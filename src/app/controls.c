/* SPDX-License-Identifier: MIT */
#include "app/controls.h"

#include "core/mathx.h"

#include <string.h>

#define PITCH_LIMIT 1.55f /* just short of straight up/down */

void tb_controls_init(tb_controls *c, float yaw)
{
    memset(c, 0, sizeof(*c));
    c->yaw = yaw;
    c->sensitivity = 0.0022f;
}

void tb_controls_key(tb_controls *c, tb_key key, bool down)
{
    if (key < 0 || key >= TB_KEY_COUNT) {
        return;
    }
    if (down && !c->down[key] && key >= TB_KEY_WEAPON1 && key <= TB_KEY_WEAPON9) {
        c->pending_weapon = (uint8_t)(key - TB_KEY_WEAPON1 + 1);
    }
    c->down[key] = down;
}

void tb_controls_mouse(tb_controls *c, float dx, float dy, float scale)
{
    c->yaw = tb_wrap_anglef(c->yaw - dx * c->sensitivity * scale);
    c->pitch = tb_clampf(c->pitch - dy * c->sensitivity * scale, -PITCH_LIMIT, PITCH_LIMIT);
}

void tb_controls_release_all(tb_controls *c)
{
    memset(c->down, 0, sizeof(c->down));
}

tb_input tb_controls_input(tb_controls *c)
{
    static const struct {
        tb_key key;
        uint16_t button;
    } map[] = {{TB_KEY_JUMP, TB_BTN_JUMP},         {TB_KEY_CROUCH, TB_BTN_CROUCH},
               {TB_KEY_SPRINT, TB_BTN_SPRINT},     {TB_KEY_FIRE, TB_BTN_FIRE},
               {TB_KEY_ALT_FIRE, TB_BTN_ALT_FIRE}, {TB_KEY_USE, TB_BTN_USE},
               {TB_KEY_MELEE, TB_BTN_MELEE}};
    tb_input in;
    memset(&in, 0, sizeof(in));
    in.forward = (int8_t)((c->down[TB_KEY_FORWARD] ? 127 : 0) - (c->down[TB_KEY_BACK] ? 127 : 0));
    in.side = (int8_t)((c->down[TB_KEY_RIGHT] ? 127 : 0) - (c->down[TB_KEY_LEFT] ? 127 : 0));
    for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
        if (c->down[map[i].key]) {
            in.buttons |= map[i].button;
        }
    }
    in.yaw = tb_angle_to_short(c->yaw);
    in.pitch = tb_angle_to_short(c->pitch);
    in.weapon = c->pending_weapon;
    c->pending_weapon = 0;
    return in;
}
