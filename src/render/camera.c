/* SPDX-License-Identifier: MIT */
#include "render/camera.h"

#include <math.h>
#include <string.h>

#define M(c, r) m[(c) * 4 + (r)]

tb_mat4 tb_mat4_identity(void)
{
    tb_mat4 r;
    memset(&r, 0, sizeof(r));
    r.M(0, 0) = r.M(1, 1) = r.M(2, 2) = r.M(3, 3) = 1.0f;
    return r;
}

tb_mat4 tb_mat4_mul(tb_mat4 a, tb_mat4 b)
{
    tb_mat4 r;
    for (int c = 0; c < 4; c++) {
        for (int row = 0; row < 4; row++) {
            float s = 0.0f;
            for (int k = 0; k < 4; k++) {
                s += a.M(k, row) * b.M(c, k);
            }
            r.M(c, row) = s;
        }
    }
    return r;
}

tb_mat4 tb_mat4_perspective(float fov_y, float aspect, float n, float f)
{
    tb_mat4 r;
    memset(&r, 0, sizeof(r));
    float t = 1.0f / tanf(fov_y * 0.5f);
    r.M(0, 0) = t / aspect;
    r.M(1, 1) = t;
    r.M(2, 2) = (f + n) / (n - f);
    r.M(2, 3) = -1.0f;
    r.M(3, 2) = 2.0f * f * n / (n - f);
    return r;
}

tb_mat4 tb_mat4_ortho(float l, float r, float b, float t, float n, float f)
{
    tb_mat4 o = tb_mat4_identity();
    o.M(0, 0) = 2.0f / (r - l);
    o.M(1, 1) = 2.0f / (t - b);
    o.M(2, 2) = -2.0f / (f - n);
    o.M(3, 0) = -(r + l) / (r - l);
    o.M(3, 1) = -(t + b) / (t - b);
    o.M(3, 2) = -(f + n) / (f - n);
    return o;
}

tb_mat4 tb_mat4_look_at(tb_vec3 eye, tb_vec3 target, tb_vec3 up)
{
    tb_vec3 f = tb_v3_normalize(tb_v3_sub(target, eye));
    tb_vec3 s = tb_v3_normalize(tb_v3_cross(f, up));
    if (tb_v3_len(s) < 1e-6f) { /* looking straight along `up` */
        s = tb_v3_normalize(tb_v3_cross(f, tb_v3(1, 0, 0)));
    }
    tb_vec3 u = tb_v3_cross(s, f);
    tb_mat4 r = tb_mat4_identity();
    r.M(0, 0) = s.x;
    r.M(1, 0) = s.y;
    r.M(2, 0) = s.z;
    r.M(0, 1) = u.x;
    r.M(1, 1) = u.y;
    r.M(2, 1) = u.z;
    r.M(0, 2) = -f.x;
    r.M(1, 2) = -f.y;
    r.M(2, 2) = -f.z;
    r.M(3, 0) = -tb_v3_dot(s, eye);
    r.M(3, 1) = -tb_v3_dot(u, eye);
    r.M(3, 2) = tb_v3_dot(f, eye);
    return r;
}

tb_vec3 tb_mat4_project(tb_mat4 m, tb_vec3 p)
{
    float x = m.M(0, 0) * p.x + m.M(1, 0) * p.y + m.M(2, 0) * p.z + m.M(3, 0);
    float y = m.M(0, 1) * p.x + m.M(1, 1) * p.y + m.M(2, 1) * p.z + m.M(3, 1);
    float z = m.M(0, 2) * p.x + m.M(1, 2) * p.y + m.M(2, 2) * p.z + m.M(3, 2);
    float w = m.M(0, 3) * p.x + m.M(1, 3) * p.y + m.M(2, 3) * p.z + m.M(3, 3);
    float inv = fabsf(w) > 1e-12f ? 1.0f / w : 1.0f;
    return tb_v3(x * inv, y * inv, z * inv);
}

tb_vec3 tb_camera_forward(const tb_camera *c)
{
    float cp = cosf(c->pitch);
    return tb_v3(cosf(c->yaw) * cp, sinf(c->yaw) * cp, sinf(c->pitch));
}

tb_mat4 tb_camera_view(const tb_camera *c)
{
    return tb_mat4_look_at(c->eye, tb_v3_add(c->eye, tb_camera_forward(c)), tb_v3(0, 0, 1));
}

tb_mat4 tb_camera_projection(const tb_camera *c, float aspect)
{
    return tb_mat4_perspective(c->fov_y, aspect, c->near_z, c->far_z);
}

float tb_fov_horizontal_to_vertical(float h, float aspect)
{
    return 2.0f * atanf(tanf(h * 0.5f) / aspect);
}
