/* SPDX-License-Identifier: MIT */
/* Scalar and small-vector helpers used by the simulation. Matrix-heavy
 * rendering math uses cglm directly. */
#ifndef TB_CORE_MATHX_H
#define TB_CORE_MATHX_H

#include <stdbool.h>

#define TB_PI 3.14159265358979323846f

typedef struct tb_vec3 {
    float x, y, z;
} tb_vec3;

float tb_clampf(float v, float lo, float hi);
float tb_lerpf(float a, float b, float t);
/* Wraps an angle in radians into [-pi, pi). */
float tb_wrap_anglef(float a);
bool tb_approx_eqf(float a, float b, float eps);
float tb_sign(float v);

tb_vec3 tb_v3(float x, float y, float z);
tb_vec3 tb_v3_add(tb_vec3 a, tb_vec3 b);
tb_vec3 tb_v3_sub(tb_vec3 a, tb_vec3 b);
tb_vec3 tb_v3_scale(tb_vec3 a, float s);
float tb_v3_dot(tb_vec3 a, tb_vec3 b);
tb_vec3 tb_v3_cross(tb_vec3 a, tb_vec3 b);
float tb_v3_len(tb_vec3 a);
/* Returns the zero vector for (near) zero-length input. */
tb_vec3 tb_v3_normalize(tb_vec3 a);
tb_vec3 tb_v3_lerp(tb_vec3 a, tb_vec3 b, float t);
bool tb_v3_approx_eq(tb_vec3 a, tb_vec3 b, float eps);

#endif
