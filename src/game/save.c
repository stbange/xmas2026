/* SPDX-License-Identifier: MIT */
#include "game/save.h"

#include "core/hash.h"
#include "core/serial.h"

#include <string.h>

/* Layout: magic u32 | version u16 | payload length u16 | payload | crc32(payload). */
#define HEADER_BYTES 8u

void tb_save_defaults(tb_save *s)
{
    memset(s, 0, sizeof(*s));
}

static void write_payload(tb_writer *w, const tb_save *s)
{
    tb_write_u32(w, s->unlocked);
    tb_write_u32(w, s->completed);
    tb_write_bytes(w, s->floppies, sizeof(s->floppies));
    tb_write_u32(w, s->challenges);
    tb_write_u32(w, s->trophies);
    tb_write_i32(w, s->alignment);
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        tb_write_u32(w, s->best_time_ms[i]);
    }
}

size_t tb_save_encode(const tb_save *s, void *buf, size_t cap)
{
    tb_writer w;
    tb_writer_init(&w, buf, cap);
    tb_write_u32(&w, TB_SAVE_MAGIC);
    tb_write_u16(&w, TB_SAVE_VERSION);
    tb_write_u16(&w, 0); /* payload length, patched below */
    size_t start = w.pos;
    write_payload(&w, s);
    if (!w.ok) {
        return 0;
    }
    size_t payload = w.pos - start;
    uint32_t crc = tb_crc32(w.buf + start, payload);
    tb_write_u32(&w, crc);
    if (!w.ok) {
        return 0;
    }
    w.buf[6] = (uint8_t)(payload & 0xFFu);
    w.buf[7] = (uint8_t)(payload >> 8);
    return w.pos;
}

static bool valid_values(const tb_save *s)
{
    const uint32_t all = TB_ALL_PORTALS;
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        if (s->floppies[i] & ~7u) {
            return false;
        }
    }
    return !(s->unlocked & ~all) && !(s->completed & ~all) && !(s->challenges & ~all) &&
           !(s->trophies & ~all);
}

tb_save_status tb_save_decode(const void *buf, size_t len, tb_save *out)
{
    tb_save_defaults(out);
    tb_reader r;
    tb_reader_init(&r, buf, len);
    uint32_t magic = tb_read_u32(&r);
    uint16_t version = tb_read_u16(&r);
    uint16_t payload = tb_read_u16(&r);
    if (!r.ok || magic != TB_SAVE_MAGIC) {
        return TB_SAVE_CORRUPT;
    }
    if (version > TB_SAVE_VERSION) {
        return TB_SAVE_TOO_NEW;
    }
    if (version == 0 || len != HEADER_BYTES + (size_t)payload + 4u) {
        return TB_SAVE_CORRUPT;
    }
    const uint8_t *bytes = buf;
    uint32_t crc = tb_crc32(bytes + HEADER_BYTES, payload);
    tb_reader cr;
    tb_reader_init(&cr, bytes + HEADER_BYTES + payload, 4);
    if (tb_read_u32(&cr) != crc) {
        return TB_SAVE_CORRUPT;
    }
    tb_save s;
    tb_save_defaults(&s);
    s.unlocked = tb_read_u32(&r);
    s.completed = tb_read_u32(&r);
    tb_read_bytes(&r, s.floppies, sizeof(s.floppies));
    s.challenges = tb_read_u32(&r);
    s.trophies = tb_read_u32(&r);
    s.alignment = tb_read_i32(&r);
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        s.best_time_ms[i] = tb_read_u32(&r);
    }
    if (!r.ok || r.pos != HEADER_BYTES + payload || !valid_values(&s)) {
        return TB_SAVE_CORRUPT;
    }
    *out = s;
    return TB_SAVE_OK;
}

tb_save_status tb_save_load(const tb_save_io *io, tb_save *out)
{
    void *data = NULL;
    size_t size = 0;
    if (!io->read(io->user, TB_SAVE_FILE, &data, &size)) {
        tb_save_defaults(out);
        return TB_SAVE_MISSING;
    }
    tb_save_status st = tb_save_decode(data, size, out);
    if (st != TB_SAVE_OK) {
        io->write(io->user, TB_SAVE_BACKUP, data, size);
    }
    io->free_data(io->user, data);
    return st;
}

bool tb_save_store(const tb_save_io *io, const tb_save *s)
{
    uint8_t buf[TB_SAVE_MAX_BYTES];
    size_t n = tb_save_encode(s, buf, sizeof(buf));
    return n > 0 && io->write(io->user, TB_SAVE_FILE, buf, n);
}

const char *tb_save_status_message(tb_save_status st)
{
    switch (st) {
    case TB_SAVE_OK:
        return "Progress loaded.";
    case TB_SAVE_MISSING:
        return "New game. Welcome to the Great Hall!";
    case TB_SAVE_CORRUPT:
        return "Your save file is damaged. A backup was kept; starting fresh.";
    case TB_SAVE_TOO_NEW:
        return "This save is from a newer version. A backup was kept; starting fresh.";
    default:
        return "?";
    }
}
