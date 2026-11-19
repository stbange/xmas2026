/* SPDX-License-Identifier: MIT */
/* The Great Hall of Saved Games: layout, collision geometry, props and the
 * 24 advent doors. Built in code until the map compiler exists; the same
 * data drives collision, rendering and the HUD. Metres, Z up, origin at the
 * centre of the hall floor, north = +Y. */
#ifndef TB_GAME_HALL_H
#define TB_GAME_HALL_H

#include "game/calendar.h"
#include "game/levels.h"
#include "sim/sim.h"

#define TB_HALL_MAX_BRUSHES 256
#define TB_HALL_MAX_PROPS   48
#define TB_HALL_BALCONY_Z   5.0f
#define TB_HALL_DOOR_HEIGHT 3.2f

typedef struct tb_hall_door {
    int day;
    tb_vec3 centre;           /* on the floor, in front of the wall */
    tb_vec3 normal;           /* points into the hall */
    tb_vec3 vol_min, vol_max; /* the portal volume (blocked while closed) */
    tb_vec3 spawn;            /* where a player stands after leaving this portal */
    float spawn_yaw;
} tb_hall_door;

typedef enum tb_prop_kind {
    TB_PROP_DECOR = 0,
    TB_PROP_NOEL_DESK,
    TB_PROP_TROPHY_WALL,
    TB_PROP_JUKEBOX,
    TB_PROP_COCOA_BAR,
    TB_PROP_LAN_TABLE,
    TB_PROP_CRT,
    TB_PROP_ARENA,
    TB_PROP_BIOS_GATE
} tb_prop_kind;

typedef struct tb_hall_prop {
    tb_prop_kind kind;
    tb_vec3 mins, maxs;
    uint8_t color[4];
    bool solid;
    bool emissive;
} tb_hall_prop;

typedef struct tb_hall {
    tb_hall_door doors[TB_PORTAL_COUNT];
    tb_hall_prop props[TB_HALL_MAX_PROPS];
    int prop_count;
    tb_vec3 spawn; /* first start: in front of the BIOS gate */
    float spawn_yaw;
    tb_vec3 tree_base;
    float tree_height;
    float tree_radius;
    tb_vec3 arena_min, arena_max; /* the Snowball Arena (west wing) */
} tb_hall;

void tb_hall_layout(tb_hall *h);
/* Collision geometry for the current calendar: closed doors are blocked.
 * `storage` must hold TB_HALL_MAX_BRUSHES brushes. */
bool tb_hall_build_geometry(const tb_hall *h, tb_portal_mask open, tb_geometry *g, tb_brush *storage);
/* Adds portal triggers (id = day) for every open door. */
void tb_hall_add_portals(const tb_hall *h, tb_portal_mask open, tb_sim *sim);
/* The door nearest to `pos` within `range` metres (horizontal), or NULL. */
const tb_hall_door *tb_hall_nearest_door(const tb_hall *h, tb_vec3 pos, float range);
/* Light rings lit on the tree = number of baubles hung (completed levels). */
int tb_hall_tree_baubles(uint32_t completed_mask);
#define TB_TREE_RING_BULBS 6
/* Bulb b (0..5) of light ring n (0..23), bottom ring first. */
tb_vec3 tb_hall_ring_bulb_pos(const tb_hall *h, int ring, int bulb);
/* Position of bauble n (0..23) on the tree, deterministic. */
tb_vec3 tb_hall_bauble_pos(const tb_hall *h, int n);

typedef struct tb_hall_mood {
    tb_vec3 ambient, sun_color, fog_color;
    float tree_light; /* 0..1 brightness of the tree's lights */
    bool emergency;   /* red emergency lighting (SHUTDOWN) */
} tb_hall_mood;

tb_hall_mood tb_hall_mood_for(tb_story_act act, int baubles);

#endif
