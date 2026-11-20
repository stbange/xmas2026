/* SPDX-License-Identifier: MIT */
/* Level wiring: named entities, relays, movers, usables, light groups,
 * shrink zones, hurt volumes and the per-player world interactions (use,
 * jetpack, Holo-Santa decoy, stomping shrunken enemies). */
#include "sim/sim_internal.h"

#include <stdio.h>
#include <string.h>

#define FIRE_DEPTH_MAX 4

void tb_sim_push_event(tb_sim *sim, tb_event_type type, int player, int32_t value, const char *name)
{
    if (sim->event_count >= TB_MAX_EVENTS) {
        return;
    }
    tb_event *ev = &sim->events[sim->event_count++];
    ev->type = (uint8_t)type;
    ev->player = (int8_t)player;
    ev->value = value;
    snprintf(ev->name, sizeof(ev->name), "%s", name ? name : "");
}

void tb_sim_set_name(tb_sim *sim, tb_handle h, const char *name, const char *target)
{
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        snprintf(e->name, sizeof(e->name), "%s", name ? name : "");
        snprintf(e->target, sizeof(e->target), "%s", target ? target : "");
    }
}

tb_handle tb_sim_find(tb_sim *sim, const char *name)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type != TB_ENT_NONE && name[0] && strcmp(e->name, name) == 0) {
            return tb__handle(i, e->gen);
        }
    }
    return 0;
}

bool tb_sim_light_on(const tb_sim *sim, int group)
{
    return group >= 0 && group < TB_LIGHT_GROUPS && ((sim->light_groups >> group) & 1u);
}

void tb_sim_set_light(tb_sim *sim, int group, bool on)
{
    if (group < 0 || group >= TB_LIGHT_GROUPS) {
        return;
    }
    if (on) {
        sim->light_groups |= 1u << group;
    } else {
        sim->light_groups &= ~(1u << group);
    }
}

static int fire(tb_sim *sim, const char *target, int depth)
{
    if (!target || !target[0] || depth > FIRE_DEPTH_MAX) {
        return 0;
    }
    int n = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type == TB_ENT_NONE || strcmp(e->name, target) != 0) {
            continue;
        }
        n++;
        if (e->type == TB_ENT_MOVER) {
            e->opening = !e->opening;
        } else if (e->type == TB_ENT_TRIGGER && e->kind == TB_TRIGGER_RELAY) {
            n += fire(sim, e->target, depth + 1);
        } else if (e->type == TB_ENT_ENEMY) {
            e->ai_timer = 0; /* wake up */
            e->ai_state = TB_AI_CHASE;
        }
    }
    return n;
}

int tb_sim_fire(tb_sim *sim, const char *target)
{
    return fire(sim, target, 0);
}

tb_handle tb_sim_add_relay(tb_sim *sim, tb_vec3 mins, tb_vec3 maxs, const char *name, const char *target,
                           bool once)
{
    tb_handle h = tb_sim_add_trigger(sim, TB_TRIGGER_RELAY, mins, maxs, 0);
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->flag = once;
        tb_sim_set_name(sim, h, name, target);
    }
    return h;
}

tb_handle tb_sim_add_usable(tb_sim *sim, tb_usable_kind kind, tb_vec3 mins, tb_vec3 maxs, const char *name,
                            const char *target, int32_t value)
{
    tb_vec3 centre = tb_v3_scale(tb_v3_add(mins, maxs), 0.5f);
    tb_handle h = tb_sim_spawn(sim, TB_ENT_USABLE, centre, tb_v3_scale(tb_v3_sub(maxs, mins), 0.5f));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->kind = (uint8_t)kind;
        e->value = value;
        tb_sim_set_name(sim, h, name, target);
    }
    return h;
}

static void set_box_brush(tb_brush *b, tb_vec3 lo, tb_vec3 hi)
{
    /* Axial planes in tb_geometry_add_box order: +x -x +y -y +z -z. */
    b->planes[0].d = hi.x;
    b->planes[1].d = -lo.x;
    b->planes[2].d = hi.y;
    b->planes[3].d = -lo.y;
    b->planes[4].d = hi.z;
    b->planes[5].d = -lo.z;
}

tb_handle tb_sim_add_mover(tb_sim *sim, int brush, tb_vec3 offset, float speed, const char *name, bool open)
{
    tb_geometry *g = sim->cfg.geometry;
    if (brush < 0 || brush >= g->count || g->brushes[brush].count != 6) {
        return 0;
    }
    const tb_brush *b = &g->brushes[brush];
    tb_vec3 lo = tb_v3(-b->planes[1].d, -b->planes[3].d, -b->planes[5].d);
    tb_vec3 hi = tb_v3(b->planes[0].d, b->planes[2].d, b->planes[4].d);
    tb_vec3 centre = tb_v3_scale(tb_v3_add(lo, hi), 0.5f);
    tb_handle h = tb_sim_spawn(sim, TB_ENT_MOVER, centre, tb_v3_scale(tb_v3_sub(hi, lo), 0.5f));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->value = brush;
        e->offset = offset;
        e->speed = speed;
        e->opening = open;
        e->progress = open ? 1.0f : 0.0f;
        tb_sim_set_name(sim, h, name, NULL);
        if (open) {
            set_box_brush(&g->brushes[brush], tb_v3_add(lo, offset), tb_v3_add(hi, offset));
        }
    }
    return h;
}

static void update_mover(tb_sim *sim, tb_entity *e)
{
    float goal = e->opening ? 1.0f : 0.0f;
    if (e->progress == goal) {
        return;
    }
    float dist = tb_v3_len(e->offset);
    float step = dist > 0.0f ? e->speed * TB_DT / dist : 1.0f;
    e->progress = e->opening ? tb_clampf(e->progress + step, 0, 1) : tb_clampf(e->progress - step, 0, 1);
    tb_vec3 shift = tb_v3_scale(e->offset, e->progress);
    tb_vec3 lo = tb_v3_add(tb_v3_add(e->origin, e->mins), shift);
    tb_vec3 hi = tb_v3_add(tb_v3_add(e->origin, e->maxs), shift);
    set_box_brush(&sim->cfg.geometry->brushes[e->value], lo, hi);
}

void tb__world_update(tb_sim *sim)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type == TB_ENT_MOVER) {
            update_mover(sim, e);
        } else if (e->type == TB_ENT_DECOY) {
            if (e->ttl > 0) {
                e->ttl--;
            }
            if (e->ttl == 0) {
                e->type = TB_ENT_NONE; /* the hologram fades */
            }
        } else if (e->type == TB_ENT_USABLE && e->ai_timer > 0) {
            e->ai_timer--; /* re-use cooldown */
        }
    }
}

/* ---- per-player ---------------------------------------------------------- */

static void use_entity(tb_sim *sim, int slot, tb_entity *e)
{
    tb_player *p = &sim->players[slot];
    if (e->ai_timer > 0) {
        return;
    }
    e->ai_timer = TB_SIM_HZ_TICKS / 2;
    switch ((tb_usable_kind)e->kind) {
    case TB_USE_SWITCH:
        tb_sim_set_light(sim, e->value, !tb_sim_light_on(sim, e->value));
        break;
    case TB_USE_TOILET:
        tb_vitals_heal(&p->vitals, 1, TB_HEALTH_MAX);
        break;
    case TB_USE_VENDING:
        tb_vitals_heal(&p->vitals, 5, TB_HEALTH_MAX);
        break;
    default: /* arcade cabinets and buttons only report and fire */
        break;
    }
    p->interactions |= 1u << e->kind;
    tb_sim_push_event(sim, TB_EVENT_USED, slot, e->kind, e->name);
    tb_sim_fire(sim, e->target);
}

static void try_use(tb_sim *sim, int slot)
{
    tb_player *p = &sim->players[slot];
    tb_vec3 eye = tb__eye(p);
    tb_vec3 end = tb_v3_add(eye, tb_v3_scale(tb__view_dir(p->move.yaw, p->move.pitch), TB_USE_RANGE));
    float wall = tb_trace_ray(sim->cfg.geometry, eye, end, TB_CONTENTS_SOLID).fraction;
    tb_entity *best = NULL;
    float best_f = wall + 0.01f;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type != TB_ENT_USABLE) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__entity_box(e, &lo, &hi);
        float f = tb_ray_box(eye, end, lo, hi);
        if (f <= best_f) {
            best_f = f;
            best = e;
        }
    }
    if (best) {
        use_entity(sim, slot, best);
    }
}

static void abilities(tb_sim *sim, int slot, const tb_input *in)
{
    tb_player *p = &sim->players[slot];
    if (p->decoy_cooldown > 0) {
        p->decoy_cooldown--;
    }
    /* Jetpack: thrust up while held. */
    p->jetting = false;
    if ((in->buttons & TB_BTN_ABILITY1) && p->inv.items[TB_ITEM_JETPACK] && p->jet_fuel > 0) {
        p->jet_fuel--;
        p->move.velocity.z += TB_JET_THRUST * TB_DT;
        if (p->move.velocity.z > 6.0f) {
            p->move.velocity.z = 6.0f;
        }
        p->move.on_ground = false;
        p->move.jumping = true;
        p->jetting = true;
    }
    /* Holo-Santa: a decoy hologram enemies shoot at. */
    if ((in->buttons & TB_BTN_ABILITY2) && !(p->last_buttons & TB_BTN_ABILITY2) &&
        p->inv.items[TB_ITEM_HOLO_SANTA] && p->decoy_cooldown == 0) {
        tb_vec3 fwd = tb__view_dir(p->move.yaw, 0);
        tb_vec3 at = tb_v3_add(p->move.origin, tb_v3_add(tb_v3_scale(fwd, 1.5f), tb_v3(0, 0, 0.9f)));
        tb_entity *d = tb_sim_get(sim, tb_sim_spawn(sim, TB_ENT_DECOY, at, tb_v3(0.3f, 0.3f, 0.9f)));
        if (d) {
            d->ttl = TB_DECOY_TICKS;
            d->owner = (tb_handle)(slot + 1);
            p->decoy_cooldown = TB_DECOY_COOLDOWN;
        }
    }
}

static void volumes(tb_sim *sim, int slot)
{
    tb_player *p = &sim->players[slot];
    tb_vec3 plo, phi;
    tb__player_box(p, &plo, &phi);
    bool in_shrink = false;
    const uint32_t me = 1u << slot;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        bool trig = e->type == TB_ENT_TRIGGER;
        bool small_enemy = e->type == TB_ENT_ENEMY && e->shrink_ticks > 0;
        if (!trig && !small_enemy) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__entity_box(e, &lo, &hi);
        bool inside = tb_boxes_overlap(plo, phi, lo, hi);
        if (trig && e->kind == TB_TRIGGER_RELAY) {
            /* Relays fire when a player enters (cooldown holds who is inside). */
            bool was = (e->cooldown & me) != 0;
            e->cooldown = inside ? (e->cooldown | me) : (e->cooldown & ~me);
            if (inside && !was && !(e->flag && e->amount)) {
                e->amount = 1; /* fired at least once */
                tb_sim_push_event(sim, TB_EVENT_TRIGGER, slot, 0, e->name);
                tb_sim_fire(sim, e->target);
            }
            continue;
        }
        if (!inside) {
            continue;
        }
        if (small_enemy) {
            tb__damage_entity(sim, e, e->vitals.health, 0, 0); /* stomp! */
        } else if (e->kind == TB_TRIGGER_SHRINK) {
            in_shrink = true;
        } else if (e->kind == TB_TRIGGER_HURT) {
            tb_sim_damage_player(sim, slot, e->value);
        }
    }
    p->in_shrink_zone = in_shrink;
    tb_move_set_shrunk(&p->move, sim->cfg.geometry, in_shrink);
}

void tb__player_world(tb_sim *sim, int slot, const tb_input *in)
{
    tb_player *p = &sim->players[slot];
    if ((in->buttons & TB_BTN_USE) && !(p->last_buttons & TB_BTN_USE)) {
        try_use(sim, slot);
    }
    abilities(sim, slot, in);
    volumes(sim, slot);
}
