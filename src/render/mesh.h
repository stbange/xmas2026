/* SPDX-License-Identifier: MIT */
/* CPU-side triangle meshes and the builders that produce them: convex
 * brushes -> polygons, voxel volumes -> greedy-merged quads. */
#ifndef TB_RENDER_MESH_H
#define TB_RENDER_MESH_H

#include "core/dynarr.h"
#include "sim/geometry.h"

typedef struct tb_vertex {
    float pos[3];
    float normal[3];
    uint8_t color[4];
} tb_vertex;

typedef struct tb_mesh {
    tb_dynarr vertices; /* tb_vertex, three per triangle */
} tb_mesh;

void tb_mesh_init(tb_mesh *m, const tb_allocator *alloc);
void tb_mesh_free(tb_mesh *m);
size_t tb_mesh_triangle_count(const tb_mesh *m);
/* Appends a convex polygon (fan-triangulated). */
bool tb_mesh_add_polygon(tb_mesh *m, const tb_vec3 *points, int count, tb_vec3 normal,
                         const uint8_t color[4]);

/* Builds the visible faces of one convex brush. Faces with no area (bevel
 * planes) are skipped. Returns false on allocation failure. */
bool tb_mesh_add_brush(tb_mesh *m, const tb_brush *b, const uint8_t color[4]);
/* All brushes of a geometry; colour chosen by face orientation (floors,
 * walls, ceilings) from `palette_floor/wall/ceiling`. */
bool tb_mesh_add_geometry(tb_mesh *m, const tb_geometry *g, const uint8_t floor[4], const uint8_t wall[4],
                          const uint8_t ceiling[4]);

/* Voxel volume: dims x*y*z, one byte per voxel (0 = empty, else palette
 * index), x fastest. Quads of equal colour are merged greedily. */
typedef struct tb_voxels {
    int sx, sy, sz;
    const uint8_t *data;
    float voxel_size;
    tb_vec3 origin;
    const uint8_t (*palette)[4]; /* 256 RGBA colours */
} tb_voxels;

bool tb_mesh_add_voxels(tb_mesh *m, const tb_voxels *v);

#endif
