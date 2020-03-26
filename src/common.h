// common.h

#ifndef COMMON_H
#define COMMON_H

// Enable CGB Fast cpu mode
// #define CPU_FAST_ENABLED

typedef enum {
    GAME_INTRO_INIT = 0,
    GAME_INTRO,
    GAME_OPTIONS_INIT,
    GAME_OPTIONS,
    GAME_BOARD_INIT,
    GAME_READY_TO_START,
    GAME_START,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_ENDED,
    GAME_OVER_SCREEN,
} GAME_STATES;

extern UINT8 game_state;


#endif