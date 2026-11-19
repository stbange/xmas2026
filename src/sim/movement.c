/* SPDX-License-Identifier: MIT */
#include "sim/movement.h"

#include <math.h>

#define MAX_CLIP_PLANES 5
#define OVERCLIP        1.001f
#define GROUND_PROBE    0.06f

int16_t tb_angle_to_short(float radians)
{
    float turns = tb_wrap_anglef(radians) / (2.0f * TB_PI); /* [-0.5, 0.5) */
    long v = lroundf(turns * 65536.0f);
    if (v > 32767) {
        v -= 65536;
    }
    return (int16_t)v;
}

float tb_short_to_angle(int16_t a)
{
    return (float)a * (2.0f * TB_PI / 65536.0f);
}

void tb_move_default_params(tb_move_params *p)
{
    p->walk_speed = 8.0f;
    p->sprint_factor = 1.35f;
    p->crouch_factor = 0.5f;
    p->accelerate = 10.0f;
    p->air_accelerate = 10.0f;
    p->air_wish_cap = 1.0f;
    p->friction = 6.0f;
    p->stop_speed = 2.5f;
    p->gravity = 20.0f;
    p->jump_speed = 6.0f;
    p->step_height = 0.45f;
    p->max_fall_speed = 60.0f;
}

void tb_move_hull(bool crouched, tb_vec3 *mins, tb_vec3 *maxs)
{
    const float h = TB_PLAYER_WIDTH * 0.5f;
    *mins = tb_v3(-h, -h, 0.0f);
    *maxs = tb_v3(h, h, crouched ? TB_PLAYER_CROUCH_HEIGHT : TB_PLAYER_HEIGHT);
}

float tb_move_eye_height(const tb_move_state *s)
{
    return s->crouched ? TB_PLAYER_CROUCH_EYE : TB_PLAYER_EYE;
}

float tb_move_ground_speed(const tb_move_state *s)
{
    return sqrtf(s->velocity.x * s->velocity.x + s->velocity.y * s->velocity.y);
}

static tb_vec3 clip_velocity(tb_vec3 v, tb_vec3 n, float overbounce)
{
    float backoff = tb_v3_dot(v, n);
    backoff = backoff < 0 ? backoff * overbounce : backoff / overbounce;
    return tb_v3_sub(v, tb_v3_scale(n, backoff));
}

static tb_trace trace_hull(const tb_move_state *s, const tb_geometry *g, tb_vec3 from, tb_vec3 to)
{
    tb_vec3 mins, maxs;
    tb_move_hull(s->crouched, &mins, &maxs);
    return tb_trace_box(g, from, to, mins, maxs, TB_CONTENTS_SOLID | TB_CONTENTS_PLAYERCLIP);
}

/* Moves along the velocity, sliding along up to MAX_CLIP_PLANES surfaces.
 * Returns true if anything was hit. */
static bool slide_move(tb_move_state *s, const tb_geometry *g, float dt)
{
    tb_vec3 planes[MAX_CLIP_PLANES];
    int num_planes = 0;
    float time_left = dt;
    bool blocked = false;
    tb_vec3 primal = s->velocity;

    for (int bump = 0; bump < 4 && time_left > 0.0f; bump++) {
        tb_vec3 end = tb_v3_add(s->origin, tb_v3_scale(s->velocity, time_left));
        tb_trace tr = trace_hull(s, g, s->origin, end);
        if (tr.all_solid) {
            s->velocity.z = 0;
            return true;
        }
        s->origin = tr.end;
        if (tr.fraction >= 1.0f) {
            break;
        }
        blocked = true;
        time_left -= time_left * tr.fraction;
        if (num_planes >= MAX_CLIP_PLANES) {
            s->velocity = tb_v3(0, 0, 0);
            return true;
        }
        planes[num_planes++] = tr.normal;

        /* Find a velocity that is valid against all planes so far. */
        int i;
        for (i = 0; i < num_planes; i++) {
            s->velocity = clip_velocity(s->velocity, planes[i], OVERCLIP);
            int j;
            for (j = 0; j < num_planes; j++) {
                if (j != i && tb_v3_dot(s->velocity, planes[j]) < 0) {
                    break;
                }
            }
            if (j == num_planes) {
                break;
            }
        }
        if (i == num_planes) {
            /* Wedged between planes: slide along their crease. */
            if (num_planes != 2) {
                s->velocity = tb_v3(0, 0, 0);
                return true;
            }
            tb_vec3 dir = tb_v3_normalize(tb_v3_cross(planes[0], planes[1]));
            s->velocity = tb_v3_scale(dir, tb_v3_dot(dir, s->velocity));
        }
        if (tb_v3_dot(s->velocity, primal) <= 0) {
            s->velocity = tb_v3(0, 0, 0);
            break;
        }
    }
    return blocked;
}

/* Slide, and if blocked on the ground, try again from step height up. */
static void step_slide_move(tb_move_state *s, const tb_geometry *g, const tb_move_params *p, float dt)
{
    tb_move_state start = *s;
    if (!slide_move(s, g, dt) || !start.on_ground) {
        return;
    }
    tb_move_state down = *s;

    tb_move_state up = start;
    tb_trace tr = trace_hull(&up, g, up.origin, tb_v3_add(up.origin, tb_v3(0, 0, p->step_height)));
    if (tr.all_solid) {
        return;
    }
    float stepped = tr.end.z - up.origin.z;
    up.origin = tr.end;
    slide_move(&up, g, dt);
    tr = trace_hull(&up, g, up.origin, tb_v3_sub(up.origin, tb_v3(0, 0, stepped + GROUND_PROBE)));
    if (tr.fraction < 1.0f && tr.normal.z < TB_MIN_WALK_NORMAL) {
        return; /* would land on something too steep */
    }
    up.origin = tr.end;

    float dist_down = (down.origin.x - start.origin.x) * (down.origin.x - start.origin.x) +
                      (down.origin.y - start.origin.y) * (down.origin.y - start.origin.y);
    float dist_up = (up.origin.x - start.origin.x) * (up.origin.x - start.origin.x) +
                    (up.origin.y - start.origin.y) * (up.origin.y - start.origin.y);
    if (dist_up > dist_down) {
        s->origin = up.origin;
        s->velocity.x = up.velocity.x;
        s->velocity.y = up.velocity.y;
    }
}

static void categorize_ground(tb_move_state *s, const tb_geometry *g)
{
    if (s->jumping && s->velocity.z > 0.0f) { /* still rising from a jump */
        s->on_ground = false;
        return;
    }
    s->jumping = false;
    tb_trace tr = trace_hull(s, g, s->origin, tb_v3_sub(s->origin, tb_v3(0, 0, GROUND_PROBE)));
    s->on_ground = tr.fraction < 1.0f && tr.normal.z >= TB_MIN_WALK_NORMAL;
    if (s->on_ground) {
        s->ground_normal = tr.normal;
        s->origin = tr.end;
        if (s->velocity.z < 0) {
            s->velocity.z = 0;
        }
    }
}

static void apply_friction(tb_move_state *s, const tb_move_params *p, float dt)
{
    float speed = tb_move_ground_speed(s);
    if (speed < 0.01f) {
        s->velocity.x = 0;
        s->velocity.y = 0;
        return;
    }
    float control = speed < p->stop_speed ? p->stop_speed : speed;
    float new_speed = speed - control * p->friction * dt;
    if (new_speed < 0) {
        new_speed = 0;
    }
    float scale = new_speed / speed;
    s->velocity.x *= scale;
    s->velocity.y *= scale;
}

static void accelerate(tb_move_state *s, tb_vec3 wish_dir, float wish_speed, float accel, float dt)
{
    float current = tb_v3_dot(s->velocity, wish_dir);
    float add = wish_speed - current;
    if (add <= 0) {
        return;
    }
    float amount = accel * dt * wish_speed;
    if (amount > add) {
        amount = add;
    }
    s->velocity = tb_v3_add(s->velocity, tb_v3_scale(wish_dir, amount));
}

static void update_crouch(tb_move_state *s, const tb_input *in, const tb_geometry *g)
{
    bool want = (in->buttons & TB_BTN_CROUCH) != 0;
    if (want) {
        s->crouched = true;
    } else if (s->crouched) {
        tb_vec3 mins, maxs;
        tb_move_hull(false, &mins, &maxs);
        tb_trace tr =
            tb_trace_box(g, s->origin, s->origin, mins, maxs, TB_CONTENTS_SOLID | TB_CONTENTS_PLAYERCLIP);
        if (!tr.start_solid) {
            s->crouched = false; /* only stand up when there is room */
        }
    }
}

void tb_move_step(tb_move_state *s, const tb_input *in, const tb_move_params *p, const tb_geometry *g,
                  float dt)
{
    s->yaw = tb_short_to_angle(in->yaw);
    s->pitch = tb_short_to_angle(in->pitch);
    update_crouch(s, in, g);
    categorize_ground(s, g);

    if (!(in->buttons & TB_BTN_JUMP)) {
        s->jump_held = false;
    } else if (s->on_ground && !s->jump_held) {
        s->velocity.z = p->jump_speed;
        s->on_ground = false;
        s->jump_held = true;
        s->jumping = true;
    }

    tb_vec3 fwd = tb_v3(cosf(s->yaw), sinf(s->yaw), 0);
    tb_vec3 right = tb_v3(sinf(s->yaw), -cosf(s->yaw), 0);
    float fm = (float)in->forward / 127.0f, sm = (float)in->side / 127.0f;
    tb_vec3 wish = tb_v3_add(tb_v3_scale(fwd, fm), tb_v3_scale(right, sm));
    float wish_len = tb_v3_len(wish);
    tb_vec3 wish_dir = tb_v3_normalize(wish);
    float max_speed = p->walk_speed;
    if (s->crouched) {
        max_speed *= p->crouch_factor;
    } else if (in->buttons & TB_BTN_SPRINT) {
        max_speed *= p->sprint_factor;
    }
    float wish_speed = max_speed * (wish_len > 1.0f ? 1.0f : wish_len);

    if (s->on_ground) {
        apply_friction(s, p, dt);
        accelerate(s, wish_dir, wish_speed, p->accelerate, dt);
        /* Keep walking along slopes instead of launching off them. */
        s->velocity = clip_velocity(s->velocity, s->ground_normal, OVERCLIP);
        step_slide_move(s, g, p, dt);
    } else {
        float capped = wish_speed > p->air_wish_cap ? p->air_wish_cap : wish_speed;
        /* Quake air control: acceleration uses the full wish speed, the
         * gain is limited by the capped speed (enables strafe-jumping). */
        float current = tb_v3_dot(s->velocity, wish_dir);
        float add = capped - current;
        if (add > 0) {
            float amount = p->air_accelerate * wish_speed * dt;
            s->velocity = tb_v3_add(s->velocity, tb_v3_scale(wish_dir, amount > add ? add : amount));
        }
        s->velocity.z -= p->gravity * dt;
        if (s->velocity.z < -p->max_fall_speed) {
            s->velocity.z = -p->max_fall_speed;
        }
        slide_move(s, g, dt);
    }
    categorize_ground(s, g);
}
