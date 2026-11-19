/* SPDX-License-Identifier: MIT */
/* The subset of OpenGL 3.3 core / OpenGL ES 3.0 / WebGL 2 the renderer
 * uses, loaded at runtime through a get-proc-address callback (SDL on all
 * platforms). Declared here so no platform GL header is needed. */
#ifndef TB_RENDER_GL_API_H
#define TB_RENDER_GL_API_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef float GLfloat;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

#define TB_GL_FUNCS(X)                                                                                       \
    X(void, Viewport, (GLint, GLint, GLsizei, GLsizei))                                                      \
    X(void, ClearColor, (GLfloat, GLfloat, GLfloat, GLfloat))                                                \
    X(void, Clear, (GLbitfield))                                                                             \
    X(void, Enable, (GLenum))                                                                                \
    X(void, Disable, (GLenum))                                                                               \
    X(void, CullFace, (GLenum))                                                                              \
    X(void, DepthFunc, (GLenum))                                                                             \
    X(void, BlendFunc, (GLenum, GLenum))                                                                     \
    X(void, PolygonOffset, (GLfloat, GLfloat))                                                               \
    X(GLenum, GetError, (void))                                                                              \
    X(void, ReadPixels, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *))                            \
    X(void, PixelStorei, (GLenum, GLint))                                                                    \
    X(GLuint, CreateShader, (GLenum))                                                                        \
    X(void, ShaderSource, (GLuint, GLsizei, const GLchar *const *, const GLint *))                           \
    X(void, CompileShader, (GLuint))                                                                         \
    X(void, GetShaderiv, (GLuint, GLenum, GLint *))                                                          \
    X(void, GetShaderInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *))                                        \
    X(void, DeleteShader, (GLuint))                                                                          \
    X(GLuint, CreateProgram, (void))                                                                         \
    X(void, AttachShader, (GLuint, GLuint))                                                                  \
    X(void, BindAttribLocation, (GLuint, GLuint, const GLchar *))                                            \
    X(void, LinkProgram, (GLuint))                                                                           \
    X(void, GetProgramiv, (GLuint, GLenum, GLint *))                                                         \
    X(void, GetProgramInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *))                                       \
    X(void, UseProgram, (GLuint))                                                                            \
    X(void, DeleteProgram, (GLuint))                                                                         \
    X(GLint, GetUniformLocation, (GLuint, const GLchar *))                                                   \
    X(void, Uniform1i, (GLint, GLint))                                                                       \
    X(void, Uniform1f, (GLint, GLfloat))                                                                     \
    X(void, Uniform2f, (GLint, GLfloat, GLfloat))                                                            \
    X(void, Uniform3f, (GLint, GLfloat, GLfloat, GLfloat))                                                   \
    X(void, Uniform3fv, (GLint, GLsizei, const GLfloat *))                                                   \
    X(void, UniformMatrix4fv, (GLint, GLsizei, GLboolean, const GLfloat *))                                  \
    X(void, GenVertexArrays, (GLsizei, GLuint *))                                                            \
    X(void, BindVertexArray, (GLuint))                                                                       \
    X(void, DeleteVertexArrays, (GLsizei, const GLuint *))                                                   \
    X(void, GenBuffers, (GLsizei, GLuint *))                                                                 \
    X(void, BindBuffer, (GLenum, GLuint))                                                                    \
    X(void, BufferData, (GLenum, GLsizeiptr, const void *, GLenum))                                          \
    X(void, DeleteBuffers, (GLsizei, const GLuint *))                                                        \
    X(void, EnableVertexAttribArray, (GLuint))                                                               \
    X(void, VertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))                  \
    X(void, DrawArrays, (GLenum, GLint, GLsizei))                                                            \
    X(void, GenTextures, (GLsizei, GLuint *))                                                                \
    X(void, BindTexture, (GLenum, GLuint))                                                                   \
    X(void, ActiveTexture, (GLenum))                                                                         \
    X(void, TexParameteri, (GLenum, GLenum, GLint))                                                          \
    X(void, TexImage2D, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))       \
    X(void, TexImage3D,                                                                                      \
      (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))                \
    X(void, DeleteTextures, (GLsizei, const GLuint *))                                                       \
    X(void, GenFramebuffers, (GLsizei, GLuint *))                                                            \
    X(void, BindFramebuffer, (GLenum, GLuint))                                                               \
    X(void, FramebufferTexture2D, (GLenum, GLenum, GLenum, GLuint, GLint))                                   \
    X(GLenum, CheckFramebufferStatus, (GLenum))                                                              \
    X(void, DeleteFramebuffers, (GLsizei, const GLuint *))                                                   \
    X(void, DrawBuffers, (GLsizei, const GLenum *))                                                          \
    X(void, ReadBuffer, (GLenum))

/* NOLINTNEXTLINE(bugprone-macro-parentheses): X-macro pastes a type and a parameter list */
#define TB_GL_DECLARE(ret, name, args) ret(*name) args;
typedef struct tb_gl {
    TB_GL_FUNCS(TB_GL_DECLARE)
} tb_gl;
#undef TB_GL_DECLARE

typedef void *(*tb_gl_loader)(const char *name);
/* Loads every function ("gl" + name). Returns false if any is missing and
 * names the first missing one in *missing. */
int tb_gl_load(tb_gl *gl, tb_gl_loader loader, const char **missing);

/* Constants (identical in GL 3.3 core and GLES 3.0). */
#define TB_GL_FALSE                  0
#define TB_GL_TRUE                   1
#define TB_GL_TRIANGLES              0x0004
#define TB_GL_DEPTH_BUFFER_BIT       0x00000100
#define TB_GL_COLOR_BUFFER_BIT       0x00004000
#define TB_GL_LEQUAL                 0x0203
#define TB_GL_LESS                   0x0201
#define TB_GL_BACK                   0x0405
#define TB_GL_FRONT                  0x0404
#define TB_GL_CULL_FACE              0x0B44
#define TB_GL_BLEND                  0x0BE2
#define TB_GL_SRC_ALPHA              0x0302
#define TB_GL_ONE_MINUS_SRC_ALPHA    0x0303
#define TB_GL_DEPTH_TEST             0x0B71
#define TB_GL_POLYGON_OFFSET_FILL    0x8037
#define TB_GL_UNSIGNED_BYTE          0x1401
#define TB_GL_FLOAT                  0x1406
#define TB_GL_UNSIGNED_INT           0x1405
#define TB_GL_RGB                    0x1907
#define TB_GL_RGBA                   0x1908
#define TB_GL_RGB8                   0x8051
#define TB_GL_RGBA8                  0x8058
#define TB_GL_DEPTH_COMPONENT        0x1902
#define TB_GL_DEPTH_COMPONENT24      0x81A6
#define TB_GL_TEXTURE_2D             0x0DE1
#define TB_GL_TEXTURE_3D             0x806F
#define TB_GL_TEXTURE0               0x84C0
#define TB_GL_TEXTURE_MIN_FILTER     0x2801
#define TB_GL_TEXTURE_MAG_FILTER     0x2800
#define TB_GL_TEXTURE_WRAP_S         0x2802
#define TB_GL_TEXTURE_WRAP_T         0x2803
#define TB_GL_TEXTURE_WRAP_R         0x8072
#define TB_GL_TEXTURE_COMPARE_MODE   0x884C
#define TB_GL_TEXTURE_COMPARE_FUNC   0x884D
#define TB_GL_COMPARE_REF_TO_TEXTURE 0x884E
#define TB_GL_NEAREST                0x2600
#define TB_GL_LINEAR                 0x2601
#define TB_GL_CLAMP_TO_EDGE          0x812F
#define TB_GL_ARRAY_BUFFER           0x8892
#define TB_GL_STATIC_DRAW            0x88E4
#define TB_GL_DYNAMIC_DRAW           0x88E8
#define TB_GL_FRAGMENT_SHADER        0x8B30
#define TB_GL_VERTEX_SHADER          0x8B31
#define TB_GL_COMPILE_STATUS         0x8B81
#define TB_GL_LINK_STATUS            0x8B82
#define TB_GL_FRAMEBUFFER            0x8D40
#define TB_GL_COLOR_ATTACHMENT0      0x8CE0
#define TB_GL_DEPTH_ATTACHMENT       0x8D00
#define TB_GL_FRAMEBUFFER_COMPLETE   0x8CD5
#define TB_GL_NONE                   0
#define TB_GL_PACK_ALIGNMENT         0x0D05
#define TB_GL_UNPACK_ALIGNMENT       0x0CF5

#endif
