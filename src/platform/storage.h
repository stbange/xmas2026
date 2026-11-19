/* SPDX-License-Identifier: MIT */
/* File-backed storage for saves and settings in one directory. */
#ifndef TB_PLATFORM_STORAGE_H
#define TB_PLATFORM_STORAGE_H

#include "game/save.h"
#include "game/settings.h"

typedef struct tb_storage {
    char dir[512]; /* ends with a path separator */
    tb_save_io io;
} tb_storage;

/* Uses `dir_override` if not empty, else the platform's pref directory.
 * The directory is created if needed. */
bool tb_storage_init(tb_storage *st, const char *dir_override);
/* Loads settings.json (defaults if missing or invalid). */
void tb_storage_load_settings(const tb_storage *st, tb_settings *out);
bool tb_storage_save_settings(const tb_storage *st, const tb_settings *s);

#endif
