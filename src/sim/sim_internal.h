/* SPDX-License-Identifier: MIT */
/* Helpers shared between the simulation's translation units. */
#ifndef TB_SIM_INTERNAL_H
#define TB_SIM_INTERNAL_H

#include "sim/sim.h"

#define TB_DT (1.0f / (float)TB_SIM_HZ_TICKS)

tb_handle tb__handle(int index, uint16_t gen);
int tb__index(const tb_sim *sim, const tb_entity *e);
void tb__entity_box(const tb_entity *e, tb_vec3 *lo, tb_vec3 *hi);
void tb__player_box(const tb_player *p, tb_vec3 *lo, tb_vec3 *hi);
tb_vec3 tb__eye(const tb_player *p);
tb_vec3 tb__view_dir(float yaw, float pitch);
/* True if the entity type takes damage (targets, enemies). */
bool tb__damageable(const tb_entity *e);
/* Applies damage/effects to an entity; kills it (with a KILLED event for
 * named enemies) when health runs out. */
void tb__damage_entity(tb_sim *sim, tb_entity *e, int32_t amount, uint32_t stun, uint32_t shrink);
/* Spawns a projectile. owner_slot >= 0: a player's; -1: an enemy's. */
tb_entity *tb__spawn_projectile(tb_sim *sim, tb_vec3 at, tb_vec3 velocity, int owner_slot, int32_t damage);
/* World line of sight between two points. */
bool tb__line_of_sight(const tb_sim *sim, tb_vec3 a, tb_vec3 b);

void tb__world_update(tb_sim *sim);
void tb__player_world(tb_sim *sim, int slot, const tb_input *in);
void tb__ai_update(tb_sim *sim);

#endif
