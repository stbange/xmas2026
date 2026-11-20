/* SPDX-License-Identifier: MIT */
/* The deterministic game simulation: players, entities, combat, enemies,
 * pickups, level wiring (named entities, triggers, movers, usables) and
 * level progress. Advanced by tb_sim_step at 60 Hz with one tb_input per
 * player. No I/O, no wall clock: identical inputs give identical results
 * (replays, network prediction, bots). */
#ifndef TB_SIM_SIM_H
#define TB_SIM_SIM_H

#include "core/rng.h"
#include "core/serial.h"
#include "sim/combat.h"
#include "sim/geometry.h"
#include "sim/inventory.h"
#include "sim/movement.h"

#define TB_SIM_HZ_TICKS 60u
#define TB_MAX_PLAYERS  4
#define TB_MAX_ENTITIES 256
#define TB_FLOPPIES     3
#define TB_NAME_LEN     16
#define TB_MAX_EVENTS   32
#define TB_LIGHT_GROUPS 32

/* Co-op revive rules (see player-core spec and design note 14). */
#define TB_REVIVE_WINDOW_TICKS (20u * TB_SIM_HZ_TICKS) /* downed this long at most */
#define TB_REVIVE_HOLD_TICKS   (3u * TB_SIM_HZ_TICKS)  /* hold Use this long */
#define TB_REVIVE_RANGE        1.5f
#define TB_REVIVE_HEALTH       30
#define TB_COOP_RESPAWN_TICKS  (10u * TB_SIM_HZ_TICKS) /* after the window ran out */
#define TB_SOLO_RESPAWN_TICKS  (2u * TB_SIM_HZ_TICKS)

/* Day-1 items. */
#define TB_JET_FUEL_MAX   1000 /* one unit per tick: about 17 s of thrust */
#define TB_JET_THRUST     32.0f
#define TB_DECOY_TICKS    (8u * TB_SIM_HZ_TICKS)
#define TB_DECOY_COOLDOWN (15u * TB_SIM_HZ_TICKS)
#define TB_USE_RANGE      2.0f

typedef uint32_t tb_handle; /* (generation << 16) | index; 0 = none */

typedef enum tb_entity_type {
    TB_ENT_NONE = 0,
    TB_ENT_TARGET, /* damageable dummy (tests, shooting galleries) */
    TB_ENT_PROJECTILE,
    TB_ENT_PICKUP,
    TB_ENT_TRIGGER, /* volumes: checkpoint, exit, portal, relay, shrink zone, hurt */
    TB_ENT_ENEMY,
    TB_ENT_USABLE, /* switches, toilets, vending machines, arcade cabinets */
    TB_ENT_MOVER,  /* a level brush that slides open and closed */
    TB_ENT_DECOY   /* the Holo-Santa */
} tb_entity_type;

typedef enum tb_pickup_kind {
    TB_PICKUP_HEALTH = 0, /* amount; overcharge if `flag` */
    TB_PICKUP_ARMOUR,
    TB_PICKUP_WEAPON, /* value = tb_weapon_id */
    TB_PICKUP_AMMO,   /* value = tb_ammo_type, amount */
    TB_PICKUP_ITEM,   /* value = tb_item_id, flag = loaner */
    TB_PICKUP_BAUBLE,
    TB_PICKUP_FLOPPY /* value = 0..2 */
} tb_pickup_kind;

typedef enum tb_trigger_kind {
    TB_TRIGGER_CHECKPOINT = 0,
    TB_TRIGGER_EXIT = 1,
    TB_TRIGGER_PORTAL = 2, /* id = advent day */
    TB_TRIGGER_RELAY = 3,  /* reports its name and fires its target when entered (once if flag) */
    TB_TRIGGER_SHRINK = 4, /* players inside are mouse-sized */
    TB_TRIGGER_HURT = 5    /* `value` damage per tick (chasms: kill) */
} tb_trigger_kind;

typedef enum tb_usable_kind {
    TB_USE_SWITCH = 0, /* toggles light group `value`, fires target */
    TB_USE_TOILET,     /* +1 health */
    TB_USE_VENDING,    /* +5 health (cola) */
    TB_USE_ARCADE,     /* reports a play */
    TB_USE_BUTTON,     /* fires target */
    TB_USE_KIND_COUNT
} tb_usable_kind;

typedef enum tb_event_type {
    TB_EVENT_TRIGGER = 0, /* a relay trigger was entered */
    TB_EVENT_USED,        /* a usable was used (value = kind) */
    TB_EVENT_KILLED,      /* a named enemy died */
    TB_EVENT_PICKUP       /* a pickup was taken (value = kind) */
} tb_event_type;

typedef struct tb_event {
    uint8_t type;
    int8_t player;
    int32_t value;
    char name[TB_NAME_LEN];
} tb_event;

typedef enum tb_ai_state { TB_AI_IDLE = 0, TB_AI_CHASE } tb_ai_state;

typedef struct tb_entity {
    uint16_t gen;
    uint8_t type; /* tb_entity_type */
    uint8_t kind; /* pickup / trigger / usable / enemy kind */
    tb_vec3 origin;
    tb_vec3 velocity;
    tb_vec3 mins, maxs;
    tb_vitals vitals;
    int32_t value;
    int32_t amount;
    bool flag;
    tb_handle owner; /* projectiles: who fired (players use handle = player slot + 1) */
    bool from_enemy; /* projectiles fired by enemies hurt players */
    int32_t damage;  /* projectiles: direct damage */
    int32_t splash_damage;
    float splash_radius;
    uint32_t stun_ticks; /* projectiles: applied on hit; targets/enemies: remaining stun */
    uint32_t ttl;        /* projectiles, decoys: ticks to live */
    bool gravity, bounce, remote;
    char name[TB_NAME_LEN];
    char target[TB_NAME_LEN];
    /* Enemies. */
    tb_move_state move;
    tb_vec3 base_half;
    uint32_t shrink_ticks;
    uint32_t cooldown, cooldown2, ai_timer;
    uint8_t ai_state;
    int8_t ai_target; /* player slot, -2 = decoy, -1 = none */
    /* Movers. */
    float progress, speed;
    tb_vec3 offset;
    bool opening;
} tb_entity;

typedef enum tb_player_state { TB_PLAYER_ALIVE = 0, TB_PLAYER_DOWNED, TB_PLAYER_DEAD } tb_player_state;

typedef struct tb_checkpoint {
    bool valid;
    int32_t id;
    tb_vec3 origin;
    float yaw;
    tb_vitals vitals;
    int32_t ammo[TB_AMMO_COUNT];
} tb_checkpoint;

typedef struct tb_player {
    bool active;
    uint8_t state; /* tb_player_state */
    tb_move_state move;
    tb_vitals vitals;
    tb_inventory inv;
    tb_weapon_state weapons[TB_WEAPON_COUNT];
    uint32_t state_ticks;     /* ticks spent downed / dead */
    uint32_t revive_progress; /* ticks a teammate has held Use */
    tb_checkpoint checkpoint;
    bool at_exit;
    int32_t portal; /* advent day of the portal touched this tick, 0 = none */
    bool completed;
    uint8_t floppies; /* floppies counted for this player */
    bool bauble;      /* present when the bauble was collected */
    uint32_t deaths;
    uint16_t last_buttons;
    int32_t jet_fuel;
    bool jetting; /* thrusting this tick: the jetpack also steers */
    uint32_t decoy_cooldown;
    uint32_t use_cooldown;
    uint32_t interactions; /* bit per tb_usable_kind used */
    bool in_shrink_zone;
} tb_player;

typedef struct tb_sim_config {
    tb_geometry *geometry; /* movers change brushes, so not const */
    uint64_t seed;
    tb_difficulty difficulty;
    bool body_replaced; /* level replaces the player's body (mech, ship) */
    tb_vec3 spawn[TB_MAX_PLAYERS];
    float spawn_yaw;
} tb_sim_config;

typedef struct tb_sim {
    tb_sim_config cfg;
    tb_move_params move_params;
    tb_rng rng;
    uint64_t tick;
    tb_player players[TB_MAX_PLAYERS];
    tb_entity entities[TB_MAX_ENTITIES];
    bool bauble_collected;
    bool challenge_done;
    bool level_complete;
    uint32_t light_groups; /* bit n: light group n is on */
    uint32_t shake_ticks;  /* camera shake (earthquakes, explosions) */
    tb_event events[TB_MAX_EVENTS];
    int event_count; /* events of the last tick (cleared at the start of each step) */
} tb_sim;

void tb_sim_init(tb_sim *sim, const tb_sim_config *cfg);
/* Adds a player in slot 0..3 at its spawn point; returns false if taken. */
bool tb_sim_add_player(tb_sim *sim, int slot);
int tb_sim_player_count(const tb_sim *sim);

tb_handle tb_sim_spawn(tb_sim *sim, tb_entity_type type, tb_vec3 origin, tb_vec3 half_extents);
tb_entity *tb_sim_get(tb_sim *sim, tb_handle h);
void tb_sim_remove(tb_sim *sim, tb_handle h);
tb_handle tb_sim_add_target(tb_sim *sim, tb_vec3 origin, int32_t health);
tb_handle tb_sim_add_pickup(tb_sim *sim, tb_pickup_kind kind, tb_vec3 origin, int32_t value, int32_t amount,
                            bool flag);
tb_handle tb_sim_add_trigger(tb_sim *sim, tb_trigger_kind kind, tb_vec3 mins, tb_vec3 maxs, int32_t id);

/* ---- level wiring (sim/world.c) ---- */
void tb_sim_set_name(tb_sim *sim, tb_handle h, const char *name, const char *target);
/* First entity with this name, or 0. */
tb_handle tb_sim_find(tb_sim *sim, const char *name);
/* Fires a target: movers toggle, relays fire their own targets, enemies
 * named so wake up. Returns how many entities reacted. */
int tb_sim_fire(tb_sim *sim, const char *target);
tb_handle tb_sim_add_relay(tb_sim *sim, tb_vec3 mins, tb_vec3 maxs, const char *name, const char *target,
                           bool once);
tb_handle tb_sim_add_usable(tb_sim *sim, tb_usable_kind kind, tb_vec3 mins, tb_vec3 maxs, const char *name,
                            const char *target, int32_t value);
/* A mover controls brush `brush` of the geometry (must be an axis-aligned
 * box built with tb_geometry_add_box): firing it slides the box by
 * `offset` metres at `speed` m/s, firing again slides it back. Movers
 * created with `open` start open. */
tb_handle tb_sim_add_mover(tb_sim *sim, int brush, tb_vec3 offset, float speed, const char *name, bool open);
void tb_sim_push_event(tb_sim *sim, tb_event_type type, int player, int32_t value, const char *name);
bool tb_sim_light_on(const tb_sim *sim, int group);
void tb_sim_set_light(tb_sim *sim, int group, bool on);

/* ---- enemies (sim/ai.c) ---- */
typedef enum tb_enemy_kind {
    TB_ENEMY_TROOPER = 0, /* Hogwash Trooper: alien boar mall cop with a shotgun */
    TB_ENEMY_LIZARD,      /* Elf-Lizard: teleports, shoots plasma */
    TB_ENEMY_BRAIN,       /* Brain-bauble: flies, throws psychic snowballs */
    TB_ENEMY_BATTLELORD,  /* Bargain-Bin Battlelord: day-1 boss, chaingun + present mortar */
    TB_ENEMY_KIND_COUNT
} tb_enemy_kind;

typedef struct tb_enemy_def {
    const char *name;
    int32_t health;
    float speed;
    tb_vec3 half;
    int32_t damage;
    int32_t pellets;
    uint32_t refire_ticks;
    float range;
    float projectile_speed;
    uint32_t teleport_ticks; /* 0 = never teleports */
    uint32_t mortar_ticks;   /* 0 = no mortar */
    int32_t mortar_damage;
    float sight;
    uint8_t color[4];
    bool flying;
    bool hitscan;       /* primary attack: hitscan pellets, else projectile */
    bool shrink_immune; /* bosses laugh at the Gift-Shrinker */
} tb_enemy_def;

const tb_enemy_def *tb_enemy_get(tb_enemy_kind kind);
tb_handle tb_sim_add_enemy(tb_sim *sim, tb_enemy_kind kind, tb_vec3 feet, const char *name);
int tb_sim_enemy_count(const tb_sim *sim);

/* Damage dealt to a player (scaled by difficulty, armour applies). */
void tb_sim_damage_player(tb_sim *sim, int slot, int32_t raw_amount);
/* Advances one tick. `inputs` has one entry per player slot. */
void tb_sim_step(tb_sim *sim, const tb_input inputs[TB_MAX_PLAYERS]);
/* Marks the level's Golden Joystick challenge as done (level scripts). */
void tb_sim_complete_challenge(tb_sim *sim);
/* Removes loaners from every player's inventory (leaving the level). */
void tb_sim_leave_level(tb_sim *sim);

/* Serialises the full state (deterministic byte order). */
void tb_sim_write_state(const tb_sim *sim, tb_writer *w);
uint64_t tb_sim_checksum(const tb_sim *sim);
/* Results screen summary for a player, e.g. "Memory Bauble: found\nFloppies 2/3\n..." */
void tb_sim_results_text(const tb_sim *sim, int slot, char *out, size_t out_len);

#endif
