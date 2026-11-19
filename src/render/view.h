/* SPDX-License-Identifier: MIT */
/* Low-resolution render target sizing and integer upscaling. */
#ifndef TB_RENDER_VIEW_H
#define TB_RENDER_VIEW_H

#include "core/types.h"

typedef enum tb_res_preset {
    TB_RES_CRUNCHY = 0, /* 180 lines (320x180 at 16:9) */
    TB_RES_CLASSIC = 1, /* 270 lines (480x270 at 16:9), default */
    TB_RES_CRISP = 2,   /* 360 lines (640x360 at 16:9) */
    TB_RES_PRESET_COUNT
} tb_res_preset;

typedef struct tb_view_layout {
    int internal_w, internal_h; /* scene render target */
    int scale;                  /* integer upscale factor (>= 1) */
    int viewport_x, viewport_y; /* letterboxed placement in the window */
    int viewport_w, viewport_h;
    int ui_w, ui_h; /* UI render resolution (2x internal) */
} tb_view_layout;

int tb_res_preset_lines(tb_res_preset p);
/* Computes the internal resolution for a window: the preset's line count
 * keeps its height and the width follows the window aspect (rounded to
 * even). The scene is upscaled by the largest integer factor that fits;
 * if even 1x does not fit, the internal size shrinks to the window. */
tb_view_layout tb_view_layout_for(int window_w, int window_h, tb_res_preset preset);
/* Split-screen sub-rectangle n of count (1-4) inside a window area. */
void tb_view_split(int count, int index, int w, int h, int *x, int *y, int *sw, int *sh);

#endif
