/* SPDX-License-Identifier: MIT */
#include "platform/fs.h"

#include "core/log.h"

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#define TB_WEB_PERSIST_DIR "/persist/"
#endif

char *tb_fs_pref_dir(const char *org, const char *app)
{
#ifdef SDL_PLATFORM_EMSCRIPTEN
    (void)org;
    (void)app;
    return SDL_strdup(TB_WEB_PERSIST_DIR);
#else
    return SDL_GetPrefPath(org, app);
#endif
}

bool tb_fs_read(const char *path, void **data, size_t *size)
{
    *data = SDL_LoadFile(path, size);
    return *data != NULL;
}

bool tb_fs_write_atomic(const char *path, const void *data, size_t size)
{
    char *tmp = NULL;
    if (SDL_asprintf(&tmp, "%s.tmp", path) < 0) {
        return false;
    }
    bool ok = SDL_SaveFile(tmp, data, size) && SDL_RenamePath(tmp, path);
    if (!ok) {
        tb_log_error("write %s: %s", path, SDL_GetError());
        SDL_RemovePath(tmp);
    }
    SDL_free(tmp);
    if (ok) {
        tb_fs_sync();
    }
    return ok;
}

bool tb_fs_exists(const char *path)
{
    return SDL_GetPathInfo(path, NULL);
}

bool tb_fs_remove(const char *path)
{
    bool ok = SDL_RemovePath(path);
    if (ok) {
        tb_fs_sync();
    }
    return ok;
}

void tb_fs_init_persistent(void)
{
#ifdef SDL_PLATFORM_EMSCRIPTEN
    emscripten_run_script("try { FS.mkdir('/persist'); } catch (e) {}"
                          "FS.mount(IDBFS, {}, '/persist');"
                          "FS.syncfs(true, function (err) { if (err) console.warn('IDBFS load', err); "
                          "Module.tbPersistReady = true; });");
#endif
}

void tb_fs_sync(void)
{
#ifdef SDL_PLATFORM_EMSCRIPTEN
    emscripten_run_script("FS.syncfs(false, function (err) { if (err) console.warn('IDBFS sync', err); });");
#endif
}
