/* SPDX-License-Identifier: MIT */
#include "sim/sim.h"

#include "core/hash.h"
#include "sim/sim_internal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define PROJECTILE_TTL  (5u * TB_SIM_HZ_TICKS)
#define PIPE_TTL        (20u * TB_SIM_HZ_TICKS)
#define PROJECTILE_HALF 0.1f
#define SELF_SPLASH_PCT 50
#define GRAVITY         20.0f

/* ---- shared helpers ---------------------------------------------------- */

tb_handle tb__handle(int index, uint16_t gen)
{
    return ((tb_handle)gen << 16) | (tb_handle)(index + 1);
}

int tb__index(const tb_sim *sim, const tb_entity *e)
{
    return (int)(e - sim->entities);
}

void tb__entity_box(const tb_entity *e, tb_vec3 *lo, tb_vec3 *hi)
{
    *lo = tb_v3_add(e->origin, e->mins);
    *hi = tb_v3_add(e->origin, e->maxs);
}

void tb__player_box(const tb_player *p, tb_vec3 *lo, tb_vec3 *hi)
{
    tb_vec3 mins, maxs;
    tb_move_state_hull(&p->move, &mins, &maxs);
    *lo = tb_v3_add(p->move.origin, mins);
    *hi = tb_v3_add(p->move.origin, maxs);
}

tb_vec3 tb__view_dir(float yaw, float pitch)
{
    float cp = cosf(pitch);
    return tb_v3(cosf(yaw) * cp, sinf(yaw) * cp, sinf(pitch));
}

tb_vec3 tb__eye(const tb_player *p)
{
    return tb_v3_add(p->move.origin, tb_v3(0, 0, tb_move_eye_height(&p->move)));
}

bool tb__line_of_sight(const tb_sim *sim, tb_vec3 a, tb_vec3 b)
{
    return tb_trace_ray(sim->cfg.geometry, a, b, TB_CONTENTS_SOLID).fraction >= 1.0f;
}

bool tb__damageable(const tb_entity *e)
{
    return e->type == TB_ENT_TARGET || e->type == TB_ENT_ENEMY;
}

/* ---- entities ---------------------------------------------------------- */

tb_handle tb_sim_spawn(tb_sim *sim, tb_entity_type type, tb_vec3 origin, tb_vec3 half)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type == TB_ENT_NONE) {
            uint16_t gen = (uint16_t)(e->gen + 1u);
            memset(e, 0, sizeof(*e));
            e->gen = gen ? gen : 1u;
            e->type = (uint8_t)type;
            e->origin = origin;
            e->mins = tb_v3_scale(half, -1.0f);
            e->maxs = half;
            e->ai_target = -1;
            return tb__handle(i, e->gen);
        }
    }
    return 0;
}

tb_entity *tb_sim_get(tb_sim *sim, tb_handle h)
{
    int index = (int)(h & 0xFFFFu) - 1;
    if (index < 0 || index >= TB_MAX_ENTITIES) {
        return NULL;
    }
    tb_entity *e = &sim->entities[index];
    return (e->type != TB_ENT_NONE && e->gen == (uint16_t)(h >> 16)) ? e : NULL;
}

void tb_sim_remove(tb_sim *sim, tb_handle h)
{
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->type = TB_ENT_NONE;
    }
}

tb_handle tb_sim_add_target(tb_sim *sim, tb_vec3 origin, int32_t health)
{
    tb_handle h = tb_sim_spawn(sim, TB_ENT_TARGET, origin, tb_v3(0.4f, 0.4f, 0.9f));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->vitals.health = health;
    }
    return h;
}

tb_handle tb_sim_add_pickup(tb_sim *sim, tb_pickup_kind kind, tb_vec3 origin, int32_t value, int32_t amount,
                            bool flag)
{
    tb_handle h = tb_sim_spawn(sim, TB_ENT_PICKUP, origin, tb_v3(0.4f, 0.4f, 0.4f));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->kind = (uint8_t)kind;
        e->value = value;
        e->amount = amount;
        e->flag = flag;
    }
    return h;
}

tb_handle tb_sim_add_trigger(tb_sim *sim, tb_trigger_kind kind, tb_vec3 mins, tb_vec3 maxs, int32_t id)
{
    tb_vec3 centre = tb_v3_scale(tb_v3_add(mins, maxs), 0.5f);
    tb_handle h = tb_sim_spawn(sim, TB_ENT_TRIGGER, centre, tb_v3_scale(tb_v3_sub(maxs, mins), 0.5f));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->kind = (uint8_t)kind;
        e->value = id;
    }
    return h;
}

/* ---- players ----------------------------------------------------------- */

static void save_checkpoint(tb_player *p, int32_t id)
{
    p->checkpoint.valid = true;
    p->checkpoint.id = id;
    p->checkpoint.origin = p->move.origin;
    p->checkpoint.yaw = p->move.yaw;
    p->checkpoint.vitals = p->vitals;
    memcpy(p->checkpoint.ammo, p->inv.ammo, sizeof(p->checkpoint.ammo));
}

static void place_player(tb_player *p, tb_vec3 origin, float yaw)
{
    memset(&p->move, 0, sizeof(p->move));
    p->move.origin = origin;
    p->move.yaw = yaw;
    p->move.ground_normal = tb_v3(0, 0, 1);
}

void tb_sim_init(tb_sim *sim, const tb_sim_config *cfg)
{
    memset(sim, 0, sizeof(*sim));
    sim->cfg = *cfg;
    tb_move_default_params(&sim->move_params);
    tb_rng_seed(&sim->rng, cfg->seed, 0x51u);
    sim->light_groups = 0xFFFFFFFFu; /* lights start on */
}

bool tb_sim_add_player(tb_sim *sim, int slot)
{
    if (slot < 0 || slot >= TB_MAX_PLAYERS || sim->players[slot].active) {
        return false;
    }
    tb_player *p = &sim->players[slot];
    memset(p, 0, sizeof(*p));
    p->active = true;
    place_player(p, sim->cfg.spawn[slot], sim->cfg.spawn_yaw);
    tb_vitals_init(&p->vitals);
    tb_inventory_init(&p->inv, sim->cfg.body_replaced);
    save_checkpoint(p, 0); /* the level start is checkpoint 0 */
    return true;
}

int tb_sim_player_count(const tb_sim *sim)
{
    int n = 0;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        n += sim->players[i].active;
    }
    return n;
}

static void respawn_at_checkpoint(tb_player *p)
{
    place_player(p, p->checkpoint.origin, p->checkpoint.yaw);
    p->vitals = p->checkpoint.vitals;
    memcpy(p->inv.ammo, p->checkpoint.ammo, sizeof(p->inv.ammo));
    memset(p->weapons, 0, sizeof(p->weapons));
    p->state = TB_PLAYER_ALIVE;
    p->state_ticks = 0;
    p->revive_progress = 0;
}

static bool any_other_alive(const tb_sim *sim, int slot)
{
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        if (i != slot && sim->players[i].active && sim->players[i].state == TB_PLAYER_ALIVE) {
            return true;
        }
    }
    return false;
}

static void player_died(tb_sim *sim, int slot)
{
    tb_player *p = &sim->players[slot];
    p->deaths++;
    p->state_ticks = 0;
    p->revive_progress = 0;
    p->state = any_other_alive(sim, slot) ? TB_PLAYER_DOWNED : TB_PLAYER_DEAD;
}

void tb_sim_damage_player(tb_sim *sim, int slot, int32_t raw_amount)
{
    tb_player *p = &sim->players[slot];
    if (!p->active || p->state != TB_PLAYER_ALIVE) {
        return;
    }
    tb_vitals_damage(&p->vitals, tb_difficulty_scale_damage(sim->cfg.difficulty, raw_amount));
    if (tb_vitals_dead(&p->vitals)) {
        player_died(sim, slot);
    }
}

/* ---- combat ------------------------------------------------------------ */

void tb__damage_entity(tb_sim *sim, tb_entity *e, int32_t amount, uint32_t stun, uint32_t shrink)
{
    if (!tb__damageable(e)) {
        return;
    }
    e->vitals.health -= amount;
    if (stun > e->stun_ticks) {
        e->stun_ticks = stun;
    }
    if (shrink && e->type == TB_ENT_ENEMY && e->shrink_ticks == 0 &&
        !tb_enemy_get((tb_enemy_kind)e->kind)->shrink_immune) {
        e->shrink_ticks = shrink;
        e->mins = tb_v3_scale(e->base_half, -0.3f);
        e->maxs = tb_v3_scale(e->base_half, 0.3f);
    }
    if (e->vitals.health <= 0) {
        if (e->type == TB_ENT_ENEMY && e->name[0]) {
            tb_sim_push_event(sim, TB_EVENT_KILLED, -1, e->kind, e->name);
        }
        e->type = TB_ENT_NONE;
    }
}

/* Nearest damageable entity hit by the segment, or NULL. */
static tb_entity *ray_entities(tb_sim *sim, tb_vec3 start, tb_vec3 end, float max_frac, tb_handle ignore)
{
    tb_entity *best = NULL;
    float best_frac = max_frac;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (!tb__damageable(e) || tb__handle(i, e->gen) == ignore) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__entity_box(e, &lo, &hi);
        float f = tb_ray_box(start, end, lo, hi);
        if (f <= best_frac) {
            best_frac = f;
            best = e;
        }
    }
    return best;
}

/* Nearest player or decoy hit by the segment (enemy fire): returns the
 * player slot, TB_MAX_PLAYERS for a decoy (*decoy set), or -1. */
static int ray_players(tb_sim *sim, tb_vec3 start, tb_vec3 end, float max_frac, tb_entity **decoy)
{
    int best = -1;
    float best_frac = max_frac;
    *decoy = NULL;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        tb_player *p = &sim->players[i];
        if (!p->active || p->state != TB_PLAYER_ALIVE) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__player_box(p, &lo, &hi);
        float f = tb_ray_box(start, end, lo, hi);
        if (f <= best_frac) {
            best_frac = f;
            best = i;
        }
    }
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type != TB_ENT_DECOY) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__entity_box(e, &lo, &hi);
        float f = tb_ray_box(start, end, lo, hi);
        if (f <= best_frac) {
            best_frac = f;
            best = TB_MAX_PLAYERS;
            *decoy = e;
        }
    }
    return best;
}

static void splash(tb_sim *sim, tb_vec3 at, float radius, int32_t damage, int owner_slot, bool from_enemy)
{
    if (!from_enemy) {
        for (int i = 0; i < TB_MAX_ENTITIES; i++) {
            tb_entity *e = &sim->entities[i];
            if (!tb__damageable(e)) {
                continue;
            }
            float d = tb_v3_len(tb_v3_sub(e->origin, at));
            if (d < radius) {
                tb__damage_entity(sim, e, (int32_t)((float)damage * (1.0f - d / radius)), 0, 0);
            }
        }
    }
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        tb_player *p = &sim->players[i];
        if (!p->active || (!from_enemy && i != owner_slot)) {
            continue; /* no friendly fire, only self-damage */
        }
        tb_vec3 centre = tb_v3_add(p->move.origin, tb_v3(0, 0, 0.9f));
        float d = tb_v3_len(tb_v3_sub(centre, at));
        if (d < radius) {
            int32_t dmg = (int32_t)((float)damage * (1.0f - d / radius));
            tb_sim_damage_player(sim, i, from_enemy ? dmg : dmg * SELF_SPLASH_PCT / 100);
        }
    }
}

tb_entity *tb__spawn_projectile(tb_sim *sim, tb_vec3 at, tb_vec3 velocity, int owner_slot, int32_t damage)
{
    tb_handle h =
        tb_sim_spawn(sim, TB_ENT_PROJECTILE, at, tb_v3(PROJECTILE_HALF, PROJECTILE_HALF, PROJECTILE_HALF));
    tb_entity *e = tb_sim_get(sim, h);
    if (e) {
        e->velocity = velocity;
        e->owner = owner_slot >= 0 ? (tb_handle)(owner_slot + 1) : 0;
        e->from_enemy = owner_slot < 0;
        e->damage = damage;
        e->ttl = PROJECTILE_TTL;
    }
    return e;
}

static void fire_weapon(tb_sim *sim, int slot, const tb_weapon_def *def, const tb_fire_event *ev)
{
    tb_player *p = &sim->players[slot];
    tb_vec3 eye = tb__eye(p);
    tb_vec3 dir = tb__view_dir(p->move.yaw, p->move.pitch);

    if (def->kind == TB_WK_PROJECTILE) {
        tb_vec3 at = tb_v3_add(eye, tb_v3_scale(dir, 0.5f));
        tb_entity *e =
            tb__spawn_projectile(sim, at, tb_v3_scale(dir, def->projectile_speed), slot, ev->damage);
        if (e) {
            e->splash_damage = def->splash_damage;
            e->splash_radius = def->splash_radius;
            e->stun_ticks = ev->stun_ticks;
            e->gravity = def->gravity;
            e->bounce = def->bounce;
            e->remote = def->remote;
            if (def->remote) {
                e->ttl = PIPE_TTL;
            }
        }
        return;
    }

    for (int i = 0; i < def->pellets; i++) {
        tb_vec3 d = dir;
        if (def->spread > 0.0f) {
            float yaw = p->move.yaw + (tb_rng_float01(&sim->rng) - 0.5f) * 2.0f * def->spread;
            float pitch = p->move.pitch + (tb_rng_float01(&sim->rng) - 0.5f) * 2.0f * def->spread;
            d = tb__view_dir(yaw, pitch);
        }
        tb_vec3 end = tb_v3_add(eye, tb_v3_scale(d, def->range));
        tb_trace wt = tb_trace_ray(sim->cfg.geometry, eye, end, TB_CONTENTS_SOLID);
        tb_entity *hit = ray_entities(sim, eye, end, wt.fraction, 0);
        if (hit) {
            tb__damage_entity(sim, hit, ev->damage, ev->stun_ticks, def->shrink_ticks);
        }
    }
}

static void explode(tb_sim *sim, tb_entity *e, tb_vec3 at, tb_entity *hit)
{
    int owner_slot = (int)e->owner - 1;
    int32_t damage = e->damage, splash_damage = e->splash_damage;
    float radius = e->splash_radius;
    uint32_t stun = e->stun_ticks;
    bool from_enemy = e->from_enemy;
    e->type = TB_ENT_NONE;
    if (hit) {
        tb__damage_entity(sim, hit, damage, stun, 0);
    }
    if (radius > 0.0f) {
        splash(sim, at, radius, splash_damage, owner_slot, from_enemy);
        if (sim->shake_ticks < 20) {
            sim->shake_ticks = 20;
        }
    }
}

/* Detonates every remote projectile of a player. */
static void detonate_remote(tb_sim *sim, int slot)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type == TB_ENT_PROJECTILE && e->remote && (int)e->owner - 1 == slot) {
            explode(sim, e, e->origin, NULL);
        }
    }
}

static void update_projectile(tb_sim *sim, int i)
{
    tb_entity *e = &sim->entities[i];
    if (e->ttl > 0) {
        e->ttl--;
    }
    if (e->ttl == 0) {
        if (e->remote) {
            explode(sim, e, e->origin, NULL);
        } else {
            e->type = TB_ENT_NONE;
        }
        return;
    }
    if (e->gravity) {
        e->velocity.z -= GRAVITY * TB_DT;
    }
    tb_vec3 end = tb_v3_add(e->origin, tb_v3_scale(e->velocity, TB_DT));
    tb_trace wt = tb_trace_ray(sim->cfg.geometry, e->origin, end, TB_CONTENTS_SOLID);
    if (e->from_enemy) {
        tb_entity *decoy;
        int victim = ray_players(sim, e->origin, end, wt.fraction, &decoy);
        if (victim >= 0) {
            tb_vec3 at = tb_v3_lerp(e->origin, end, 0.5f);
            if (decoy) {
                decoy->type = TB_ENT_NONE; /* the hologram pops */
            } else {
                tb_sim_damage_player(sim, victim, e->damage);
            }
            explode(sim, e, at, NULL);
            return;
        }
    } else if (!e->remote) {
        tb_entity *hit = ray_entities(sim, e->origin, end, wt.fraction, tb__handle(i, e->gen));
        if (hit) {
            explode(sim, e, hit->origin, hit);
            return;
        }
    }
    if (wt.fraction >= 1.0f) {
        e->origin = end;
        return;
    }
    if (e->bounce) {
        /* Reflect and lose energy; come to rest when slow. */
        e->origin = tb_v3_add(wt.end, tb_v3_scale(wt.normal, 0.02f));
        float vn = tb_v3_dot(e->velocity, wt.normal);
        e->velocity = tb_v3_scale(tb_v3_sub(e->velocity, tb_v3_scale(wt.normal, 2.0f * vn)), 0.45f);
        if (tb_v3_len(e->velocity) < 1.0f) {
            e->velocity = tb_v3(0, 0, 0);
            e->gravity = wt.normal.z < 0.7f; /* resting on a floor stays put */
        }
        return;
    }
    explode(sim, e, wt.end, NULL);
}

static void update_projectiles(tb_sim *sim)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (sim->entities[i].type == TB_ENT_PROJECTILE) {
            update_projectile(sim, i);
        }
    }
}

/* ---- pickups and triggers --------------------------------------------- */

static bool apply_pickup(tb_sim *sim, int slot, const tb_entity *e)
{
    tb_player *p = &sim->players[slot];
    switch ((tb_pickup_kind)e->kind) {
    case TB_PICKUP_HEALTH:
        return tb_vitals_heal(&p->vitals, e->amount, e->flag ? TB_HEALTH_OVERCHARGE : TB_HEALTH_MAX) > 0;
    case TB_PICKUP_ARMOUR:
        return tb_vitals_add_armour(&p->vitals, e->amount) > 0;
    case TB_PICKUP_WEAPON: {
        bool had = tb_inventory_has_weapon(&p->inv, (tb_weapon_id)e->value);
        const tb_weapon_def *def = tb_weapon_get((tb_weapon_id)e->value);
        if (!def || !tb_inventory_give_weapon(&p->inv, (tb_weapon_id)e->value)) {
            return false;
        }
        tb_inventory_add_ammo(&p->inv, def->ammo, e->amount);
        return !had || e->amount > 0;
    }
    case TB_PICKUP_AMMO:
        tb_inventory_add_ammo(&p->inv, (tb_ammo_type)e->value, e->amount);
        return true;
    case TB_PICKUP_ITEM:
        tb_inventory_give_item(&p->inv, (tb_item_id)e->value, e->flag);
        if (e->value == TB_ITEM_JETPACK) {
            p->jet_fuel = TB_JET_FUEL_MAX;
        }
        return true;
    case TB_PICKUP_BAUBLE:
    case TB_PICKUP_FLOPPY:
        /* Shared collectibles: count for every active player present now. */
        for (int i = 0; i < TB_MAX_PLAYERS; i++) {
            tb_player *q = &sim->players[i];
            if (!q->active) {
                continue;
            }
            if (e->kind == TB_PICKUP_BAUBLE) {
                q->bauble = true;
            } else if (e->value >= 0 && e->value < TB_FLOPPIES) {
                q->floppies |= (uint8_t)(1u << e->value);
            }
        }
        if (e->kind == TB_PICKUP_BAUBLE) {
            sim->bauble_collected = true;
        }
        return true;
    default:
        return false;
    }
}

static void touch_entities(tb_sim *sim, int slot)
{
    tb_player *p = &sim->players[slot];
    tb_vec3 plo, phi;
    tb__player_box(p, &plo, &phi);
    p->at_exit = false;
    p->portal = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        tb_entity *e = &sim->entities[i];
        if (e->type != TB_ENT_PICKUP && e->type != TB_ENT_TRIGGER) {
            continue;
        }
        tb_vec3 lo, hi;
        tb__entity_box(e, &lo, &hi);
        if (!tb_boxes_overlap(plo, phi, lo, hi)) {
            continue;
        }
        if (e->type == TB_ENT_PICKUP) {
            if (apply_pickup(sim, slot, e)) {
                tb_sim_push_event(sim, TB_EVENT_PICKUP, slot, e->kind, e->name);
                e->type = TB_ENT_NONE;
            }
        } else if (e->kind == TB_TRIGGER_CHECKPOINT) {
            if (!p->checkpoint.valid || p->checkpoint.id < e->value) {
                save_checkpoint(p, e->value);
            }
        } else if (e->kind == TB_TRIGGER_EXIT) {
            p->at_exit = true;
        } else if (e->kind == TB_TRIGGER_PORTAL) {
            p->portal = e->value;
        }
    }
}

/* ---- per-player update ------------------------------------------------- */

static void update_alive(tb_sim *sim, int slot, const tb_input *in)
{
    tb_player *p = &sim->players[slot];
    if (in->weapon >= 1 && in->weapon <= TB_WEAPON_SLOTS) {
        tb_inventory_select(&p->inv, in->weapon);
    }
    tb__player_world(sim, slot, in); /* use, jetpack, decoy, shrink zones, stomps */
    if (p->state != TB_PLAYER_ALIVE) {
        return; /* a hurt volume just killed them */
    }
    if (p->jetting) {
        /* Jetpack steering: real air control while thrusting. */
        tb_move_params jet = sim->move_params;
        jet.air_wish_cap = jet.walk_speed * 0.8f;
        jet.air_accelerate = 3.0f;
        float drag = 1.0f - 1.5f * TB_DT; /* hovering stays controllable */
        p->move.velocity.x *= drag;
        p->move.velocity.y *= drag;
        tb_move_step(&p->move, in, &jet, sim->cfg.geometry, TB_DT);
    } else {
        tb_move_step(&p->move, in, &sim->move_params, sim->cfg.geometry, TB_DT);
    }

    tb_weapon_id wid = tb_inventory_current(&p->inv);
    const tb_weapon_def *def = tb_weapon_get(wid);
    if (def) {
        int32_t *ammo = def->ammo != TB_AMMO_NONE ? &p->inv.ammo[def->ammo] : NULL;
        tb_fire_event ev = tb_weapon_tick(&p->weapons[wid], def, (in->buttons & TB_BTN_FIRE) != 0, ammo);
        if (ev.fired) {
            fire_weapon(sim, slot, def, &ev);
        }
        if (def->remote && (in->buttons & TB_BTN_ALT_FIRE) && !(p->last_buttons & TB_BTN_ALT_FIRE)) {
            detonate_remote(sim, slot);
        }
    }
    if ((in->buttons & TB_BTN_MELEE) && !(p->last_buttons & TB_BTN_MELEE)) {
        tb_fire_event ev = {true, 1.0f, tb_weapon_get(TB_WEAPON_CANDY_CANE)->damage, 0};
        fire_weapon(sim, slot, tb_weapon_get(TB_WEAPON_CANDY_CANE), &ev);
    }
    tb_vitals_tick(&p->vitals, TB_SIM_HZ_TICKS);
    touch_entities(sim, slot);
}

static void update_reviver(tb_sim *sim, int slot, const tb_input *in)
{
    /* A living player holding Use next to a downed teammate revives them. */
    tb_player *r = &sim->players[slot];
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        tb_player *d = &sim->players[i];
        if (i == slot || !d->active || d->state != TB_PLAYER_DOWNED) {
            continue;
        }
        float dist = tb_v3_len(tb_v3_sub(d->move.origin, r->move.origin));
        if (!(in->buttons & TB_BTN_USE) || dist > TB_REVIVE_RANGE) {
            continue;
        }
        if (++d->revive_progress >= TB_REVIVE_HOLD_TICKS) {
            d->state = TB_PLAYER_ALIVE;
            d->state_ticks = 0;
            d->revive_progress = 0;
            d->vitals.health = TB_REVIVE_HEALTH;
        }
    }
}

static void update_down_or_dead(tb_sim *sim, int slot)
{
    tb_player *p = &sim->players[slot];
    p->state_ticks++;
    if (p->state == TB_PLAYER_DOWNED) {
        if (p->state_ticks >= TB_REVIVE_WINDOW_TICKS || !any_other_alive(sim, slot)) {
            p->state = TB_PLAYER_DEAD;
            p->state_ticks = 0;
        }
        return;
    }
    uint32_t wait = tb_sim_player_count(sim) > 1 ? TB_COOP_RESPAWN_TICKS : TB_SOLO_RESPAWN_TICKS;
    if (p->state_ticks >= wait) {
        respawn_at_checkpoint(p);
    }
}

static void update_level_state(tb_sim *sim)
{
    if (sim->level_complete || !sim->bauble_collected) {
        return;
    }
    int alive = 0, at_exit = 0;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        const tb_player *p = &sim->players[i];
        if (p->active && p->state == TB_PLAYER_ALIVE) {
            alive++;
            at_exit += p->at_exit;
        }
    }
    if (alive > 0 && at_exit == alive) {
        sim->level_complete = true;
        for (int i = 0; i < TB_MAX_PLAYERS; i++) {
            sim->players[i].completed = sim->players[i].active;
        }
    }
}

void tb_sim_step(tb_sim *sim, const tb_input inputs[TB_MAX_PLAYERS])
{
    sim->event_count = 0;
    if (sim->shake_ticks > 0) {
        sim->shake_ticks--;
    }
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        tb_player *p = &sim->players[i];
        if (!p->active) {
            continue;
        }
        if (p->state == TB_PLAYER_ALIVE) {
            update_alive(sim, i, &inputs[i]);
            update_reviver(sim, i, &inputs[i]);
        } else {
            update_down_or_dead(sim, i);
        }
        p->last_buttons = inputs[i].buttons;
    }
    tb__world_update(sim);
    tb__ai_update(sim);
    update_projectiles(sim);
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        if (sim->entities[i].type == TB_ENT_TARGET && sim->entities[i].stun_ticks > 0) {
            sim->entities[i].stun_ticks--;
        }
    }
    update_level_state(sim);
    sim->tick++;
}

void tb_sim_complete_challenge(tb_sim *sim)
{
    sim->challenge_done = true;
}

void tb_sim_leave_level(tb_sim *sim)
{
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        tb_inventory_leave_level(&sim->players[i].inv);
    }
}

/* ---- state ------------------------------------------------------------- */

static void write_v3(tb_writer *w, tb_vec3 v)
{
    tb_write_f32(w, v.x);
    tb_write_f32(w, v.y);
    tb_write_f32(w, v.z);
}

static void write_vitals(tb_writer *w, const tb_vitals *v)
{
    tb_write_i32(w, v->health);
    tb_write_i32(w, v->armour);
    tb_write_i32(w, v->drain_ticks);
}

static void write_player(tb_writer *w, const tb_player *p)
{
    tb_write_u8(w, p->state);
    write_v3(w, p->move.origin);
    write_v3(w, p->move.velocity);
    tb_write_f32(w, p->move.yaw);
    tb_write_f32(w, p->move.pitch);
    tb_write_bool(w, p->move.on_ground);
    tb_write_bool(w, p->move.crouched);
    tb_write_bool(w, p->move.shrunk);
    write_vitals(w, &p->vitals);
    for (int s = 0; s < TB_WEAPON_SLOTS; s++) {
        tb_write_u8(w, (uint8_t)p->inv.slots[s]);
    }
    for (int a = 0; a < TB_AMMO_COUNT; a++) {
        tb_write_i32(w, p->inv.ammo[a]);
    }
    tb_write_u8(w, (uint8_t)p->inv.current);
    for (int k = 0; k < TB_WEAPON_COUNT; k++) {
        tb_write_u32(w, p->weapons[k].cooldown);
        tb_write_u32(w, p->weapons[k].charge);
    }
    tb_write_u32(w, p->state_ticks);
    tb_write_u32(w, p->revive_progress);
    tb_write_i32(w, p->checkpoint.id);
    tb_write_u8(w, p->floppies);
    tb_write_bool(w, p->bauble);
    tb_write_u32(w, p->deaths);
    tb_write_i32(w, p->jet_fuel);
    tb_write_u8(w, p->jetting);
    tb_write_u32(w, p->interactions);
}

void tb_sim_write_state(const tb_sim *sim, tb_writer *w)
{
    tb_write_u64(w, sim->tick);
    tb_write_u64(w, sim->rng.state);
    tb_write_u64(w, sim->rng.inc);
    tb_write_bool(w, sim->bauble_collected);
    tb_write_bool(w, sim->challenge_done);
    tb_write_bool(w, sim->level_complete);
    tb_write_u32(w, sim->light_groups);
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        const tb_player *p = &sim->players[i];
        tb_write_bool(w, p->active);
        if (p->active) {
            write_player(w, p);
        }
    }
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        const tb_entity *e = &sim->entities[i];
        tb_write_u8(w, e->type);
        if (e->type == TB_ENT_NONE) {
            continue;
        }
        tb_write_u16(w, e->gen);
        tb_write_u8(w, e->kind);
        write_v3(w, e->origin);
        write_v3(w, e->velocity);
        write_vitals(w, &e->vitals);
        tb_write_u32(w, e->stun_ticks);
        tb_write_u32(w, e->ttl);
        tb_write_u32(w, e->shrink_ticks);
        tb_write_u8(w, e->ai_state);
        tb_write_f32(w, e->progress);
    }
}

uint64_t tb_sim_checksum(const tb_sim *sim)
{
    static uint8_t buf[96 * 1024];
    tb_writer w;
    tb_writer_init(&w, buf, sizeof(buf));
    tb_sim_write_state(sim, &w);
    return tb_fnv1a64(buf, w.pos);
}

void tb_sim_results_text(const tb_sim *sim, int slot, char *out, size_t out_len)
{
    const tb_player *p = &sim->players[slot];
    int floppies = (p->floppies & 1) + ((p->floppies >> 1) & 1) + ((p->floppies >> 2) & 1);
    snprintf(out, out_len, "Memory Bauble: %s\nFloppies %d/%d\nGolden Joystick: %s",
             p->bauble ? "found" : "missing", floppies, TB_FLOPPIES, sim->challenge_done ? "won" : "not yet");
}
