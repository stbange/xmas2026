/* SPDX-License-Identifier: MIT */
#include "render/mesh.h"

#include <math.h>
#include <string.h>

#define MAX_POLY 32
#define BIG      4096.0f

void tb_mesh_init(tb_mesh *m, const tb_allocator *alloc)
{
    tb_dynarr_init(&m->vertices, alloc, sizeof(tb_vertex));
}

void tb_mesh_free(tb_mesh *m)
{
    tb_dynarr_free(&m->vertices);
}

size_t tb_mesh_triangle_count(const tb_mesh *m)
{
    return m->vertices.len / 3;
}

static bool push_vertex(tb_mesh *m, tb_vec3 p, tb_vec3 n, const uint8_t color[4])
{
    tb_vertex v = {{p.x, p.y, p.z}, {n.x, n.y, n.z}, {color[0], color[1], color[2], color[3]}};
    return tb_dynarr_push(&m->vertices, &v);
}

bool tb_mesh_add_polygon(tb_mesh *m, const tb_vec3 *pts, int count, tb_vec3 normal, const uint8_t color[4])
{
    for (int i = 1; i + 1 < count; i++) {
        if (!push_vertex(m, pts[0], normal, color) || !push_vertex(m, pts[i], normal, color) ||
            !push_vertex(m, pts[i + 1], normal, color)) {
            return false;
        }
    }
    return true;
}

/* Clips a polygon to the back side of plane (n, d). */
static int clip_polygon(const tb_vec3 *in, int count, tb_vec3 n, float d, tb_vec3 *out)
{
    int o = 0;
    for (int i = 0; i < count; i++) {
        tb_vec3 a = in[i], b = in[(i + 1) % count];
        float da = tb_v3_dot(n, a) - d, db = tb_v3_dot(n, b) - d;
        if (da <= 0.0f) {
            out[o++] = a;
        }
        if ((da < 0.0f && db > 0.0f) || (da > 0.0f && db < 0.0f)) {
            out[o++] = tb_v3_lerp(a, b, da / (da - db));
        }
        if (o >= MAX_POLY - 1) {
            break;
        }
    }
    return o;
}

static float polygon_area(const tb_vec3 *p, int count)
{
    tb_vec3 sum = tb_v3(0, 0, 0);
    for (int i = 1; i + 1 < count; i++) {
        sum = tb_v3_add(sum, tb_v3_cross(tb_v3_sub(p[i], p[0]), tb_v3_sub(p[i + 1], p[0])));
    }
    return 0.5f * tb_v3_len(sum);
}

static int brush_face(const tb_brush *b, int face, tb_vec3 *poly)
{
    tb_vec3 n = b->planes[face].n;
    float d = b->planes[face].d;
    /* Large quad on the plane, wound counter-clockwise seen from outside. */
    tb_vec3 up = fabsf(n.z) > 0.9f ? tb_v3(1, 0, 0) : tb_v3(0, 0, 1);
    tb_vec3 u = tb_v3_normalize(tb_v3_cross(up, n));
    tb_vec3 v = tb_v3_cross(n, u);
    tb_vec3 c = tb_v3_scale(n, d);
    tb_vec3 buf_a[MAX_POLY], buf_b[MAX_POLY];
    buf_a[0] = tb_v3_add(c, tb_v3_add(tb_v3_scale(u, -BIG), tb_v3_scale(v, -BIG)));
    buf_a[1] = tb_v3_add(c, tb_v3_add(tb_v3_scale(u, BIG), tb_v3_scale(v, -BIG)));
    buf_a[2] = tb_v3_add(c, tb_v3_add(tb_v3_scale(u, BIG), tb_v3_scale(v, BIG)));
    buf_a[3] = tb_v3_add(c, tb_v3_add(tb_v3_scale(u, -BIG), tb_v3_scale(v, BIG)));
    int count = 4;
    tb_vec3 *src = buf_a, *dst = buf_b;
    for (int i = 0; i < b->count && count >= 3; i++) {
        if (i == face) {
            continue;
        }
        count = clip_polygon(src, count, b->planes[i].n, b->planes[i].d, dst);
        tb_vec3 *t = src;
        src = dst;
        dst = t;
    }
    if (count < 3 || polygon_area(src, count) < 1e-4f) {
        return 0;
    }
    memcpy(poly, src, (size_t)count * sizeof(tb_vec3));
    return count;
}

static bool add_brush_faces(tb_mesh *m, const tb_brush *b, const uint8_t floor[4], const uint8_t wall[4],
                            const uint8_t ceiling[4])
{
    tb_vec3 poly[MAX_POLY];
    for (int f = 0; f < b->count; f++) {
        /* Skip duplicate planes (e.g. a bevel equal to a real face). */
        bool dup = false;
        for (int k = 0; k < f; k++) {
            dup |= tb_v3_approx_eq(b->planes[k].n, b->planes[f].n, 1e-4f) &&
                   fabsf(b->planes[k].d - b->planes[f].d) < 1e-4f;
        }
        int count = dup ? 0 : brush_face(b, f, poly);
        tb_vec3 n = b->planes[f].n;
        const uint8_t *c = n.z > 0.7f ? floor : (n.z < -0.7f ? ceiling : wall);
        if (count && !tb_mesh_add_polygon(m, poly, count, n, c)) {
            return false;
        }
    }
    return true;
}

bool tb_mesh_add_brush(tb_mesh *m, const tb_brush *b, const uint8_t color[4])
{
    return add_brush_faces(m, b, color, color, color);
}

bool tb_mesh_add_geometry(tb_mesh *m, const tb_geometry *g, const uint8_t floor[4], const uint8_t wall[4],
                          const uint8_t ceiling[4])
{
    for (int i = 0; i < g->count; i++) {
        if (!add_brush_faces(m, &g->brushes[i], floor, wall, ceiling)) {
            return false;
        }
    }
    return true;
}

/* ---- voxels: greedy meshing (per face direction, per slice) ---------- */

static uint8_t voxel_at(const tb_voxels *v, int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || x >= v->sx || y >= v->sy || z >= v->sz) {
        return 0;
    }
    return v->data[(z * v->sy + y) * v->sx + x];
}

bool tb_mesh_add_voxels(tb_mesh *m, const tb_voxels *v)
{
    const int dims[3] = {v->sx, v->sy, v->sz};
    static uint8_t mask[256 * 256];
    for (int axis = 0; axis < 3; axis++) { /* every slice must fit the mask */
        if (dims[(axis + 1) % 3] * dims[(axis + 2) % 3] > (int)sizeof(mask)) {
            return false;
        }
    }
    for (int axis = 0; axis < 3; axis++) {
        int u = (axis + 1) % 3, w = (axis + 2) % 3;
        for (int dir = -1; dir <= 1; dir += 2) {
            for (int slice = 0; slice < dims[axis]; slice++) {
                /* Mask of faces in this slice that are visible in `dir`. */
                for (int b = 0; b < dims[w]; b++) {
                    for (int a = 0; a < dims[u]; a++) {
                        int p[3], q[3];
                        p[axis] = slice;
                        p[u] = a;
                        p[w] = b;
                        memcpy(q, p, sizeof(q));
                        q[axis] += dir;
                        uint8_t here = voxel_at(v, p[0], p[1], p[2]);
                        mask[b * dims[u] + a] = (here && !voxel_at(v, q[0], q[1], q[2])) ? here : 0;
                    }
                }
                /* Greedy: grow rectangles of equal colour. */
                for (int b = 0; b < dims[w]; b++) {
                    for (int a = 0; a < dims[u];) {
                        uint8_t c = mask[b * dims[u] + a];
                        if (!c) {
                            a++;
                            continue;
                        }
                        int wa = 1;
                        while (a + wa < dims[u] && mask[b * dims[u] + a + wa] == c) {
                            wa++;
                        }
                        int hb = 1;
                        for (; b + hb < dims[w]; hb++) {
                            int k = 0;
                            while (k < wa && mask[(b + hb) * dims[u] + a + k] == c) {
                                k++;
                            }
                            if (k < wa) {
                                break;
                            }
                        }
                        for (int y2 = 0; y2 < hb; y2++) {
                            memset(&mask[(b + y2) * dims[u] + a], 0, (size_t)wa);
                        }
                        float plane = (float)(slice + (dir > 0 ? 1 : 0));
                        float corners[4][3];
                        const int du[4] = {0, wa, wa, 0}, dw[4] = {0, 0, hb, hb};
                        for (int k = 0; k < 4; k++) {
                            corners[k][axis] = plane;
                            corners[k][u] = (float)(a + du[k]);
                            corners[k][w] = (float)(b + dw[k]);
                        }
                        tb_vec3 pts[4];
                        for (int k = 0; k < 4; k++) {
                            int src = dir > 0 ? k : 3 - k; /* keep outward winding */
                            pts[k] = tb_v3_add(v->origin, tb_v3_scale(tb_v3(corners[src][0], corners[src][1],
                                                                            corners[src][2]),
                                                                      v->voxel_size));
                        }
                        float nv[3] = {0, 0, 0};
                        nv[axis] = (float)dir;
                        if (!tb_mesh_add_polygon(m, pts, 4, tb_v3(nv[0], nv[1], nv[2]), v->palette[c])) {
                            return false;
                        }
                        a += wa;
                    }
                }
            }
        }
    }
    return true;
}
