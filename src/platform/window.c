/* SPDX-License-Identifier: MIT */
#include "platform/window.h"

#include "core/log.h"

#include <SDL3/SDL.h>
#include <string.h>

#define TB_GL_COLOR_BUFFER_BIT 0x00004000u
#define TB_GL_DEPTH_BUFFER_BIT 0x00000100u

static void set_gl_attributes(void)
{
#ifdef SDL_PLATFORM_EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

bool tb_window_create(tb_window *w, const char *title, int width, int height)
{
    memset(w, 0, sizeof(*w));
    set_gl_attributes();
    w->window = SDL_CreateWindow(title, width, height,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!w->window) {
        tb_log_error("window: %s", SDL_GetError());
        return false;
    }
    w->gl = SDL_GL_CreateContext(w->window);
    if (!w->gl) {
        tb_log_error("OpenGL context: %s", SDL_GetError());
        tb_window_destroy(w);
        return false;
    }
    SDL_GL_SetSwapInterval(1);
    w->glClearColor = (void (*)(float, float, float, float))SDL_GL_GetProcAddress("glClearColor");
    w->glClear = (void (*)(unsigned int))SDL_GL_GetProcAddress("glClear");
    w->glViewport = (void (*)(int, int, int, int))SDL_GL_GetProcAddress("glViewport");
    if (!w->glClearColor || !w->glClear || !w->glViewport) {
        tb_log_error("OpenGL entry points missing");
        tb_window_destroy(w);
        return false;
    }
    return true;
}

void tb_window_destroy(tb_window *w)
{
    if (w->gl) {
        SDL_GL_DestroyContext(w->gl);
    }
    if (w->window) {
        SDL_DestroyWindow(w->window);
    }
    memset(w, 0, sizeof(*w));
}

void tb_window_present_clear(tb_window *w, float r, float g, float b)
{
    int pw = 0, ph = 0;
    SDL_GetWindowSizeInPixels(w->window, &pw, &ph);
    w->glViewport(0, 0, pw, ph);
    w->glClearColor(r, g, b, 1.0f);
    w->glClear(TB_GL_COLOR_BUFFER_BIT | TB_GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(w->window);
}
