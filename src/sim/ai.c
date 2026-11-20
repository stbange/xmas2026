/* SPDX-License-Identifier: MIT */
/* Enemies: definitions and a small, deterministic behaviour model (see,
 * chase, keep distance, shoot; teleporting, flying and mortar variants). */
#include "sim/sim_internal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define RETARGET_TICKS 15u
#define GRAVITY        20.0f

static const tb_enemy_def g_enemies[TB_ENEMY_KIND_COUNT] = {
    [TB_ENEMY_TROOPER] = {.name = "Hogwash Trooper",
                          .health = 60,
                          .speed = 4.5f,
                          .half = {0.4f, 0.4f, 0.95f},
                          .hitscan = true,
                          .damage = 4,
                          .pellets = 4,
                          .refire_ticks = 100,
                          .range = 22.0f,
                          .sight = 30.0f,
                          .color = {200, 120, 140, 255}},
    [TB_ENEMY_LIZARD] = {.name = "Elf-Lizard",
                         .health = 45,
                         .speed = 5.0f,
                         .half = {0.35f, 0.35f, 0.9f},
                         .damage = 12,
                         .pellets = 1,
                         .refire_ticks = 70,
                         .range = 25.0f,
                         .projectile_speed = 16.0f,
                         .teleport_ticks = 300,
                         .sight = 30.0f,
                         .color = {90, 200, 90, 255}},
    [TB_ENEMY_BRAIN] = {.name = "Brain-bauble",
                        .health = 50,
                        .speed = 4.0f,
                        .flying = true,
                        .half = {0.5f, 0.5f, 0.5f},
                        .damage = 10,
                        .pellets = 1,
                        .refire_ticks = 80,
                        .range = 24.0f,
                        .projectile_speed = 14.0f,
                        .sight = 30.0f,
                        .color = {220, 120, 255, 255}},
    [TB_ENEMY_BATTLELORD] = {.name = "Bargain-Bin Battlelord",
                             .health = 600,
                             .speed = 3.5f,
                             .half = {0.9f, 0.9f, 1.6f},
                             .shrink_immune = true,
                             .hitscan = true,
                             .damage = 3,
                             .pellets = 1,
                             .refire_ticks = 6,
                             .range = 30.0f,
                             .mortar_ticks = 150,
                             .mortar_damage = 45,
                             .sight = 50.0f,
                             .color = {220, 40, 40, 255}},
};

const tb_enemy_def *tb_enemy_get(tb_enemy_kind kind)
{
    return (kind >= 0 && kind < TB_ENEMY_KIND_COUNT) ? &g_enemies[kind] : NULL;
}

static void sync_origin(tb_entity *e)
{
    e->origin = tb_v3_add(e->move.origin, tb_v3(0, 0, e->base_half.z));
}

tb_handle tb_sim_add_enemy(tb_sim *sim, tb_enemy_kind kind, tb_vec3 feet, const char *name)
{
    const tb_enemy_def *def = tb_enemy_get(kind);
    if (!def) {
        return 0;
    }
    feet.z += 0.01f; /* clear of the floor so movement traces do not start solid */
    tb_handle h = tb_sim_spawn(sim, TB_ENT_ENEMY, feet, def->half);
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->kind = (uint8_t)kind;
        e->base_half = def->half;
        e->vitals.health = def->health;
        e->move.origin = feet;
        e->move.ground_normal = tb_v3(0, 0, 1);
        e->ai_timer = (uint32_t)tb__index(sim, e) % RETARGET_TICKS; /* stagger thinking */
        e->cooldown = def->refire_ticks;
        e->cooldown2 = def->teleport_ticks ? def->teleport_ticks : def->mortar_ticks;
        snprintf(e->name, sizeof(e->name), "%s", name ? name : "");
        sync_origin(e);
    }
    return h;
}

int tb_sim_enemy_count(const tb_sim *sim)
{
    int n = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        n += sim->entities[i].type == TB_ENT_ENEMY;
    }
    return n;
}

static tb_vec3 enemy_eye(const tb_entity *e)
{
    return tb_v3_add(e->origin, tb_v3(0, 0, e->maxs.z * 0.6f));
}

/* Where the enemy aims: player chest or decoy centre. */
static bool target_pos(tb_sim *sim, const tb_entity *e, tb_vec3 *out, tb_entity **decoy)
{
    *decoy = NULL;
    if (e->ai_target == -2) {
        for (int i = 0; i < TB_MAX_ENTITIES; i++) {
            if (sim->entities[i].type == TB_ENT_DECOY) {
                *decoy = &sim->entities[i];
                *out = (*decoy)->origin;
                return true;
            }
        }
        return false;
    }
    if (e->ai_target < 0) {
        return false;
    }
    const tb_player *p = &sim->players[e->ai_target];
    if (!p->active || p->state != TB_PLAYER_ALIVE) {
        return false;
    }
    *out = tb_v3_add(p->move.origin, tb_v3(0, 0, p->move.shrunk ? 0.3f : 1.0f));
    return true;
}

static void choose_target(tb_sim *sim, tb_entity *e, const tb_enemy_def *def)
{
    tb_vec3 eye = enemy_eye(e);
    float best = def->sight;
    int target = -1;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        const tb_entity *d = &sim->entities[i];
        if (d->type == TB_ENT_DECOY) {
            float dist = tb_v3_len(tb_v3_sub(d->origin, eye));
            if (dist < best && tb__line_of_sight(sim, eye, d->origin)) {
                best = dist;
                target = -2; /* holograms are irresistible */
            }
        }
    }
    if (target == -1) {
        for (int i = 0; i < TB_MAX_PLAYERS; i++) {
            const tb_player *p = &sim->players[i];
            if (!p->active || p->state != TB_PLAYER_ALIVE) {
                continue;
            }
            tb_vec3 chest = tb_v3_add(p->move.origin, tb_v3(0, 0, 1.0f));
            float dist = tb_v3_len(tb_v3_sub(chest, eye));
            if (dist < best && tb__line_of_sight(sim, eye, chest)) {
                best = dist;
                target = i;
            }
        }
    }
    if (target != -1) {
        e->ai_target = (int8_t)target;
        e->ai_state = TB_AI_CHASE;
    } else if (e->ai_state == TB_AI_CHASE && e->ai_target == -2) {
        e->ai_target = -1; /* the decoy vanished */
    }
}

static void hit_target(tb_sim *sim, tb_entity *e, tb_entity *decoy, int32_t damage)
{
    if (decoy) {
        decoy->type = TB_ENT_NONE;
    } else {
        tb_sim_damage_player(sim, e->ai_target, damage);
    }
}

static void attack(tb_sim *sim, tb_entity *e, const tb_enemy_def *def, tb_vec3 aim, tb_entity *decoy)
{
    tb_vec3 eye = enemy_eye(e);
    tb_vec3 to = tb_v3_sub(aim, eye);
    float dist = tb_v3_len(to);
    if (e->cooldown == 0 && dist <= def->range) {
        e->cooldown = def->refire_ticks;
        if (def->hitscan) {
            float chance = tb_clampf(0.65f - dist / def->range * 0.5f, 0.1f, 0.65f);
            for (int i = 0; i < def->pellets; i++) {
                if (tb_rng_chance(&sim->rng, chance)) {
                    hit_target(sim, e, decoy, def->damage);
                    if (decoy) {
                        break;
                    }
                }
            }
        } else {
            tb_vec3 v = tb_v3_scale(tb_v3_normalize(to), def->projectile_speed);
            tb__spawn_projectile(sim, tb_v3_add(eye, tb_v3_scale(tb_v3_normalize(to), 0.6f)), v, -1,
                                 def->damage);
        }
    }
    if (def->mortar_ticks && e->cooldown2 == 0 && dist <= 40.0f) {
        /* Lob a present: ballistic arc landing on the target. */
        e->cooldown2 = def->mortar_ticks;
        float t = tb_clampf(dist / 12.0f, 0.8f, 2.5f);
        tb_vec3 v = tb_v3_scale(to, 1.0f / t);
        v.z += 0.5f * GRAVITY * t;
        tb_entity *p = tb__spawn_projectile(sim, tb_v3_add(eye, tb_v3(0, 0, 0.8f)), v, -1, 0);
        if (p) {
            p->gravity = true;
            p->splash_damage = def->mortar_damage;
            p->splash_radius = 4.0f;
        }
    }
}

static void try_teleport(tb_sim *sim, tb_entity *e, tb_vec3 near)
{
    tb_vec3 mins, maxs;
    tb_move_hull(false, &mins, &maxs);
    for (int attempt = 0; attempt < 8; attempt++) {
        float a = tb_rng_float01(&sim->rng) * 2.0f * TB_PI;
        float r = 4.0f + 3.0f * tb_rng_float01(&sim->rng);
        tb_vec3 spot = tb_v3(near.x + cosf(a) * r, near.y + sinf(a) * r, e->move.origin.z + 0.05f);
        tb_trace here = tb_trace_box(sim->cfg.geometry, spot, spot, mins, maxs, TB_CONTENTS_SOLID);
        tb_trace floor = tb_trace_box(sim->cfg.geometry, spot, tb_v3_sub(spot, tb_v3(0, 0, 2.0f)), mins, maxs,
                                      TB_CONTENTS_SOLID);
        if (!here.start_solid && floor.fraction < 1.0f && tb__line_of_sight(sim, spot, near)) {
            e->move.origin = floor.end;
            e->move.velocity = tb_v3(0, 0, 0);
            return;
        }
    }
}

static void move_enemy(tb_sim *sim, tb_entity *e, const tb_enemy_def *def, tb_vec3 goal, bool approach,
                       bool flee)
{
    tb_vec3 to = tb_v3_sub(goal, e->origin);
    float yaw = atan2f(to.y, to.x) + (flee ? TB_PI : 0.0f);
    if (def->flying) {
        tb_vec3 want = approach ? tb_v3_scale(tb_v3_normalize(tb_v3_add(to, tb_v3(0, 0, 1.5f))), def->speed)
                                : tb_v3(0, 0, 0);
        tb_vec3 end = tb_v3_add(e->origin, tb_v3_scale(want, TB_DT));
        tb_trace tr = tb_trace_box(sim->cfg.geometry, e->origin, end, e->mins, e->maxs, TB_CONTENTS_SOLID);
        e->origin = tr.end;
        e->move.origin = tb_v3_sub(e->origin, tb_v3(0, 0, e->base_half.z));
        return;
    }
    tb_move_params params = sim->move_params;
    params.walk_speed = def->speed * (flee ? 0.35f : 1.0f);
    tb_input in;
    memset(&in, 0, sizeof(in));
    in.yaw = tb_angle_to_short(yaw);
    in.forward = (approach || flee) ? 127 : 0;
    in.side = (int8_t)((approach || flee) ? 0 : ((e->ai_timer & 64u) ? 60 : -60)); /* strafe while shooting */
    tb_move_step(&e->move, &in, &params, sim->cfg.geometry, TB_DT);
    sync_origin(e);
}

static void think(tb_sim *sim, tb_entity *e)
{
    const tb_enemy_def *def = &g_enemies[e->kind];
    if (e->stun_ticks > 0) {
        e->stun_ticks--;
        return; /* frozen by a charged snowball */
    }
    e->ai_timer++;
    if (e->cooldown > 0) {
        e->cooldown--;
    }
    if (e->cooldown2 > 0) {
        e->cooldown2--;
    }
    if (e->ai_timer % RETARGET_TICKS == 0) {
        choose_target(sim, e, def);
    }
    tb_vec3 aim;
    tb_entity *decoy;
    bool has_target = target_pos(sim, e, &aim, &decoy);
    if (e->shrink_ticks > 0) {
        if (--e->shrink_ticks == 0) {
            e->mins = tb_v3_scale(e->base_half, -1.0f);
            e->maxs = e->base_half;
        }
        if (has_target) {
            move_enemy(sim, e, def, aim, false, true); /* tiny and scared */
        }
        return;
    }
    if (!has_target || e->ai_state != TB_AI_CHASE) {
        if (!def->flying) {
            move_enemy(sim, e, def, e->origin, false, false); /* stand (and fall) */
        }
        return;
    }
    float dist = tb_v3_len(tb_v3_sub(aim, e->origin));
    bool visible = tb__line_of_sight(sim, enemy_eye(e), aim);
    if (def->teleport_ticks && e->cooldown2 == 0) {
        e->cooldown2 = def->teleport_ticks;
        try_teleport(sim, e, aim);
        sync_origin(e);
    }
    move_enemy(sim, e, def, aim, !visible || dist > def->range * 0.5f, false);
    if (visible) {
        attack(sim, e, def, aim, decoy);
    }
}

void tb__ai_update(tb_sim *sim)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (sim->entities[i].type == TB_ENT_ENEMY) {
            think(sim, &sim->entities[i]);
        }
    }
}
