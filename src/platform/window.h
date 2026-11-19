/* SPDX-License-Identifier: MIT */
#ifndef TB_PLATFORM_WINDOW_H
#define TB_PLATFORM_WINDOW_H

#include "core/types.h"

typedef struct SDL_Window SDL_Window;

typedef struct tb_window {
    SDL_Window *window;
    void *gl; /* SDL_GLContext */
    void (*glClearColor)(float, float, float, float);
    void (*glClear)(unsigned int);
    void (*glViewport)(int, int, int, int);
} tb_window;

/* Creates a resizable window with an OpenGL 3.3 core (desktop) or OpenGL ES
 * 3.0 / WebGL 2 (web) context. SDL video must be initialised. On failure
 * everything created so far is released and false is returned. */
bool tb_window_create(tb_window *w, const char *title, int width, int height);
void tb_window_destroy(tb_window *w);
/* Clears to a colour and presents the frame. */
void tb_window_present_clear(tb_window *w, float r, float g, float b);

#endif
