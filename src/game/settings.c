/* SPDX-License-Identifier: MIT */
#include "game/settings.h"

#include "core/mathx.h"

#include <cJSON.h>
#include <string.h>

void tb_settings_defaults(tb_settings *s)
{
    memset(s, 0, sizeof(*s));
    s->fov = 90.0f;
    s->mouse_sensitivity = 1.0f;
    s->master_volume = 0.8f;
    s->music_volume = 0.7f;
    s->resolution_preset = 1;
    s->palette_quantize = true;
    s->dithering = true;
    s->crt_filter = false;
    s->head_bob = true;
    s->camera_shake = true;
    s->reduce_flashes = false;
    s->subtitles = true;
    s->touch_mode = TB_TOUCH_AUTO;
    s->gore = TB_GORE_CONFETTI;
}

static void get_float(const cJSON *root, const char *key, float *v, float lo, float hi)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
    if (cJSON_IsNumber(item)) {
        *v = tb_clampf((float)item->valuedouble, lo, hi);
    }
}

static void get_int(const cJSON *root, const char *key, int *v, int lo, int hi)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
    if (cJSON_IsNumber(item)) {
        double d = item->valuedouble;
        *v = d < lo ? lo : (d > hi ? hi : (int)d);
    }
}

static void get_bool(const cJSON *root, const char *key, bool *v)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
    if (cJSON_IsBool(item)) {
        *v = cJSON_IsTrue(item);
    }
}

static int find_name(const char *const *names, int count, const char *value)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], value) == 0) {
            return i;
        }
    }
    return -1;
}

static const char *const k_touch[] = {"auto", "on", "off"};
static const char *const k_gore[] = {"off", "confetti", "classic"};

static void get_enum(const cJSON *root, const char *key, const char *const *names, int count, int *v)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
    if (cJSON_IsString(item)) {
        int i = find_name(names, count, item->valuestring);
        if (i >= 0) {
            *v = i;
        }
    }
}

bool tb_settings_from_json(tb_settings *s, const char *json, size_t len)
{
    tb_settings_defaults(s);
    cJSON *root = cJSON_ParseWithLength(json, len);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        return false;
    }
    get_float(root, "fov", &s->fov, 60.0f, 110.0f);
    get_float(root, "mouseSensitivity", &s->mouse_sensitivity, 0.1f, 10.0f);
    get_float(root, "masterVolume", &s->master_volume, 0.0f, 1.0f);
    get_float(root, "musicVolume", &s->music_volume, 0.0f, 1.0f);
    get_int(root, "resolutionPreset", &s->resolution_preset, 0, 2);
    get_bool(root, "paletteQuantize", &s->palette_quantize);
    get_bool(root, "dithering", &s->dithering);
    get_bool(root, "crtFilter", &s->crt_filter);
    get_bool(root, "headBob", &s->head_bob);
    get_bool(root, "cameraShake", &s->camera_shake);
    get_bool(root, "reduceFlashes", &s->reduce_flashes);
    get_bool(root, "subtitles", &s->subtitles);
    int touch = (int)s->touch_mode, gore = (int)s->gore;
    get_enum(root, "touchMode", k_touch, 3, &touch);
    get_enum(root, "gore", k_gore, 3, &gore);
    s->touch_mode = (tb_touch_mode)touch;
    s->gore = (tb_gore)gore;
    cJSON_Delete(root);
    return true;
}

char *tb_settings_to_json(const tb_settings *s)
{
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        return NULL;
    }
    cJSON_AddNumberToObject(root, "fov", s->fov);
    cJSON_AddNumberToObject(root, "mouseSensitivity", s->mouse_sensitivity);
    cJSON_AddNumberToObject(root, "masterVolume", s->master_volume);
    cJSON_AddNumberToObject(root, "musicVolume", s->music_volume);
    cJSON_AddNumberToObject(root, "resolutionPreset", s->resolution_preset);
    cJSON_AddBoolToObject(root, "paletteQuantize", s->palette_quantize);
    cJSON_AddBoolToObject(root, "dithering", s->dithering);
    cJSON_AddBoolToObject(root, "crtFilter", s->crt_filter);
    cJSON_AddBoolToObject(root, "headBob", s->head_bob);
    cJSON_AddBoolToObject(root, "cameraShake", s->camera_shake);
    cJSON_AddBoolToObject(root, "reduceFlashes", s->reduce_flashes);
    cJSON_AddBoolToObject(root, "subtitles", s->subtitles);
    cJSON_AddStringToObject(root, "touchMode", k_touch[s->touch_mode]);
    cJSON_AddStringToObject(root, "gore", k_gore[s->gore]);
    char *out = cJSON_Print(root);
    cJSON_Delete(root);
    return out;
}
