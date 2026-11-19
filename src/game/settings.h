/* SPDX-License-Identifier: MIT */
/* Player settings, stored as human-readable JSON. Unknown keys are ignored
 * and out-of-range values are clamped, so old or hand-edited files load. */
#ifndef TB_GAME_SETTINGS_H
#define TB_GAME_SETTINGS_H

#include "core/types.h"

#define TB_SETTINGS_FILE "settings.json"

typedef enum tb_touch_mode { TB_TOUCH_AUTO = 0, TB_TOUCH_ON = 1, TB_TOUCH_OFF = 2 } tb_touch_mode;
typedef enum tb_gore { TB_GORE_OFF = 0, TB_GORE_CONFETTI = 1, TB_GORE_CLASSIC = 2 } tb_gore;

typedef struct tb_settings {
    float fov;               /* degrees, 60-110 */
    float mouse_sensitivity; /* 0.1-10 */
    float master_volume;     /* 0-1 */
    float music_volume;      /* 0-1 */
    int resolution_preset;   /* 0 crunchy 320x180, 1 classic 480x270, 2 crisp 640x360 */
    bool palette_quantize;
    bool dithering;
    bool crt_filter;
    bool head_bob;
    bool camera_shake;
    bool reduce_flashes;
    bool subtitles;
    tb_touch_mode touch_mode;
    tb_gore gore;
} tb_settings;

void tb_settings_defaults(tb_settings *s);
/* Parses JSON over defaults. Returns false for invalid JSON (s = defaults). */
bool tb_settings_from_json(tb_settings *s, const char *json, size_t len);
/* Returns a malloc'd JSON string (free with free()), or NULL on OOM. */
char *tb_settings_to_json(const tb_settings *s);

#endif
