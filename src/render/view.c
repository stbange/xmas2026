/* SPDX-License-Identifier: MIT */
#include "render/view.h"

int tb_res_preset_lines(tb_res_preset p)
{
    switch (p) {
    case TB_RES_CRUNCHY:
        return 180;
    case TB_RES_CRISP:
        return 360;
    default:
        return 270;
    }
}

tb_view_layout tb_view_layout_for(int window_w, int window_h, tb_res_preset preset)
{
    tb_view_layout l = {0, 0, 1, 0, 0, 0, 0, 0, 0};
    if (window_w < 1) {
        window_w = 1;
    }
    if (window_h < 1) {
        window_h = 1;
    }
    int lines = tb_res_preset_lines(preset);
    if (lines > window_h) {
        lines = window_h;
    }
    int w = (int)((long long)window_w * lines / window_h);
    w = (w + 1) & ~1; /* even width */
    if (w > window_w) {
        w = window_w;
    }
    if (w < 1) {
        w = 1;
    }
    int scale = window_h / lines;
    if (window_w / w < scale) {
        scale = window_w / w;
    }
    if (scale < 1) {
        scale = 1;
    }
    l.internal_w = w;
    l.internal_h = lines;
    l.scale = scale;
    l.viewport_w = w * scale;
    l.viewport_h = lines * scale;
    l.viewport_x = (window_w - l.viewport_w) / 2;
    l.viewport_y = (window_h - l.viewport_h) / 2;
    l.ui_w = w * 2;
    l.ui_h = lines * 2;
    return l;
}

void tb_view_split(int count, int index, int w, int h, int *x, int *y, int *sw, int *sh)
{
    *x = 0;
    *y = 0;
    *sw = w;
    *sh = h;
    if (count == 2) {
        *sw = w / 2;
        *x = index == 1 ? w - *sw : 0;
    } else if (count == 3) {
        /* Player 1 gets the top half, players 2 and 3 share the bottom. */
        *sh = h / 2;
        if (index == 0) {
            *y = h - *sh;
        } else {
            *sw = w / 2;
            *x = index == 2 ? w - *sw : 0;
        }
    } else if (count >= 4) {
        *sw = w / 2;
        *sh = h / 2;
        *x = (index % 2) ? w - *sw : 0;
        *y = (index < 2) ? h - *sh : 0;
    }
}
