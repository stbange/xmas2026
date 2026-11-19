/* SPDX-License-Identifier: MIT */
/* What the session looks like: the static world mesh (hall or level) and
 * the per-frame scene (mood lighting, portals, baubles on the tree). */
#ifndef TB_APP_WORLD_VIEW_H
#define TB_APP_WORLD_VIEW_H

#include "app/app.h"
#include "render/mesh.h"
#include "render/scene.h"

/* Rebuilds the static world mesh for the current flow state. */
bool tb_world_mesh_build(const tb_app *app, tb_mesh *mesh);
/* Fills the scene for this frame (lights, camera, dynamic boxes). */
void tb_world_scene(const tb_app *app, tb_scene *scene, float fov_h_radians, float aspect);
/* The voxel Christmas tree (dims 32 x 32 x 48, 0.25 m voxels). */
void tb_world_tree_voxels(tb_voxels *out);

#endif
