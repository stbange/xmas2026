/* SPDX-License-Identifier: MIT */
#include "render/palette.h"

void tb_palette_default(tb_palette *p)
{
    int n = 0;
    for (int b = 0; b < 6; b++) {
        for (int g = 0; g < 7; g++) {
            for (int r = 0; r < 6; r++) {
                p->colors[n].r = (uint8_t)(r * 255 / 5);
                p->colors[n].g = (uint8_t)(g * 255 / 6);
                p->colors[n].b = (uint8_t)(b * 255 / 5);
                n++;
            }
        }
    }
    /* 252 cube colours + 4 extra greys between the cube's grey steps. */
    const uint8_t greys[4] = {25, 76, 178, 229};
    for (int i = 0; i < 4; i++) {
        p->colors[n].r = p->colors[n].g = p->colors[n].b = greys[i];
        n++;
    }
    p->count = n;
}

int tb_palette_nearest(const tb_palette *p, tb_rgb c)
{
    int best = 0;
    long best_d = -1;
    for (int i = 0; i < p->count; i++) {
        long dr = (long)c.r - p->colors[i].r;
        long dg = (long)c.g - p->colors[i].g;
        long db = (long)c.b - p->colors[i].b;
        long d = 3 * dr * dr + 4 * dg * dg + 2 * db * db; /* eye is most sensitive to green */
        if (best_d < 0 || d < best_d) {
            best_d = d;
            best = i;
        }
    }
    return best;
}

void tb_palette_build_lut(const tb_palette *p, tb_rgb *lut)
{
    const int n = TB_LUT_SIZE;
    for (int b = 0; b < n; b++) {
        for (int g = 0; g < n; g++) {
            for (int r = 0; r < n; r++) {
                tb_rgb c = {(uint8_t)(r * 255 / (n - 1)), (uint8_t)(g * 255 / (n - 1)),
                            (uint8_t)(b * 255 / (n - 1))};
                lut[(b * n + g) * n + r] = p->count > 0 ? p->colors[tb_palette_nearest(p, c)] : c;
            }
        }
    }
}

float tb_bayer4(int x, int y)
{
    static const int m[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    return ((float)m[(y & 3) * 4 + (x & 3)] + 0.5f) / 16.0f - 0.5f;
}
