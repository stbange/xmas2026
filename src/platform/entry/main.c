/* SPDX-License-Identifier: MIT */
/* SDL entry point. Only forwards SDL's callbacks to the tested host
 * (platform/host.c); keep this file minimal, it is excluded from coverage. */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "platform/host.h"

static SDL_AppResult to_sdl(tb_host_result r)
{
    return r == TB_HOST_CONTINUE ? SDL_APP_CONTINUE
                                 : (r == TB_HOST_SUCCESS ? SDL_APP_SUCCESS : SDL_APP_FAILURE);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    tb_host *host = SDL_calloc(1, sizeof(*host));
    *appstate = host;
    return host ? to_sdl(tb_host_init(host, argc, (const char *const *)argv)) : SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return to_sdl(tb_host_event(appstate, event));
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    return to_sdl(tb_host_iterate(appstate));
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)result;
    if (appstate) {
        tb_host_quit(appstate);
        SDL_free(appstate);
    }
}
