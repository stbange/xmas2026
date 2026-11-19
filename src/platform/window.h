/* SPDX-License-Identifier: MIT */
#ifndef TB_PLATFORM_WINDOW_H
#define TB_PLATFORM_WINDOW_H

#include "core/types.h"

typedef struct SDL_Window SDL_Window;

typedef struct tb_window {
    SDL_Window *window;
    void *gl; /* SDL_GLContext */
} tb_window;

/* Creates a resizable window with an OpenGL 3.3 core (desktop) or OpenGL ES
 * 3.0 / WebGL 2 (web) context. SDL video must be initialised. On failure
 * everything created so far is released and false is returned. */
bool tb_window_create(tb_window *w, const char *title, int width, int height);
void tb_window_destroy(tb_window *w);
/* Size of the drawable in pixels. */
void tb_window_pixel_size(const tb_window *w, int *width, int *height);
/* Presents the frame. */
void tb_window_swap(tb_window *w);
/* Loader for tb_gl_load (SDL_GL_GetProcAddress). */
void *tb_window_gl_proc(const char *name);

#endif
