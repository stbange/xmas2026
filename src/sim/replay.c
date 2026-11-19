/* SPDX-License-Identifier: MIT */
#include "sim/replay.h"

#include "core/hash.h"
#include "core/serial.h"

#include <string.h>

#define INPUT_BYTES  10u
#define HEADER_BYTES (4u + 2u + 8u + 1u + 1u + 4u + 4u)

void tb_replay_init(tb_replay *r, const tb_allocator *alloc)
{
    memset(r, 0, sizeof(*r));
    tb_dynarr_init(&r->ticks, alloc, sizeof(tb_replay_frame));
}

void tb_replay_free(tb_replay *r)
{
    tb_dynarr_free(&r->ticks);
}

bool tb_replay_push(tb_replay *r, const tb_input inputs[TB_MAX_PLAYERS])
{
    tb_replay_frame f;
    memcpy(f.inputs, inputs, sizeof(f.inputs));
    return tb_dynarr_push(&r->ticks, &f);
}

size_t tb_replay_length(const tb_replay *r)
{
    return r->ticks.len;
}

static int active_count(uint8_t mask)
{
    int n = 0;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        n += (mask >> i) & 1;
    }
    return n;
}

size_t tb_replay_encoded_size(const tb_replay *r)
{
    return HEADER_BYTES + r->ticks.len * (size_t)active_count(r->player_mask) * INPUT_BYTES + 8u + 4u;
}

static void write_input(tb_writer *w, const tb_input *in)
{
    tb_write_u8(w, (uint8_t)in->forward);
    tb_write_u8(w, (uint8_t)in->side);
    tb_write_u8(w, (uint8_t)in->up);
    tb_write_u8(w, in->weapon);
    tb_write_u16(w, in->buttons);
    tb_write_u16(w, (uint16_t)in->yaw);
    tb_write_u16(w, (uint16_t)in->pitch);
}

static void read_input(tb_reader *rd, tb_input *in)
{
    in->forward = (int8_t)tb_read_u8(rd);
    in->side = (int8_t)tb_read_u8(rd);
    in->up = (int8_t)tb_read_u8(rd);
    in->weapon = tb_read_u8(rd);
    in->buttons = tb_read_u16(rd);
    in->yaw = (int16_t)tb_read_u16(rd);
    in->pitch = (int16_t)tb_read_u16(rd);
}

size_t tb_replay_encode(const tb_replay *r, void *buf, size_t cap)
{
    tb_writer w;
    tb_writer_init(&w, buf, cap);
    tb_write_u32(&w, TB_REPLAY_MAGIC);
    tb_write_u16(&w, TB_REPLAY_VERSION);
    tb_write_u64(&w, r->seed);
    tb_write_u8(&w, r->difficulty);
    tb_write_u8(&w, r->player_mask);
    tb_write_u32(&w, r->level);
    tb_write_u32(&w, (uint32_t)r->ticks.len);
    for (size_t t = 0; t < r->ticks.len; t++) {
        const tb_replay_frame *f = tb_dynarr_at(&r->ticks, t);
        for (int p = 0; p < TB_MAX_PLAYERS; p++) {
            if ((r->player_mask >> p) & 1) {
                write_input(&w, &f->inputs[p]);
            }
        }
    }
    tb_write_u64(&w, r->final_checksum);
    if (!w.ok) {
        return 0;
    }
    tb_write_u32(&w, tb_crc32(buf, w.pos));
    return w.ok ? w.pos : 0;
}

bool tb_replay_decode(tb_replay *r, const void *buf, size_t len)
{
    const tb_allocator *alloc = r->ticks.alloc;
    tb_replay_free(r);
    tb_replay_init(r, alloc);
    if (!buf || len < 4) {
        return false;
    }
    tb_reader cr;
    tb_reader_init(&cr, (const uint8_t *)buf + len - 4, 4);
    if (tb_read_u32(&cr) != tb_crc32(buf, len - 4)) {
        return false;
    }
    tb_reader rd;
    tb_reader_init(&rd, buf, len - 4);
    if (tb_read_u32(&rd) != TB_REPLAY_MAGIC || tb_read_u16(&rd) != TB_REPLAY_VERSION) {
        return false;
    }
    r->seed = tb_read_u64(&rd);
    r->difficulty = tb_read_u8(&rd);
    r->player_mask = tb_read_u8(&rd);
    r->level = tb_read_u32(&rd);
    uint32_t count = tb_read_u32(&rd);
    int players = active_count(r->player_mask);
    if (!rd.ok || r->player_mask >= (1u << TB_MAX_PLAYERS) || players == 0 ||
        (size_t)count * (size_t)players * INPUT_BYTES + 8u != tb_reader_remaining(&rd)) {
        return false;
    }
    if (!tb_dynarr_reserve(&r->ticks, count)) {
        return false;
    }
    for (uint32_t t = 0; t < count; t++) {
        tb_replay_frame f;
        memset(&f, 0, sizeof(f));
        for (int p = 0; p < TB_MAX_PLAYERS; p++) {
            if ((r->player_mask >> p) & 1) {
                read_input(&rd, &f.inputs[p]);
            }
        }
        tb_dynarr_push(&r->ticks, &f);
    }
    r->final_checksum = tb_read_u64(&rd);
    return rd.ok;
}

uint64_t tb_replay_play(const tb_replay *r, tb_sim *sim)
{
    for (int p = 0; p < TB_MAX_PLAYERS; p++) {
        if ((r->player_mask >> p) & 1) {
            tb_sim_add_player(sim, p);
        }
    }
    for (size_t t = 0; t < r->ticks.len; t++) {
        const tb_replay_frame *f = tb_dynarr_at(&r->ticks, t);
        tb_sim_step(sim, f->inputs);
    }
    return tb_sim_checksum(sim);
}
