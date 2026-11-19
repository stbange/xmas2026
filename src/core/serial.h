/* SPDX-License-Identifier: MIT */
/* Bounds-checked little-endian binary serialisation for saves, network
 * packets, replays and content packages. Errors are sticky: after the first
 * failure every further call is a no-op and `ok` stays false. */
#ifndef TB_CORE_SERIAL_H
#define TB_CORE_SERIAL_H

#include "core/types.h"

typedef struct tb_writer {
    uint8_t *buf;
    size_t cap;
    size_t pos;
    bool ok;
} tb_writer;

typedef struct tb_reader {
    const uint8_t *buf;
    size_t len;
    size_t pos;
    bool ok;
} tb_reader;

void tb_writer_init(tb_writer *w, void *buf, size_t cap);
void tb_write_u8(tb_writer *w, uint8_t v);
void tb_write_u16(tb_writer *w, uint16_t v);
void tb_write_u32(tb_writer *w, uint32_t v);
void tb_write_u64(tb_writer *w, uint64_t v);
void tb_write_i32(tb_writer *w, int32_t v);
void tb_write_f32(tb_writer *w, float v);
void tb_write_bool(tb_writer *w, bool v);
void tb_write_bytes(tb_writer *w, const void *data, size_t len);
/* u16 length prefix + bytes (no terminator). Fails for strings > 65535 bytes. */
void tb_write_str(tb_writer *w, const char *s);

void tb_reader_init(tb_reader *r, const void *buf, size_t len);
uint8_t tb_read_u8(tb_reader *r);
uint16_t tb_read_u16(tb_reader *r);
uint32_t tb_read_u32(tb_reader *r);
uint64_t tb_read_u64(tb_reader *r);
int32_t tb_read_i32(tb_reader *r);
float tb_read_f32(tb_reader *r);
/* Fails on values other than 0 and 1 (malformed input). */
bool tb_read_bool(tb_reader *r);
void tb_read_bytes(tb_reader *r, void *out, size_t len);
/* Reads a string written by tb_write_str into out (always NUL-terminated
 * when out_cap > 0). Fails if it does not fit. */
void tb_read_str(tb_reader *r, char *out, size_t out_cap);
size_t tb_reader_remaining(const tb_reader *r);

#endif
