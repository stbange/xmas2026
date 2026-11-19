/* SPDX-License-Identifier: MIT */
/* 4x4 matrices (column-major, OpenGL clip space) and the first-person
 * camera. World: metres, Z up, yaw 0 = +X, pitch > 0 = up. */
#ifndef TB_RENDER_CAMERA_H
#define TB_RENDER_CAMERA_H

#include "core/mathx.h"

typedef struct tb_mat4 {
    float m[16];
} tb_mat4;

tb_mat4 tb_mat4_identity(void);
tb_mat4 tb_mat4_mul(tb_mat4 a, tb_mat4 b);
tb_mat4 tb_mat4_perspective(float fov_y_radians, float aspect, float near_z, float far_z);
tb_mat4 tb_mat4_ortho(float l, float r, float b, float t, float n, float f);
tb_mat4 tb_mat4_look_at(tb_vec3 eye, tb_vec3 target, tb_vec3 up);
/* Transforms a point (w = 1) and divides by w. */
tb_vec3 tb_mat4_project(tb_mat4 m, tb_vec3 p);

typedef struct tb_camera {
    tb_vec3 eye;
    float yaw, pitch;
    float fov_y; /* radians */
    float near_z, far_z;
} tb_camera;

tb_vec3 tb_camera_forward(const tb_camera *c);
tb_mat4 tb_camera_view(const tb_camera *c);
tb_mat4 tb_camera_projection(const tb_camera *c, float aspect);
/* Converts a horizontal FOV (the setting players know) to vertical. */
float tb_fov_horizontal_to_vertical(float h_radians, float aspect);

#endif
