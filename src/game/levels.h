/* SPDX-License-Identifier: MIT */
/* The 24 levels: in-game titles and NOEL's one-line briefing. The games
 * that inspired them are named only in the credits ("inspired by"). */
#ifndef TB_GAME_LEVELS_H
#define TB_GAME_LEVELS_H

#include "core/types.h"

typedef enum tb_story_act {
    TB_ACT_BOOT = 0,     /* days 1-8 */
    TB_ACT_DEFRAG = 1,   /* days 9-16 */
    TB_ACT_SHUTDOWN = 2, /* days 17-24 */
    TB_ACT_RESTORED = 3  /* after level 24 */
} tb_story_act;

typedef struct tb_level_info {
    int day;
    const char *title;
    const char *briefing;    /* NOEL's one-line summary */
    const char *inspired_by; /* credits only */
} tb_level_info;

/* Day 1..24, or NULL. */
const tb_level_info *tb_level_info_get(int day);
tb_story_act tb_level_act(int day);
/* The hall's mood: by calendar day (0 = before December, 25 = after
 * Christmas Eve) unless level 24 has been finished (restored). */
tb_story_act tb_hall_act(int calendar_day, uint32_t completed_mask);
const char *tb_story_act_name(tb_story_act act);

#endif
