/* SPDX-License-Identifier: MIT */
#include "platform/storage.h"

#include "core/log.h"
#include "platform/fs.h"

#include <SDL3/SDL.h>
#include <stdlib.h>

static void full_path(const tb_storage *st, const char *name, char *out, size_t out_len)
{
    SDL_snprintf(out, out_len, "%s%s", st->dir, name);
}

static bool io_read(void *user, const char *name, void **data, size_t *size)
{
    char path[640];
    full_path(user, name, path, sizeof(path));
    return tb_fs_read(path, data, size);
}

static void io_free(void *user, void *data)
{
    (void)user;
    SDL_free(data);
}

static bool io_write(void *user, const char *name, const void *data, size_t size)
{
    char path[640];
    full_path(user, name, path, sizeof(path));
    return tb_fs_write_atomic(path, data, size);
}

bool tb_storage_init(tb_storage *st, const char *dir_override)
{
    SDL_zerop(st);
    if (dir_override && *dir_override) {
        size_t n = SDL_strlen(dir_override);
        bool has_sep = dir_override[n - 1] == '/' || dir_override[n - 1] == '\\';
        SDL_snprintf(st->dir, sizeof(st->dir), "%s%s", dir_override, has_sep ? "" : "/");
        if (!SDL_CreateDirectory(st->dir)) {
            tb_log_error("data dir %s: %s", st->dir, SDL_GetError());
            return false;
        }
    } else {
        char *pref = tb_fs_pref_dir("AdventOfLAN", "xmas2026");
        if (!pref) {
            tb_log_error("no data directory: %s", SDL_GetError());
            return false;
        }
        SDL_strlcpy(st->dir, pref, sizeof(st->dir));
        SDL_free(pref);
    }
    st->io.read = io_read;
    st->io.free_data = io_free;
    st->io.write = io_write;
    st->io.user = st;
    return true;
}

void tb_storage_load_settings(const tb_storage *st, tb_settings *out)
{
    void *data = NULL;
    size_t size = 0;
    tb_settings_defaults(out);
    if (io_read((void *)st, TB_SETTINGS_FILE, &data, &size)) {
        if (!tb_settings_from_json(out, data, size)) {
            tb_log_warn("settings.json is invalid; using defaults");
        }
        SDL_free(data);
    }
}

bool tb_storage_save_settings(const tb_storage *st, const tb_settings *s)
{
    char *json = tb_settings_to_json(s);
    if (!json) {
        return false;
    }
    bool ok = io_write((void *)st, TB_SETTINGS_FILE, json, SDL_strlen(json));
    free(json);
    return ok;
}
