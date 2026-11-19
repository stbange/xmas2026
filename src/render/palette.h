/* SPDX-License-Identifier: MIT */
/* Level palettes: the final image is quantised to up to 256 colours through
 * a 3D lookup table, optionally with ordered (Bayer) dithering. */
#ifndef TB_RENDER_PALETTE_H
#define TB_RENDER_PALETTE_H

#include "core/types.h"

#define TB_PALETTE_MAX 256
#define TB_LUT_SIZE    32 /* LUT resolution per channel */

typedef struct tb_rgb {
    uint8_t r, g, b;
} tb_rgb;

typedef struct tb_palette {
    tb_rgb colors[TB_PALETTE_MAX];
    int count;
} tb_palette;

/* A neutral 256-colour default (6x7x6 colour cube + greys), used until a
 * level provides its own. */
void tb_palette_default(tb_palette *p);
/* Index of the nearest palette colour (perceptually weighted distance). */
int tb_palette_nearest(const tb_palette *p, tb_rgb c);
/* Fills an RGB lookup table of TB_LUT_SIZE^3 entries (r fastest, then g,
 * then b) mapping each colour to its nearest palette colour. */
void tb_palette_build_lut(const tb_palette *p, tb_rgb *lut);
/* 4x4 Bayer threshold in [-0.5, 0.5) for pixel (x, y). */
float tb_bayer4(int x, int y);

#endif
