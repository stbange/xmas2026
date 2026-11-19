/* SPDX-License-Identifier: MIT */
/* The HUD and full-screen panels (briefing, results), built as UI
 * rectangles from the session state. */
#ifndef TB_APP_HUD_H
#define TB_APP_HUD_H

#include "app/app.h"
#include "render/scene.h"
#include "render/ui.h"

/* `scene` provides the camera for door labels; `aspect` is the scene's. */
void tb_hud_build(const tb_app *app, const tb_scene *scene, float aspect, tb_ui *ui);
/* The text shown for an advent door: "DOOR 7 - OPENS IN 3 D 14 H" or
 * "DAY 7 - BAUBLEGUM CITY - WALK IN" (+ " - NEW!" for today's door). */
void tb_hud_door_text(const tb_app *app, int day, char *out, size_t out_len);

#endif
