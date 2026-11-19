/* SPDX-License-Identifier: MIT */
/* Files for saves and settings. Desktop: the user's pref path; web:
 * IndexedDB-backed storage (IDBFS) mounted at /persist and synced after
 * every write. */
#ifndef TB_PLATFORM_FS_H
#define TB_PLATFORM_FS_H

#include "core/types.h"

/* Directory for saves/settings, ending with a path separator. Caller frees
 * with SDL_free. org/app identify the game ("AdventOfLAN", "xmas2026"). */
char *tb_fs_pref_dir(const char *org, const char *app);
/* Reads a whole file. *data is NUL-terminated (not counted in *size);
 * free with SDL_free. Returns false if missing or unreadable. */
bool tb_fs_read(const char *path, void **data, size_t *size);
/* Writes atomically (temp file + rename), so a crash never leaves a
 * half-written save. */
bool tb_fs_write_atomic(const char *path, const void *data, size_t size);
bool tb_fs_exists(const char *path);
/* Returns true if the file no longer exists afterwards (also when it never
 * existed); false if it could not be removed. */
bool tb_fs_remove(const char *path);
/* Web: mounts IndexedDB storage and loads it (async). No-op elsewhere. */
void tb_fs_init_persistent(void);
/* True once persistent storage can be read (web: IndexedDB loaded). */
bool tb_fs_persistent_ready(void);
/* Web: flushes writes to IndexedDB. No-op elsewhere. */
void tb_fs_sync(void);

#endif
