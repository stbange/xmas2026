/* SPDX-License-Identifier: MIT */
#include "app/hud.h"

#include <stdio.h>
#include <string.h>

static const uint8_t WHITE[4] = {240, 240, 255, 255};
static const uint8_t GOLD[4] = {255, 204, 77, 255};
static const uint8_t SNOW[4] = {190, 225, 255, 255};
static const uint8_t RED[4] = {255, 90, 90, 255};
static const uint8_t PANEL[4] = {8, 10, 28, 225};

void tb_hud_door_text(const tb_app *app, int day, char *out, size_t out_len)
{
    if (!tb_portal_is_open(app->open, day)) {
        char cd[40];
        tb_portal_countdown_text(&app->calendar, day, cd, sizeof(cd));
        snprintf(out, out_len, "DOOR %d - %s", day, cd[0] ? cd : "OPENS SOON");
        return;
    }
    const tb_level_info *info = tb_level_info_get(day);
    snprintf(out, out_len, "DAY %d - %s - WALK IN%s", day, info->title,
             day == app->calendar_day ? " - NEW!" : "");
}

static void door_labels(const tb_app *app, const tb_scene *scene, float aspect, tb_ui *ui)
{
    tb_mat4 vp = tb_mat4_mul(tb_camera_projection(&scene->camera, aspect), tb_camera_view(&scene->camera));
    tb_vec3 fwd = tb_camera_forward(&scene->camera);
    for (int i = 0; i < TB_PORTAL_COUNT; i++) {
        const tb_hall_door *d = &app->hall.doors[i];
        tb_vec3 at = tb_v3_add(d->centre, tb_v3(0, d->normal.y * 0.7f, TB_HALL_DOOR_HEIGHT + 0.3f));
        tb_vec3 rel = tb_v3_sub(at, scene->camera.eye);
        float dist = tb_v3_len(rel);
        if (dist > 30.0f || tb_v3_dot(rel, fwd) <= 0.5f) {
            continue;
        }
        tb_vec3 ndc = tb_mat4_project(vp, at);
        if (ndc.x < -1 || ndc.x > 1 || ndc.y < -1 || ndc.y > 1) {
            continue;
        }
        int x = (int)((ndc.x * 0.5f + 0.5f) * (float)ui->width);
        int y = (int)((0.5f - ndc.y * 0.5f) * (float)ui->height);
        char num[4];
        snprintf(num, sizeof(num), "%d", d->day);
        bool open = tb_portal_is_open(app->open, d->day);
        tb_ui_text(ui, x, y, dist < 12.0f ? 2 : 1, TB_ALIGN_CENTER, open ? GOLD : SNOW, num);
    }
}

static void panel(tb_ui *ui)
{
    int m = ui->width / 10;
    tb_ui_rect_add(ui, m, ui->height / 8, ui->width - 2 * m, ui->height * 3 / 4, PANEL);
}

static void briefing(const tb_app *app, tb_ui *ui)
{
    const tb_level_info *info = tb_level_info_get(app->level_day);
    char line[64], wrapped[256];
    panel(ui);
    int y = ui->height / 8 + 16;
    snprintf(line, sizeof(line), "DAY %d", app->level_day);
    tb_ui_text(ui, ui->width / 2, y, 2, TB_ALIGN_CENTER, SNOW, line);
    y += 26;
    tb_ui_text(ui, ui->width / 2, y, ui->width >= 720 ? 3 : 2, TB_ALIGN_CENTER, GOLD, info->title);
    y += 40;
    tb_ui_wrap(info->briefing, ui->width / 8 - 16, wrapped, sizeof(wrapped));
    tb_ui_text(ui, ui->width / 2, y, 1, TB_ALIGN_CENTER, WHITE, wrapped);
    y += TB_FONT_LINE * tb_ui_text_lines(wrapped) + 14;
    tb_ui_text(ui, ui->width / 2, y, 1, TB_ALIGN_CENTER, SNOW,
               "THIS IS A TEST COURSE UNTIL THE REAL LEVEL ARRIVES.\nFIND THE BAUBLE, THEN THE EXIT.");
    tb_ui_text(ui, ui->width / 2, ui->height * 7 / 8 - 20, 1, TB_ALIGN_CENTER, GOLD,
               "CLICK, E OR SPACE TO START");
}

static void results(const tb_app *app, tb_ui *ui)
{
    panel(ui);
    tb_ui_text(ui, ui->width / 2, ui->height / 8 + 16, 2, TB_ALIGN_CENTER, GOLD, "MEMORY RESTORED");
    tb_ui_text(ui, ui->width / 2, ui->height / 8 + 52, 1, TB_ALIGN_CENTER, WHITE, app->results);
    tb_ui_text(ui, ui->width / 2, ui->height * 7 / 8 - 20, 1, TB_ALIGN_CENTER, GOLD,
               "CLICK, E OR SPACE TO RETURN TO THE HALL");
}

static void status(const tb_app *app, tb_ui *ui)
{
    const tb_player *p = &app->sim.players[0];
    char line[96];
    /* Crosshair */
    int cx = ui->width / 2, cy = ui->height / 2;
    tb_ui_rect_add(ui, cx - 4, cy, 3, 1, WHITE);
    tb_ui_rect_add(ui, cx + 2, cy, 3, 1, WHITE);
    tb_ui_rect_add(ui, cx, cy - 4, 1, 3, WHITE);
    tb_ui_rect_add(ui, cx, cy + 2, 1, 3, WHITE);
    snprintf(line, sizeof(line), "HP %d  AR %d", p->vitals.health, p->vitals.armour);
    tb_ui_text(ui, 8, ui->height - 22, 2, TB_ALIGN_LEFT, p->vitals.health <= 25 ? RED : WHITE, line);
    const tb_weapon_def *w = tb_weapon_get(tb_inventory_current(&p->inv));
    if (w) {
        if (w->ammo != TB_AMMO_NONE) {
            snprintf(line, sizeof(line), "%s %d", w->name, p->inv.ammo[w->ammo]);
        } else {
            snprintf(line, sizeof(line), "%s", w->name);
        }
        tb_ui_text(ui, ui->width - 8, ui->height - 22, 2, TB_ALIGN_RIGHT, WHITE, line);
    }
    if (p->state != TB_PLAYER_ALIVE) {
        tb_ui_text(ui, cx, cy - 40, 2, TB_ALIGN_CENTER, RED, "SNOWED UNDER... RESPAWNING");
    }
}

void tb_hud_build(const tb_app *app, const tb_scene *scene, float aspect, tb_ui *ui)
{
    char line[128];
    switch (app->flow) {
    case TB_FLOW_BRIEFING:
        briefing(app, ui);
        break;
    case TB_FLOW_RESULTS:
        results(app, ui);
        break;
    case TB_FLOW_LEVEL: {
        status(app, ui);
        const tb_player *p = &app->sim.players[0];
        int f = (p->floppies & 1) + ((p->floppies >> 1) & 1) + ((p->floppies >> 2) & 1);
        snprintf(line, sizeof(line), "%s   FLOPPIES %d/3",
                 app->sim.bauble_collected ? "BAUBLE FOUND - GO TO THE EXIT" : "FIND THE MEMORY BAUBLE", f);
        tb_ui_text_boxed(ui, ui->width / 2, 8, 1, TB_ALIGN_CENTER, GOLD, line);
        break;
    }
    default: { /* hall */
        status(app, ui);
        door_labels(app, scene, aspect, ui);
        const tb_hall_door *d = tb_hall_nearest_door(&app->hall, app->sim.players[0].move.origin, 4.0f);
        if (d) {
            tb_hud_door_text(app, d->day, line, sizeof(line));
            tb_ui_text_boxed(ui, ui->width / 2, ui->height * 3 / 4, 1, TB_ALIGN_CENTER, GOLD, line);
        } else if (app->calendar_day == 0) {
            tb_hud_door_text(app, 1, line, sizeof(line));
            tb_ui_text_boxed(ui, ui->width - 8, 8, 1, TB_ALIGN_RIGHT, SNOW, line);
        }
        break;
    }
    }
    if (app->message_ticks > 0 && app->flow != TB_FLOW_BRIEFING) {
        tb_ui_text_boxed(ui, ui->width / 2, 22, 1, TB_ALIGN_CENTER, SNOW, app->message);
    }
    if (app->args.playtest) {
        snprintf(line, sizeof(line), "PLAYTEST - TICK %llu - ACT %s - %d/24 OPEN",
                 (unsigned long long)app->ts.tick,
                 tb_story_act_name(tb_hall_act(app->calendar_day, app->save.completed)),
                 tb_hall_tree_baubles(app->open));
        tb_ui_text(ui, 8, 8, 1, TB_ALIGN_LEFT, RED, line);
    }
}
