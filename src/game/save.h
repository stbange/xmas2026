/* SPDX-License-Identifier: MIT */
/* Player progress, stored as a versioned, CRC-checked binary file. Loading
 * never fails hard: a missing file gives defaults, a corrupt one is backed
 * up and replaced by defaults. */
#ifndef TB_GAME_SAVE_H
#define TB_GAME_SAVE_H

#include "game/calendar.h"

#define TB_SAVE_MAGIC     0x56534254u /* "TBSV" little-endian */
#define TB_SAVE_VERSION   1u
#define TB_SAVE_MAX_BYTES 1024
#define TB_SAVE_FILE      "progress.sav"
#define TB_SAVE_BACKUP    "progress.sav.corrupt"

typedef struct tb_save {
    tb_portal_mask unlocked;                /* portals ever opened on this device */
    uint32_t completed;                     /* bit n-1: level n finished (bauble on the tree) */
    uint8_t floppies[TB_PORTAL_COUNT];      /* bits 0-2: the three floppies per level */
    uint32_t challenges;                    /* Golden Joysticks */
    uint32_t trophies;                      /* trophies on the wall */
    int32_t alignment;                      /* Nice (>0) / Naughty (<0) */
    uint32_t best_time_ms[TB_PORTAL_COUNT]; /* 0 = no time yet */
} tb_save;

typedef enum tb_save_status {
    TB_SAVE_OK = 0,
    TB_SAVE_MISSING, /* no file: fresh defaults */
    TB_SAVE_CORRUPT, /* bad magic, checksum, size or values: defaults, backup kept */
    TB_SAVE_TOO_NEW  /* written by a newer game: defaults, backup kept */
} tb_save_status;

void tb_save_defaults(tb_save *s);
/* Returns bytes written, 0 if buf is too small. */
size_t tb_save_encode(const tb_save *s, void *buf, size_t cap);
tb_save_status tb_save_decode(const void *buf, size_t len, tb_save *out);

/* Storage interface (the platform implements it with files). */
typedef struct tb_save_io {
    bool (*read)(void *user, const char *name, void **data, size_t *size);
    void (*free_data)(void *user, void *data);
    bool (*write)(void *user, const char *name, const void *data, size_t size);
    void *user;
} tb_save_io;

/* Loads progress. On TB_SAVE_CORRUPT/TOO_NEW the original bytes are copied
 * to TB_SAVE_BACKUP before defaults are returned. */
tb_save_status tb_save_load(const tb_save_io *io, tb_save *out);
bool tb_save_store(const tb_save_io *io, const tb_save *s);
const char *tb_save_status_message(tb_save_status st);

#endif
