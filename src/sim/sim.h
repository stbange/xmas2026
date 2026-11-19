/* SPDX-License-Identifier: MIT */
/* The deterministic game simulation: players, entities, combat, pickups,
 * checkpoints and level progress. Advanced by tb_sim_step at 60 Hz with one
 * tb_input per player. No I/O, no wall clock: identical inputs give
 * identical results (replays, network prediction, bots). */
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

/* Co-op revive rules (see player-core spec). */
#define TB_REVIVE_WINDOW_TICKS (20u * TB_SIM_HZ_TICKS) /* downed this long at most */
#define TB_REVIVE_HOLD_TICKS   (3u * TB_SIM_HZ_TICKS)  /* hold Use this long */
#define TB_REVIVE_RANGE        1.5f
#define TB_REVIVE_HEALTH       30
#define TB_COOP_RESPAWN_TICKS  (10u * TB_SIM_HZ_TICKS) /* after the window ran out */
#define TB_SOLO_RESPAWN_TICKS  (2u * TB_SIM_HZ_TICKS)

typedef uint32_t tb_handle; /* (generation << 16) | index; 0 = none */

typedef enum tb_entity_type {
    TB_ENT_NONE = 0,
    TB_ENT_TARGET, /* damageable dummy (tests, shooting galleries) */
    TB_ENT_PROJECTILE,
    TB_ENT_PICKUP,
    TB_ENT_TRIGGER /* checkpoint / exit volumes */
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

typedef enum tb_trigger_kind { TB_TRIGGER_CHECKPOINT = 0, TB_TRIGGER_EXIT = 1 } tb_trigger_kind;

typedef struct tb_entity {
    uint16_t gen;
    uint8_t type; /* tb_entity_type */
    uint8_t kind; /* pickup / trigger kind */
    tb_vec3 origin;
    tb_vec3 velocity;
    tb_vec3 mins, maxs;
    tb_vitals vitals;
    int32_t value;
    int32_t amount;
    bool flag;
    tb_handle owner; /* projectiles: who fired (players use handle = player slot + 1) */
    int32_t damage;  /* projectiles: direct damage */
    int32_t splash_damage;
    float splash_radius;
    uint32_t stun_ticks; /* projectiles: applied on hit; targets: remaining stun */
    uint32_t ttl;        /* projectiles: ticks to live */
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
    bool completed;
    uint8_t floppies; /* floppies counted for this player */
    bool bauble;      /* present when the bauble was collected */
    uint32_t deaths;
    uint16_t last_buttons;
} tb_player;

typedef struct tb_sim_config {
    const tb_geometry *geometry;
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
