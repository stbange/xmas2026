/* SPDX-License-Identifier: MIT */
/* OpenGL renderer: low-resolution scene target, forward lighting with point
 * lights and a shadowed moon/sun light, fog, then palette quantisation with
 * ordered dithering and nearest-neighbour upscaling to the window. */
#ifndef TB_RENDER_GL_RENDERER_H
#define TB_RENDER_GL_RENDERER_H

#include "render/gl/gl_api.h"
#include "render/mesh.h"
#include "render/palette.h"
#include "render/scene.h"
#include "render/ui.h"
#include "render/view.h"

#define TB_SHADOW_MAP_SIZE 1024

typedef struct tb_render_options {
    bool palette_quantize;
    bool dithering;
    bool shadows;
} tb_render_options;

typedef struct tb_renderer {
    tb_gl gl;
    bool gles; /* OpenGL ES 3 / WebGL 2 shader dialect */
    GLuint scene_prog, depth_prog, post_prog, ui_prog;
    GLuint ui_vao, ui_vbo;
    tb_mesh ui_mesh;
    GLuint world_vao, world_vbo;
    GLsizei world_vertices;
    GLuint dyn_vao, dyn_vbo;
    GLuint post_vao;
    GLuint scene_fbo, scene_color, scene_depth;
    GLuint post_fbo, post_color; /* palette-quantised image at internal resolution */
    int scene_w, scene_h;
    GLuint shadow_fbo, shadow_tex;
    GLuint lut_tex;
    tb_mesh dynamic; /* rebuilt per frame from the scene's boxes */
    tb_render_options options;
    char error[256];
} tb_renderer;

/* Creates GPU resources; requires a current GL context. On failure returns
 * false with a message in r->error. */
bool tb_renderer_create(tb_renderer *r, tb_gl_loader loader, bool gles);
void tb_renderer_destroy(tb_renderer *r);
bool tb_renderer_upload_world(tb_renderer *r, const tb_mesh *mesh);
void tb_renderer_set_palette(tb_renderer *r, const tb_palette *p);
/* Renders the scene into the low-res target and presents it upscaled into
 * the window's default framebuffer. */
void tb_renderer_draw(tb_renderer *r, const tb_scene *scene, const tb_view_layout *layout, int window_w,
                      int window_h);
/* Draws the UI over the presented frame (UI pixels map onto the
 * letterboxed viewport). Call after tb_renderer_draw, before swapping. */
void tb_renderer_draw_ui(tb_renderer *r, const tb_ui *ui, const tb_view_layout *layout);
/* Reads back the low-res scene image after post-processing (tests). RGBA,
 * bottom-up rows; out must hold internal_w * internal_h * 4 bytes. */
bool tb_renderer_read_scene(tb_renderer *r, const tb_view_layout *layout, uint8_t *out);

#endif
