/* SPDX-License-Identifier: MIT */
#include "render/ui.h"

#include <string.h>

/* The Advent of LAN 5x7 font. Rows top to bottom, bit 4 = leftmost pixel. */
static const struct {
    char c;
    uint8_t rows[7];
} k_font[] = {
    {' ', {0, 0, 0, 0, 0, 0, 0}},        {'!', {4, 4, 4, 4, 4, 0, 4}},
    {'"', {10, 10, 0, 0, 0, 0, 0}},      {'#', {10, 31, 10, 10, 10, 31, 10}},
    {'%', {25, 26, 2, 4, 8, 11, 19}},    {'\'', {4, 4, 0, 0, 0, 0, 0}},
    {'(', {2, 4, 8, 8, 8, 4, 2}},        {')', {8, 4, 2, 2, 2, 4, 8}},
    {'*', {0, 4, 21, 14, 21, 4, 0}},     {'+', {0, 4, 4, 31, 4, 4, 0}},
    {',', {0, 0, 0, 0, 12, 4, 8}},       {'-', {0, 0, 0, 31, 0, 0, 0}},
    {'.', {0, 0, 0, 0, 0, 12, 12}},      {'/', {1, 1, 2, 4, 8, 16, 16}},
    {'0', {14, 17, 19, 21, 25, 17, 14}}, {'1', {4, 12, 4, 4, 4, 4, 14}},
    {'2', {14, 17, 1, 2, 4, 8, 31}},     {'3', {31, 2, 4, 2, 1, 17, 14}},
    {'4', {2, 6, 10, 18, 31, 2, 2}},     {'5', {31, 16, 30, 1, 1, 17, 14}},
    {'6', {6, 8, 16, 30, 17, 17, 14}},   {'7', {31, 1, 2, 4, 8, 8, 8}},
    {'8', {14, 17, 17, 14, 17, 17, 14}}, {'9', {14, 17, 17, 15, 1, 2, 12}},
    {':', {0, 12, 12, 0, 12, 12, 0}},    {';', {0, 12, 12, 0, 12, 4, 8}},
    {'<', {2, 4, 8, 16, 8, 4, 2}},       {'=', {0, 0, 31, 0, 31, 0, 0}},
    {'>', {8, 4, 2, 1, 2, 4, 8}},        {'?', {14, 17, 1, 2, 4, 0, 4}},
    {'@', {14, 17, 23, 21, 23, 16, 14}}, {'A', {14, 17, 17, 31, 17, 17, 17}},
    {'B', {30, 17, 17, 30, 17, 17, 30}}, {'C', {14, 17, 16, 16, 16, 17, 14}},
    {'D', {28, 18, 17, 17, 17, 18, 28}}, {'E', {31, 16, 16, 30, 16, 16, 31}},
    {'F', {31, 16, 16, 30, 16, 16, 16}}, {'G', {14, 17, 16, 23, 17, 17, 15}},
    {'H', {17, 17, 17, 31, 17, 17, 17}}, {'I', {14, 4, 4, 4, 4, 4, 14}},
    {'J', {7, 2, 2, 2, 2, 18, 12}},      {'K', {17, 18, 20, 24, 20, 18, 17}},
    {'L', {16, 16, 16, 16, 16, 16, 31}}, {'M', {17, 27, 21, 21, 17, 17, 17}},
    {'N', {17, 17, 25, 21, 19, 17, 17}}, {'O', {14, 17, 17, 17, 17, 17, 14}},
    {'P', {30, 17, 17, 30, 16, 16, 16}}, {'Q', {14, 17, 17, 17, 21, 18, 13}},
    {'R', {30, 17, 17, 30, 20, 18, 17}}, {'S', {15, 16, 16, 14, 1, 1, 30}},
    {'T', {31, 4, 4, 4, 4, 4, 4}},       {'U', {17, 17, 17, 17, 17, 17, 14}},
    {'V', {17, 17, 17, 17, 17, 10, 4}},  {'W', {17, 17, 17, 21, 21, 21, 10}},
    {'X', {17, 17, 10, 4, 10, 17, 17}},  {'Y', {17, 17, 17, 10, 4, 4, 4}},
    {'Z', {31, 1, 2, 4, 8, 16, 31}},     {'[', {14, 8, 8, 8, 8, 8, 14}},
    {']', {14, 2, 2, 2, 2, 2, 14}},      {'_', {0, 0, 0, 0, 0, 0, 31}},
};
static const uint8_t k_unknown[7] = {31, 17, 17, 17, 17, 17, 31};

const uint8_t *tb_font_glyph(char c)
{
    if (c >= 'a' && c <= 'z') {
        c = (char)(c - 'a' + 'A');
    }
    for (size_t i = 0; i < sizeof(k_font) / sizeof(k_font[0]); i++) {
        if (k_font[i].c == c) {
            return k_font[i].rows;
        }
    }
    return k_unknown;
}

void tb_ui_begin(tb_ui *ui, int width, int height)
{
    ui->count = 0;
    ui->width = width;
    ui->height = height;
    ui->overflow = false;
}

void tb_ui_rect_add(tb_ui *ui, int x, int y, int w, int h, const uint8_t color[4])
{
    if (w <= 0 || h <= 0) {
        return;
    }
    if (ui->count >= TB_UI_MAX_RECTS) {
        ui->overflow = true;
        return;
    }
    tb_ui_rect *r = &ui->rects[ui->count++];
    r->x = (int16_t)x;
    r->y = (int16_t)y;
    r->w = (int16_t)w;
    r->h = (int16_t)h;
    memcpy(r->color, color, 4);
}

int tb_ui_text_lines(const char *text)
{
    int lines = 1;
    for (const char *p = text; *p; p++) {
        lines += *p == '\n';
    }
    return lines;
}

static int line_len(const char *s)
{
    int n = 0;
    while (s[n] && s[n] != '\n') {
        n++;
    }
    return n;
}

int tb_ui_text_width(const char *text, int scale)
{
    int best = 0;
    for (const char *p = text;;) {
        int n = line_len(p);
        if (n > best) {
            best = n;
        }
        if (!p[n]) {
            break;
        }
        p += n + 1;
    }
    return best > 0 ? (best * TB_FONT_ADVANCE - 1) * scale : 0;
}

static void draw_glyph(tb_ui *ui, int x, int y, int scale, const uint8_t color[4], char c)
{
    const uint8_t *rows = tb_font_glyph(c);
    for (int row = 0; row < TB_FONT_H; row++) {
        /* Merge horizontal runs into one rectangle. */
        for (int col = 0; col < TB_FONT_W;) {
            if (!(rows[row] & (16 >> col))) {
                col++;
                continue;
            }
            int run = 1;
            while (col + run < TB_FONT_W && (rows[row] & (16 >> (col + run)))) {
                run++;
            }
            tb_ui_rect_add(ui, x + col * scale, y + row * scale, run * scale, scale, color);
            col += run;
        }
    }
}

void tb_ui_text(tb_ui *ui, int x, int y, int scale, tb_ui_align align, const uint8_t color[4],
                const char *text)
{
    static const uint8_t shadow[4] = {0, 0, 0, 200};
    for (const char *p = text;; y += TB_FONT_LINE * scale) {
        int n = line_len(p);
        int w = n > 0 ? (n * TB_FONT_ADVANCE - 1) * scale : 0;
        int left = align == TB_ALIGN_CENTER ? x - w / 2 : (align == TB_ALIGN_RIGHT ? x - w : x);
        for (int pass = 0; pass < 2; pass++) {
            for (int i = 0; i < n; i++) {
                if (p[i] != ' ') {
                    int gx = left + i * TB_FONT_ADVANCE * scale + (pass ? 0 : scale);
                    draw_glyph(ui, gx, y + (pass ? 0 : scale), scale, pass ? color : shadow, p[i]);
                }
            }
        }
        if (!p[n]) {
            break;
        }
        p += n + 1;
    }
}

void tb_ui_text_boxed(tb_ui *ui, int x, int y, int scale, tb_ui_align align, const uint8_t color[4],
                      const char *text)
{
    static const uint8_t back[4] = {6, 8, 20, 190};
    int w = tb_ui_text_width(text, scale);
    int h = (tb_ui_text_lines(text) * TB_FONT_LINE - 2) * scale;
    int left = align == TB_ALIGN_CENTER ? x - w / 2 : (align == TB_ALIGN_RIGHT ? x - w : x);
    tb_ui_rect_add(ui, left - 3 * scale, y - 2 * scale, w + 6 * scale, h + 4 * scale, back);
    tb_ui_text(ui, x, y, scale, align, color, text);
}

void tb_ui_wrap(const char *text, int max_chars, char *out, size_t out_len)
{
    size_t o = 0;
    int col = 0;
    const char *p = text;
    while (*p && o + 1 < out_len) {
        if (*p == '\n') {
            out[o++] = *p++;
            col = 0;
            continue;
        }
        if (*p == ' ') {
            /* Measure the next word; break the line if it does not fit. */
            int w = 0;
            while (p[1 + w] && p[1 + w] != ' ' && p[1 + w] != '\n') {
                w++;
            }
            if (col > 0 && col + 1 + w > max_chars) {
                out[o++] = '\n';
                col = 0;
                p++;
                continue;
            }
        }
        out[o++] = *p++;
        col++;
    }
    out[o] = '\0';
}
