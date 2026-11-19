/* SPDX-License-Identifier: MIT */
#include "core/serial.h"

#include <string.h>

void tb_writer_init(tb_writer *w, void *buf, size_t cap)
{
    w->buf = buf;
    w->cap = buf ? cap : 0;
    w->pos = 0;
    w->ok = true;
}

static uint8_t *w_reserve(tb_writer *w, size_t n)
{
    if (!w->ok || n > w->cap - w->pos) {
        w->ok = false;
        return NULL;
    }
    uint8_t *p = w->buf + w->pos;
    w->pos += n;
    return p;
}

static void w_le(tb_writer *w, uint64_t v, size_t n)
{
    uint8_t *p = w_reserve(w, n);
    if (p) {
        for (size_t i = 0; i < n; i++) {
            p[i] = (uint8_t)(v >> (8 * i));
        }
    }
}

void tb_write_u8(tb_writer *w, uint8_t v)
{
    w_le(w, v, 1);
}

void tb_write_u16(tb_writer *w, uint16_t v)
{
    w_le(w, v, 2);
}

void tb_write_u32(tb_writer *w, uint32_t v)
{
    w_le(w, v, 4);
}

void tb_write_u64(tb_writer *w, uint64_t v)
{
    w_le(w, v, 8);
}

void tb_write_i32(tb_writer *w, int32_t v)
{
    w_le(w, (uint32_t)v, 4);
}

void tb_write_f32(tb_writer *w, float v)
{
    uint32_t bits;
    memcpy(&bits, &v, sizeof(bits));
    w_le(w, bits, 4);
}

void tb_write_bool(tb_writer *w, bool v)
{
    w_le(w, v ? 1u : 0u, 1);
}

void tb_write_bytes(tb_writer *w, const void *data, size_t len)
{
    uint8_t *p = w_reserve(w, len);
    if (p && len) {
        memcpy(p, data, len);
    }
}

void tb_write_str(tb_writer *w, const char *s)
{
    size_t len = strlen(s);
    if (len > UINT16_MAX) {
        w->ok = false;
        return;
    }
    tb_write_u16(w, (uint16_t)len);
    tb_write_bytes(w, s, len);
}

void tb_reader_init(tb_reader *r, const void *buf, size_t len)
{
    r->buf = buf;
    r->len = buf ? len : 0;
    r->pos = 0;
    r->ok = true;
}

static const uint8_t *r_take(tb_reader *r, size_t n)
{
    if (!r->ok || n > r->len - r->pos) {
        r->ok = false;
        return NULL;
    }
    const uint8_t *p = r->buf + r->pos;
    r->pos += n;
    return p;
}

static uint64_t r_le(tb_reader *r, size_t n)
{
    const uint8_t *p = r_take(r, n);
    uint64_t v = 0;
    if (p) {
        for (size_t i = 0; i < n; i++) {
            v |= (uint64_t)p[i] << (8 * i);
        }
    }
    return v;
}

uint8_t tb_read_u8(tb_reader *r)
{
    return (uint8_t)r_le(r, 1);
}

uint16_t tb_read_u16(tb_reader *r)
{
    return (uint16_t)r_le(r, 2);
}

uint32_t tb_read_u32(tb_reader *r)
{
    return (uint32_t)r_le(r, 4);
}

uint64_t tb_read_u64(tb_reader *r)
{
    return r_le(r, 8);
}

int32_t tb_read_i32(tb_reader *r)
{
    return (int32_t)(uint32_t)r_le(r, 4);
}

float tb_read_f32(tb_reader *r)
{
    uint32_t bits = (uint32_t)r_le(r, 4);
    float v;
    memcpy(&v, &bits, sizeof(v));
    return v;
}

bool tb_read_bool(tb_reader *r)
{
    uint8_t v = tb_read_u8(r);
    if (v > 1) {
        r->ok = false;
        return false;
    }
    return v == 1;
}

void tb_read_bytes(tb_reader *r, void *out, size_t len)
{
    const uint8_t *p = r_take(r, len);
    if (p && len) {
        memcpy(out, p, len);
    }
}

void tb_read_str(tb_reader *r, char *out, size_t out_cap)
{
    if (out_cap > 0) {
        out[0] = '\0';
    }
    size_t len = tb_read_u16(r);
    if (!r->ok || len >= out_cap) {
        r->ok = false;
        return;
    }
    tb_read_bytes(r, out, len);
    out[r->ok ? len : 0] = '\0';
}

size_t tb_reader_remaining(const tb_reader *r)
{
    return r->ok ? r->len - r->pos : 0;
}
