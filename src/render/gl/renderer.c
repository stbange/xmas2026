/* SPDX-License-Identifier: MIT */
#include "render/gl/renderer.h"

#include <stdio.h>
#include <string.h>

/* ---- shaders ------------------------------------------------------------ */

static const char *k_header_gl = "#version 330 core\n";
static const char *k_header_es = "#version 300 es\n"
                                 "precision highp float;\n"
                                 "precision highp sampler3D;\n"
                                 "precision highp sampler2DShadow;\n";

static const char *k_scene_vs = "in vec3 a_pos;\n"
                                "in vec3 a_normal;\n"
                                "in vec4 a_color;\n"
                                "uniform mat4 u_viewproj;\n"
                                "uniform mat4 u_shadow_mat;\n"
                                "out vec3 v_pos;\n"
                                "out vec3 v_normal;\n"
                                "out vec4 v_color;\n"
                                "out vec4 v_shadow;\n"
                                "void main() {\n"
                                "  v_pos = a_pos;\n"
                                "  v_normal = a_normal;\n"
                                "  v_color = a_color;\n"
                                "  v_shadow = u_shadow_mat * vec4(a_pos, 1.0);\n"
                                "  gl_Position = u_viewproj * vec4(a_pos, 1.0);\n"
                                "}\n";

static const char *k_scene_fs =
    "in vec3 v_pos;\n"
    "in vec3 v_normal;\n"
    "in vec4 v_color;\n"
    "in vec4 v_shadow;\n"
    "uniform vec3 u_ambient;\n"
    "uniform vec3 u_sun_dir;\n"
    "uniform vec3 u_sun_color;\n"
    "uniform vec3 u_fog_color;\n"
    "uniform vec2 u_fog;\n"
    "uniform vec3 u_eye;\n"
    "uniform int u_light_count;\n"
    "uniform vec3 u_light_pos[16];\n"
    "uniform vec3 u_light_color[16];\n"
    "uniform vec3 u_light_params[16];\n"
    "uniform int u_shadows;\n"
    "uniform sampler2DShadow u_shadow;\n"
    "out vec4 o_color;\n"
    "void main() {\n"
    "  vec3 n = normalize(v_normal);\n"
    "  float sh = 1.0;\n"
    "  if (u_shadows == 1) {\n"
    "    vec3 sc = v_shadow.xyz / v_shadow.w * 0.5 + 0.5;\n"
    "    if (sc.x > 0.0 && sc.x < 1.0 && sc.y > 0.0 && sc.y < 1.0 && sc.z < 1.0)\n"
    "      sh = texture(u_shadow, vec3(sc.xy, sc.z - 0.0015));\n"
    "  }\n"
    "  vec3 light = u_ambient + u_sun_color * max(dot(n, -u_sun_dir), 0.0) * sh;\n"
    "  for (int i = 0; i < 16; i++) {\n"
    "    if (i >= u_light_count) break;\n"
    "    vec3 d = u_light_pos[i] - v_pos;\n"
    "    float dist = max(length(d), 0.001);\n"
    "    float att = clamp(1.0 - dist / u_light_params[i].x, 0.0, 1.0);\n"
    "    light += u_light_color[i] * att * att * max(dot(n, d / dist), 0.0);\n"
    "  }\n"
    "  vec3 c = v_color.a < 0.75 ? v_color.rgb : v_color.rgb * light;\n"
    "  float f = clamp((length(v_pos - u_eye) - u_fog.x) / max(u_fog.y - u_fog.x, 0.001), 0.0, 1.0);\n"
    "  o_color = vec4(mix(c, u_fog_color, f), 1.0);\n"
    "}\n";

static const char *k_depth_vs = "in vec3 a_pos;\n"
                                "uniform mat4 u_mat;\n"
                                "void main() { gl_Position = u_mat * vec4(a_pos, 1.0); }\n";

static const char *k_depth_fs = "void main() {}\n";

static const char *k_post_vs = "out vec2 v_uv;\n"
                               "void main() {\n"
                               "  vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));\n"
                               "  v_uv = p;\n"
                               "  gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);\n"
                               "}\n";

static const char *k_post_fs =
    "in vec2 v_uv;\n"
    "uniform sampler2D u_src;\n"
    "uniform sampler3D u_lut;\n"
    "uniform int u_mode;\n" /* 0 copy, 1 quantise, 2 quantise + dither */
    "out vec4 o_color;\n"
    "const int bayer[16] = int[16](0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);\n"
    "void main() {\n"
    "  vec3 c = texture(u_src, v_uv).rgb;\n"
    "  if (u_mode >= 1) {\n"
    "    if (u_mode == 2) {\n"
    "      ivec2 p = ivec2(gl_FragCoord.xy) & 3;\n"
    "      float t = (float(bayer[p.y * 4 + p.x]) + 0.5) / 16.0 - 0.5;\n"
    "      c += t / 12.0;\n"
    "    }\n"
    "    c = texture(u_lut, clamp(c, 0.0, 1.0) * (31.0 / 32.0) + 0.5 / 32.0).rgb;\n"
    "  }\n"
    "  o_color = vec4(c, 1.0);\n"
    "}\n";

static GLuint compile(tb_renderer *r, GLenum type, const char *src)
{
    tb_gl *gl = &r->gl;
    const char *parts[2] = {r->gles ? k_header_es : k_header_gl, src};
    GLuint s = gl->CreateShader(type);
    gl->ShaderSource(s, 2, parts, NULL);
    gl->CompileShader(s);
    GLint ok = 0;
    gl->GetShaderiv(s, TB_GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[200] = "";
        gl->GetShaderInfoLog(s, sizeof(log), NULL, log);
        snprintf(r->error, sizeof(r->error), "shader compile: %s", log);
        gl->DeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint program(tb_renderer *r, const char *vs_src, const char *fs_src)
{
    tb_gl *gl = &r->gl;
    GLuint vs = compile(r, TB_GL_VERTEX_SHADER, vs_src);
    GLuint fs = vs ? compile(r, TB_GL_FRAGMENT_SHADER, fs_src) : 0;
    if (!fs) {
        if (vs) {
            gl->DeleteShader(vs);
        }
        return 0;
    }
    GLuint p = gl->CreateProgram();
    gl->AttachShader(p, vs);
    gl->AttachShader(p, fs);
    gl->BindAttribLocation(p, 0, "a_pos");
    gl->BindAttribLocation(p, 1, "a_normal");
    gl->BindAttribLocation(p, 2, "a_color");
    gl->LinkProgram(p);
    gl->DeleteShader(vs);
    gl->DeleteShader(fs);
    GLint ok = 0;
    gl->GetProgramiv(p, TB_GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[200] = "";
        gl->GetProgramInfoLog(p, sizeof(log), NULL, log);
        snprintf(r->error, sizeof(r->error), "program link: %s", log);
        gl->DeleteProgram(p);
        return 0;
    }
    return p;
}

/* ---- resources ----------------------------------------------------------- */

/* GL takes vertex buffer offsets as pointers. */
// NOLINTBEGIN(performance-no-int-to-ptr)
static void vertex_layout(tb_gl *gl)
{
    const GLsizei stride = (GLsizei)sizeof(tb_vertex);
    gl->EnableVertexAttribArray(0);
    gl->VertexAttribPointer(0, 3, TB_GL_FLOAT, TB_GL_FALSE, stride, (const void *)offsetof(tb_vertex, pos));
    gl->EnableVertexAttribArray(1);
    gl->VertexAttribPointer(1, 3, TB_GL_FLOAT, TB_GL_FALSE, stride,
                            (const void *)offsetof(tb_vertex, normal));
    gl->EnableVertexAttribArray(2);
    gl->VertexAttribPointer(2, 4, TB_GL_UNSIGNED_BYTE, TB_GL_TRUE, stride,
                            (const void *)offsetof(tb_vertex, color));
}
// NOLINTEND(performance-no-int-to-ptr)

static void make_buffer(tb_gl *gl, GLuint *vao, GLuint *vbo)
{
    gl->GenVertexArrays(1, vao);
    gl->BindVertexArray(*vao);
    gl->GenBuffers(1, vbo);
    gl->BindBuffer(TB_GL_ARRAY_BUFFER, *vbo);
    vertex_layout(gl);
    gl->BindVertexArray(0);
}

static GLuint make_texture(tb_gl *gl, GLint internal, GLsizei w, GLsizei h, GLenum format, GLenum type)
{
    GLuint t;
    gl->GenTextures(1, &t);
    gl->BindTexture(TB_GL_TEXTURE_2D, t);
    gl->TexImage2D(TB_GL_TEXTURE_2D, 0, internal, w, h, 0, format, type, NULL);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_MIN_FILTER, TB_GL_NEAREST);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_MAG_FILTER, TB_GL_NEAREST);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_WRAP_S, TB_GL_CLAMP_TO_EDGE);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_WRAP_T, TB_GL_CLAMP_TO_EDGE);
    return t;
}

static bool make_framebuffer(tb_gl *gl, GLuint *fbo, GLuint color, GLuint depth)
{
    gl->GenFramebuffers(1, fbo);
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, *fbo);
    if (color) {
        gl->FramebufferTexture2D(TB_GL_FRAMEBUFFER, TB_GL_COLOR_ATTACHMENT0, TB_GL_TEXTURE_2D, color, 0);
    } else {
        GLenum none = TB_GL_NONE;
        gl->DrawBuffers(1, &none);
        gl->ReadBuffer(TB_GL_NONE);
    }
    if (depth) {
        gl->FramebufferTexture2D(TB_GL_FRAMEBUFFER, TB_GL_DEPTH_ATTACHMENT, TB_GL_TEXTURE_2D, depth, 0);
    }
    bool ok = gl->CheckFramebufferStatus(TB_GL_FRAMEBUFFER) == TB_GL_FRAMEBUFFER_COMPLETE;
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, 0);
    return ok;
}

static void free_targets(tb_renderer *r)
{
    tb_gl *gl = &r->gl;
    GLuint fbos[2] = {r->scene_fbo, r->post_fbo};
    GLuint texs[3] = {r->scene_color, r->scene_depth, r->post_color};
    gl->DeleteFramebuffers(2, fbos);
    gl->DeleteTextures(3, texs);
    r->scene_fbo = r->post_fbo = r->scene_color = r->scene_depth = r->post_color = 0;
    r->scene_w = r->scene_h = 0;
}

static bool ensure_targets(tb_renderer *r, int w, int h)
{
    if (r->scene_w == w && r->scene_h == h) {
        return true;
    }
    tb_gl *gl = &r->gl;
    free_targets(r);
    r->scene_color = make_texture(gl, TB_GL_RGBA8, w, h, TB_GL_RGBA, TB_GL_UNSIGNED_BYTE);
    r->scene_depth =
        make_texture(gl, TB_GL_DEPTH_COMPONENT24, w, h, TB_GL_DEPTH_COMPONENT, TB_GL_UNSIGNED_INT);
    r->post_color = make_texture(gl, TB_GL_RGBA8, w, h, TB_GL_RGBA, TB_GL_UNSIGNED_BYTE);
    bool ok = make_framebuffer(gl, &r->scene_fbo, r->scene_color, r->scene_depth) &&
              make_framebuffer(gl, &r->post_fbo, r->post_color, 0);
    r->scene_w = w;
    r->scene_h = h;
    return ok;
}

bool tb_renderer_create(tb_renderer *r, tb_gl_loader loader, bool gles)
{
    memset(r, 0, sizeof(*r));
    r->gles = gles;
    r->options.palette_quantize = true;
    r->options.dithering = true;
    r->options.shadows = true;
    tb_mesh_init(&r->dynamic, NULL);
    const char *missing = NULL;
    if (!tb_gl_load(&r->gl, loader, &missing)) {
        snprintf(r->error, sizeof(r->error), "OpenGL function missing: %s", missing);
        return false;
    }
    tb_gl *gl = &r->gl;
    r->scene_prog = program(r, k_scene_vs, k_scene_fs);
    r->depth_prog = r->scene_prog ? program(r, k_depth_vs, k_depth_fs) : 0;
    r->post_prog = r->depth_prog ? program(r, k_post_vs, k_post_fs) : 0;
    if (!r->post_prog) {
        return false;
    }
    make_buffer(gl, &r->world_vao, &r->world_vbo);
    make_buffer(gl, &r->dyn_vao, &r->dyn_vbo);
    gl->GenVertexArrays(1, &r->post_vao);

    r->shadow_tex = make_texture(gl, TB_GL_DEPTH_COMPONENT24, TB_SHADOW_MAP_SIZE, TB_SHADOW_MAP_SIZE,
                                 TB_GL_DEPTH_COMPONENT, TB_GL_UNSIGNED_INT);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_COMPARE_MODE, TB_GL_COMPARE_REF_TO_TEXTURE);
    gl->TexParameteri(TB_GL_TEXTURE_2D, TB_GL_TEXTURE_COMPARE_FUNC, TB_GL_LEQUAL);
    if (!make_framebuffer(gl, &r->shadow_fbo, 0, r->shadow_tex)) {
        snprintf(r->error, sizeof(r->error), "shadow framebuffer incomplete");
        return false;
    }
    gl->GenTextures(1, &r->lut_tex);
    tb_palette pal;
    tb_palette_default(&pal);
    tb_renderer_set_palette(r, &pal);
    return true;
}

void tb_renderer_destroy(tb_renderer *r)
{
    tb_gl *gl = &r->gl;
    if (gl->DeleteProgram) {
        free_targets(r);
        GLuint vaos[3] = {r->world_vao, r->dyn_vao, r->post_vao};
        GLuint vbos[2] = {r->world_vbo, r->dyn_vbo};
        GLuint texs[2] = {r->shadow_tex, r->lut_tex};
        gl->DeleteVertexArrays(3, vaos);
        gl->DeleteBuffers(2, vbos);
        gl->DeleteTextures(2, texs);
        gl->DeleteFramebuffers(1, &r->shadow_fbo);
        gl->DeleteProgram(r->scene_prog);
        gl->DeleteProgram(r->depth_prog);
        gl->DeleteProgram(r->post_prog);
    }
    tb_mesh_free(&r->dynamic);
    memset(&r->gl, 0, sizeof(r->gl));
}

bool tb_renderer_upload_world(tb_renderer *r, const tb_mesh *mesh)
{
    tb_gl *gl = &r->gl;
    gl->BindBuffer(TB_GL_ARRAY_BUFFER, r->world_vbo);
    gl->BufferData(TB_GL_ARRAY_BUFFER, (GLsizeiptr)(mesh->vertices.len * sizeof(tb_vertex)),
                   mesh->vertices.data, TB_GL_STATIC_DRAW);
    r->world_vertices = (GLsizei)mesh->vertices.len;
    return gl->GetError() == 0;
}

void tb_renderer_set_palette(tb_renderer *r, const tb_palette *p)
{
    static tb_rgb lut[TB_LUT_SIZE * TB_LUT_SIZE * TB_LUT_SIZE];
    tb_palette_build_lut(p, lut);
    tb_gl *gl = &r->gl;
    gl->BindTexture(TB_GL_TEXTURE_3D, r->lut_tex);
    gl->PixelStorei(TB_GL_UNPACK_ALIGNMENT, 1);
    gl->TexImage3D(TB_GL_TEXTURE_3D, 0, TB_GL_RGB8, TB_LUT_SIZE, TB_LUT_SIZE, TB_LUT_SIZE, 0, TB_GL_RGB,
                   TB_GL_UNSIGNED_BYTE, lut);
    gl->TexParameteri(TB_GL_TEXTURE_3D, TB_GL_TEXTURE_MIN_FILTER, TB_GL_NEAREST);
    gl->TexParameteri(TB_GL_TEXTURE_3D, TB_GL_TEXTURE_MAG_FILTER, TB_GL_NEAREST);
    gl->TexParameteri(TB_GL_TEXTURE_3D, TB_GL_TEXTURE_WRAP_S, TB_GL_CLAMP_TO_EDGE);
    gl->TexParameteri(TB_GL_TEXTURE_3D, TB_GL_TEXTURE_WRAP_T, TB_GL_CLAMP_TO_EDGE);
    gl->TexParameteri(TB_GL_TEXTURE_3D, TB_GL_TEXTURE_WRAP_R, TB_GL_CLAMP_TO_EDGE);
}

/* ---- drawing ------------------------------------------------------------- */

static void build_dynamic(tb_renderer *r, const tb_scene *s)
{
    tb_dynarr_clear(&r->dynamic.vertices);
    for (int i = 0; i < s->box_count; i++) {
        const tb_box *b = &s->boxes[i];
        tb_brush brush;
        tb_geometry g;
        tb_geometry_init(&g, &brush, 1);
        tb_geometry_add_box(&g, b->mins, b->maxs, TB_CONTENTS_SOLID);
        uint8_t c[4] = {b->color[0], b->color[1], b->color[2], b->emissive ? 128 : 255};
        tb_mesh_add_brush(&r->dynamic, &brush, c);
    }
    tb_gl *gl = &r->gl;
    gl->BindBuffer(TB_GL_ARRAY_BUFFER, r->dyn_vbo);
    gl->BufferData(TB_GL_ARRAY_BUFFER, (GLsizeiptr)(r->dynamic.vertices.len * sizeof(tb_vertex)),
                   r->dynamic.vertices.data, TB_GL_DYNAMIC_DRAW);
}

static void draw_geometry(tb_renderer *r)
{
    tb_gl *gl = &r->gl;
    if (r->world_vertices > 0) {
        gl->BindVertexArray(r->world_vao);
        gl->DrawArrays(TB_GL_TRIANGLES, 0, r->world_vertices);
    }
    if (r->dynamic.vertices.len > 0) {
        gl->BindVertexArray(r->dyn_vao);
        gl->DrawArrays(TB_GL_TRIANGLES, 0, (GLsizei)r->dynamic.vertices.len);
    }
    gl->BindVertexArray(0);
}

static tb_mat4 shadow_matrix(const tb_scene *s)
{
    tb_vec3 centre = tb_v3(s->camera.eye.x, s->camera.eye.y, 0.0f);
    tb_vec3 from = tb_v3_sub(centre, tb_v3_scale(s->sun_dir, 50.0f));
    tb_mat4 view = tb_mat4_look_at(from, centre, tb_v3(0, 0, 1));
    return tb_mat4_mul(tb_mat4_ortho(-30, 30, -30, 30, 1.0f, 120.0f), view);
}

static void uniform_mat(tb_gl *gl, GLuint prog, const char *name, tb_mat4 m)
{
    gl->UniformMatrix4fv(gl->GetUniformLocation(prog, name), 1, TB_GL_FALSE, m.m);
}

static void uniform_v3(tb_gl *gl, GLuint prog, const char *name, tb_vec3 v)
{
    gl->Uniform3f(gl->GetUniformLocation(prog, name), v.x, v.y, v.z);
}

static void scene_pass(tb_renderer *r, const tb_scene *s, tb_mat4 shadow_mat, float aspect)
{
    tb_gl *gl = &r->gl;
    GLuint p = r->scene_prog;
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, r->scene_fbo);
    gl->Viewport(0, 0, r->scene_w, r->scene_h);
    gl->ClearColor(s->fog_color.x, s->fog_color.y, s->fog_color.z, 1.0f);
    gl->Clear(TB_GL_COLOR_BUFFER_BIT | TB_GL_DEPTH_BUFFER_BIT);
    gl->Enable(TB_GL_DEPTH_TEST);
    gl->DepthFunc(TB_GL_LEQUAL);
    gl->Enable(TB_GL_CULL_FACE);
    gl->CullFace(TB_GL_BACK);
    gl->UseProgram(p);
    tb_mat4 vp = tb_mat4_mul(tb_camera_projection(&s->camera, aspect), tb_camera_view(&s->camera));
    uniform_mat(gl, p, "u_viewproj", vp);
    uniform_mat(gl, p, "u_shadow_mat", shadow_mat);
    uniform_v3(gl, p, "u_ambient", s->ambient);
    uniform_v3(gl, p, "u_sun_dir", s->sun_dir);
    uniform_v3(gl, p, "u_sun_color", s->sun_color);
    uniform_v3(gl, p, "u_fog_color", s->fog_color);
    uniform_v3(gl, p, "u_eye", s->camera.eye);
    gl->Uniform2f(gl->GetUniformLocation(p, "u_fog"), s->fog_start, s->fog_end);
    float pos[TB_SCENE_MAX_LIGHTS * 3], col[TB_SCENE_MAX_LIGHTS * 3], par[TB_SCENE_MAX_LIGHTS * 3];
    memset(par, 0, sizeof(par));
    for (int i = 0; i < s->light_count; i++) {
        const tb_light *l = &s->lights[i];
        size_t k = (size_t)i * 3u;
        memcpy(&pos[k], &l->pos, sizeof(float) * 3);
        memcpy(&col[k], &l->color, sizeof(float) * 3);
        par[k] = l->radius;
    }
    gl->Uniform1i(gl->GetUniformLocation(p, "u_light_count"), s->light_count);
    if (s->light_count > 0) {
        gl->Uniform3fv(gl->GetUniformLocation(p, "u_light_pos"), s->light_count, pos);
        gl->Uniform3fv(gl->GetUniformLocation(p, "u_light_color"), s->light_count, col);
        gl->Uniform3fv(gl->GetUniformLocation(p, "u_light_params"), s->light_count, par);
    }
    gl->Uniform1i(gl->GetUniformLocation(p, "u_shadows"), r->options.shadows ? 1 : 0);
    gl->ActiveTexture(TB_GL_TEXTURE0);
    gl->BindTexture(TB_GL_TEXTURE_2D, r->shadow_tex);
    gl->Uniform1i(gl->GetUniformLocation(p, "u_shadow"), 0);
    draw_geometry(r);
    gl->Disable(TB_GL_CULL_FACE);
    gl->Disable(TB_GL_DEPTH_TEST);
}

static void shadow_pass(tb_renderer *r, tb_mat4 shadow_mat)
{
    tb_gl *gl = &r->gl;
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, r->shadow_fbo);
    gl->Viewport(0, 0, TB_SHADOW_MAP_SIZE, TB_SHADOW_MAP_SIZE);
    gl->Clear(TB_GL_DEPTH_BUFFER_BIT);
    gl->Enable(TB_GL_DEPTH_TEST);
    gl->DepthFunc(TB_GL_LESS);
    gl->Enable(TB_GL_POLYGON_OFFSET_FILL);
    gl->PolygonOffset(2.0f, 4.0f);
    gl->UseProgram(r->depth_prog);
    uniform_mat(gl, r->depth_prog, "u_mat", shadow_mat);
    draw_geometry(r);
    gl->Disable(TB_GL_POLYGON_OFFSET_FILL);
    gl->Disable(TB_GL_DEPTH_TEST);
}

static void post_pass(tb_renderer *r, GLuint src, int mode)
{
    tb_gl *gl = &r->gl;
    gl->UseProgram(r->post_prog);
    gl->ActiveTexture(TB_GL_TEXTURE0);
    gl->BindTexture(TB_GL_TEXTURE_2D, src);
    gl->Uniform1i(gl->GetUniformLocation(r->post_prog, "u_src"), 0);
    gl->ActiveTexture(TB_GL_TEXTURE0 + 1);
    gl->BindTexture(TB_GL_TEXTURE_3D, r->lut_tex);
    gl->Uniform1i(gl->GetUniformLocation(r->post_prog, "u_lut"), 1);
    gl->Uniform1i(gl->GetUniformLocation(r->post_prog, "u_mode"), mode);
    gl->BindVertexArray(r->post_vao);
    gl->DrawArrays(TB_GL_TRIANGLES, 0, 3);
    gl->BindVertexArray(0);
    gl->ActiveTexture(TB_GL_TEXTURE0);
}

void tb_renderer_draw(tb_renderer *r, const tb_scene *scene, const tb_view_layout *layout, int window_w,
                      int window_h)
{
    tb_gl *gl = &r->gl;
    if (!ensure_targets(r, layout->internal_w, layout->internal_h)) {
        snprintf(r->error, sizeof(r->error), "scene framebuffer incomplete");
        return;
    }
    build_dynamic(r, scene);
    tb_mat4 shadow_mat = shadow_matrix(scene);
    if (r->options.shadows) {
        shadow_pass(r, shadow_mat);
    }
    scene_pass(r, scene, shadow_mat, (float)layout->internal_w / (float)layout->internal_h);

    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, r->post_fbo);
    gl->Viewport(0, 0, r->scene_w, r->scene_h);
    int mode = r->options.palette_quantize ? (r->options.dithering ? 2 : 1) : 0;
    post_pass(r, r->scene_color, mode);

    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, 0);
    gl->Viewport(0, 0, window_w, window_h);
    gl->ClearColor(0, 0, 0, 1);
    gl->Clear(TB_GL_COLOR_BUFFER_BIT);
    gl->Viewport(layout->viewport_x, layout->viewport_y, layout->viewport_w, layout->viewport_h);
    post_pass(r, r->post_color, 0);
}

bool tb_renderer_read_scene(tb_renderer *r, const tb_view_layout *layout, uint8_t *out)
{
    tb_gl *gl = &r->gl;
    if (r->scene_w != layout->internal_w || r->scene_h != layout->internal_h) {
        return false;
    }
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, r->post_fbo);
    gl->PixelStorei(TB_GL_PACK_ALIGNMENT, 1);
    gl->ReadPixels(0, 0, r->scene_w, r->scene_h, TB_GL_RGBA, TB_GL_UNSIGNED_BYTE, out);
    gl->BindFramebuffer(TB_GL_FRAMEBUFFER, 0);
    return gl->GetError() == 0;
}
