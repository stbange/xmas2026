/* SPDX-License-Identifier: MIT */
/* 2D user interface as a list of coloured rectangles in UI pixels (origin
 * top-left; the UI resolution is 2x the scene's internal resolution). Text
 * uses the game's own 5x7 pixel font (CC BY 4.0, see assets/ATTRIBUTION.md). */
#ifndef TB_RENDER_UI_H
#define TB_RENDER_UI_H

#include "core/types.h"

#define TB_UI_MAX_RECTS 8192
#define TB_FONT_W       5
#define TB_FONT_H       7
#define TB_FONT_ADVANCE 6 /* glyph + 1 pixel spacing */
#define TB_FONT_LINE    9

typedef struct tb_ui_rect {
    int16_t x, y, w, h;
    uint8_t color[4];
} tb_ui_rect;

typedef struct tb_ui {
    tb_ui_rect rects[TB_UI_MAX_RECTS];
    int count;
    int width, height; /* UI resolution */
    bool overflow;     /* rectangles were dropped */
} tb_ui;

typedef enum tb_ui_align { TB_ALIGN_LEFT = 0, TB_ALIGN_CENTER = 1, TB_ALIGN_RIGHT = 2 } tb_ui_align;

void tb_ui_begin(tb_ui *ui, int width, int height);
void tb_ui_rect_add(tb_ui *ui, int x, int y, int w, int h, const uint8_t color[4]);
/* Width in UI pixels of the longest line of `text` at `scale`. */
int tb_ui_text_width(const char *text, int scale);
int tb_ui_text_lines(const char *text);
/* Draws text (multi-line with '\n'); `x` is the left/centre/right edge
 * depending on `align`. A one-pixel shadow keeps it readable. */
void tb_ui_text(tb_ui *ui, int x, int y, int scale, tb_ui_align align, const uint8_t color[4],
                const char *text);
/* Text on a dark backing panel (for lines over busy scenes). */
void tb_ui_text_boxed(tb_ui *ui, int x, int y, int scale, tb_ui_align align, const uint8_t color[4],
                      const char *text);
/* Wraps text to at most `max_chars` per line into out (words kept whole). */
void tb_ui_wrap(const char *text, int max_chars, char *out, size_t out_len);
/* The 7 row bitmasks (5 bits each, MSB = left) of a character; unknown
 * characters render as a box. */
const uint8_t *tb_font_glyph(char c);

#endif
