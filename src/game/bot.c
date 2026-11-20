/* SPDX-License-Identifier: MIT */
#include "game/bot.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define ARRIVE     0.5f
#define ARRIVE_JET 1.5f

void tb_bot_init(tb_bot *bot, const tb_bot_step *steps, int count)
{
    memset(bot, 0, sizeof(*bot));
    bot->steps = steps;
    bot->count = count;
}

static void next(tb_bot *bot)
{
    bot->index++;
    bot->step_ticks = 0;
    bot->pressed = false;
    if (bot->index >= bot->count) {
        bot->done = true;
    }
}

static void fail(tb_bot *bot, const char *why)
{
    bot->failed = true;
    snprintf(bot->reason, sizeof(bot->reason), "step %d: %s", bot->index, why);
}

/* The nearest visible living enemy within range, or NULL. */
static tb_entity *visible_enemy(tb_sim *sim, tb_vec3 eye)
{
    tb_entity *best = NULL;
    float best_d = TB_BOT_FIGHT_RANGE;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type != TB_ENT_ENEMY) {
            continue;
        }
        float d = tb_v3_len(tb_v3_sub(e->origin, eye));
        if (d < best_d &&
            tb_trace_ray(sim->cfg.geometry, eye, e->origin, TB_CONTENTS_SOLID).fraction >= 1.0f) {
            best_d = d;
            best = e;
        }
    }
    return best;
}

/* The living enemy nearest to `at` within the clear radius, or NULL. */
static tb_entity *enemy_near(tb_sim *sim, tb_vec3 at)
{
    tb_entity *best = NULL;
    float best_d = TB_BOT_CLEAR_RADIUS;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        float d = tb_v3_len(tb_v3_sub(e->origin, at));
        if (e->type == TB_ENT_ENEMY && d < best_d) {
            best_d = d;
            best = e;
        }
    }
    return best;
}

static void aim_at(tb_input *in, tb_vec3 from, tb_vec3 to)
{
    tb_vec3 d = tb_v3_sub(to, from);
    in->yaw = tb_angle_to_short(atan2f(d.y, d.x));
    in->pitch = tb_angle_to_short(atan2f(d.z, sqrtf(d.x * d.x + d.y * d.y)));
}

/* Moves towards `goal` regardless of where the bot is looking. */
static void steer(tb_input *in, tb_vec3 from, tb_vec3 goal, float amount)
{
    float want = atan2f(goal.y - from.y, goal.x - from.x);
    float rel = want - tb_short_to_angle(in->yaw);
    in->forward = (int8_t)(127.0f * amount * cosf(rel));
    in->side = (int8_t)(-127.0f * amount * sinf(rel));
}

static float flat_dist(tb_vec3 a, tb_vec3 b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

tb_input tb_bot_think(tb_bot *bot, tb_sim *sim, int slot)
{
    tb_input in;
    memset(&in, 0, sizeof(in));
    tb_player *p = &sim->players[slot];
    if (bot->done || bot->failed) {
        return in;
    }
    if (p->deaths != bot->deaths_seen) {
        if (p->deaths >= TB_BOT_MAX_DEATHS) {
            fail(bot, "died too often");
            return in;
        }
        bot->deaths_seen = p->deaths; /* respawned: go back to the last checkpoint step */
        bot->index = bot->resume;
        bot->step_ticks = 0;
    }
    if (p->state != TB_PLAYER_ALIVE) {
        return in;
    }
    const tb_bot_step *s = &bot->steps[bot->index];
    tb_vec3 eye = tb_v3_add(p->move.origin, tb_v3(0, 0, tb_move_eye_height(&p->move)));
    in.yaw = tb_angle_to_short(p->move.yaw);
    if (++bot->step_ticks > (uint32_t)(s->op == TB_BOT_CLEAR ? s->arg : (int)TB_BOT_STEP_TIMEOUT)) {
        fail(bot, "timed out");
        return in;
    }

    /* Fight whatever is in sight, on every step. */
    tb_entity *enemy = visible_enemy(sim, eye);
    if (enemy) {
        aim_at(&in, eye, enemy->origin);
        in.buttons |= (sim->tick & 1u) ? TB_BTN_FIRE : 0; /* pump the trigger */
    }

    switch (s->op) {
    case TB_BOT_GOTO:
    case TB_BOT_JET_TO:
        if (s->op == TB_BOT_GOTO ? flat_dist(p->move.origin, s->at) < ARRIVE
                                 : (flat_dist(p->move.origin, s->at) < ARRIVE_JET && p->move.on_ground &&
                                    p->move.origin.z > s->at.z - 0.1f)) {
            next(bot);
            break;
        }
        if (!enemy) {
            aim_at(&in, eye, tb_v3(s->at.x, s->at.y, eye.z));
        }
        if (s->op == TB_BOT_GOTO) {
            steer(&in, p->move.origin, s->at, 1.0f);
        } else {
            /* Jet: climb first, then fly over, slowing down on approach. */
            float d = flat_dist(p->move.origin, s->at);
            if (p->move.origin.z > s->at.z + 0.3f) {
                steer(&in, p->move.origin, s->at, d < 4.0f ? d / 4.0f : 1.0f);
            }
            if ((p->move.origin.z < s->at.z + 1.5f && d >= ARRIVE_JET) || p->move.origin.z < s->at.z - 0.1f) {
                in.buttons |= TB_BTN_ABILITY1;
            }
        }
        break;
    case TB_BOT_USE:
        aim_at(&in, eye, s->at);
        in.buttons &= (uint16_t)~TB_BTN_FIRE;
        if (bot->step_ticks > 2 && !bot->pressed) {
            in.buttons |= TB_BTN_USE;
            bot->pressed = true;
        } else if (bot->pressed) {
            next(bot);
        }
        break;
    case TB_BOT_WAIT:
        if ((int)bot->step_ticks >= s->arg) {
            next(bot);
        }
        break;
    case TB_BOT_SELECT:
        in.weapon = (uint8_t)s->arg;
        next(bot);
        break;
    case TB_BOT_CLEAR: {
        tb_entity *left = enemy_near(sim, s->at);
        if (!left) {
            next(bot);
        } else if (!enemy) {
            /* Walk towards it until it is in sight. */
            aim_at(&in, eye, left->origin);
            steer(&in, p->move.origin, left->origin, 1.0f);
        }
        break;
    }
    default: /* TB_BOT_CHECKPOINT */
        bot->resume = bot->index;
        next(bot);
        break;
    }
    return in;
}
