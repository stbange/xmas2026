/* SPDX-License-Identifier: MIT */
#include "core/mathx.h"

#include <math.h>

float tb_clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

float tb_lerpf(float a, float b, float t)
{
    return a + (b - a) * t;
}

float tb_wrap_anglef(float a)
{
    const float two_pi = 2.0f * TB_PI;
    float r = fmodf(a + TB_PI, two_pi);
    if (r < 0.0f) {
        r += two_pi;
    }
    return r - TB_PI;
}

bool tb_approx_eqf(float a, float b, float eps)
{
    return fabsf(a - b) <= eps;
}

float tb_sign(float v)
{
    return (float)((v > 0.0f) - (v < 0.0f));
}

tb_vec3 tb_v3(float x, float y, float z)
{
    tb_vec3 v = {x, y, z};
    return v;
}

tb_vec3 tb_v3_add(tb_vec3 a, tb_vec3 b)
{
    return tb_v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

tb_vec3 tb_v3_sub(tb_vec3 a, tb_vec3 b)
{
    return tb_v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

tb_vec3 tb_v3_scale(tb_vec3 a, float s)
{
    return tb_v3(a.x * s, a.y * s, a.z * s);
}

float tb_v3_dot(tb_vec3 a, tb_vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

tb_vec3 tb_v3_cross(tb_vec3 a, tb_vec3 b)
{
    return tb_v3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float tb_v3_len(tb_vec3 a)
{
    return sqrtf(tb_v3_dot(a, a));
}

tb_vec3 tb_v3_normalize(tb_vec3 a)
{
    float len = tb_v3_len(a);
    if (len <= 1e-8f) {
        return tb_v3(0.0f, 0.0f, 0.0f);
    }
    return tb_v3_scale(a, 1.0f / len);
}

tb_vec3 tb_v3_lerp(tb_vec3 a, tb_vec3 b, float t)
{
    return tb_v3(tb_lerpf(a.x, b.x, t), tb_lerpf(a.y, b.y, t), tb_lerpf(a.z, b.z, t));
}

bool tb_v3_approx_eq(tb_vec3 a, tb_vec3 b, float eps)
{
    return tb_approx_eqf(a.x, b.x, eps) && tb_approx_eqf(a.y, b.y, eps) && tb_approx_eqf(a.z, b.z, eps);
}
