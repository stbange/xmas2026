/* SPDX-License-Identifier: MIT */
/* The golden-path test bot: follows a level's scripted route, uses the
 * level's mechanics (use, jetpack, weapons) and fights whatever it sees.
 * Game tests run it headless to prove every level can be finished. */
#ifndef TB_GAME_BOT_H
#define TB_GAME_BOT_H

#include "sim/sim.h"

typedef enum tb_bot_op {
    TB_BOT_GOTO = 0,   /* walk to `at` */
    TB_BOT_JET_TO,     /* walk/fly to `at` holding the jetpack */
    TB_BOT_USE,        /* look at `at` and press Use */
    TB_BOT_WAIT,       /* wait `arg` ticks */
    TB_BOT_SELECT,     /* select weapon slot `arg` */
    TB_BOT_CLEAR,      /* fight until no enemy is alive near `at` (timeout `arg` ticks) */
    TB_BOT_CHECKPOINT, /* resume here after dying */
} tb_bot_op;

typedef struct tb_bot_step {
    tb_bot_op op;
    tb_vec3 at;
    int arg;
} tb_bot_step;

typedef struct tb_bot {
    const tb_bot_step *steps;
    int count;
    int index;
    int resume;
    uint32_t step_ticks;
    uint32_t deaths_seen;
    bool done;
    bool failed;
    bool pressed;
    char reason[96];
} tb_bot;

#define TB_BOT_STEP_TIMEOUT (40u * TB_SIM_HZ_TICKS)
#define TB_BOT_FIGHT_RANGE  26.0f
#define TB_BOT_CLEAR_RADIUS 22.0f
#define TB_BOT_MAX_DEATHS   8

void tb_bot_init(tb_bot *bot, const tb_bot_step *steps, int count);
/* This tick's input for the bot's player. */
tb_input tb_bot_think(tb_bot *bot, tb_sim *sim, int slot);

#endif
