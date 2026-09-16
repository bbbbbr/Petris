// common.h

#ifndef COMMON_H
#define COMMON_H

#define ARRAY_LEN(A)  sizeof(A) / sizeof(A[0])

#define TILE_NUM_0  0u

enum GAME_STATES {
    GAME_INTRO_INIT = 0,
    GAME_INTRO,
    GAME_OPTIONS_INIT,
    GAME_OPTIONS,
    GAME_BOARD_INIT,
    GAME_READY_TO_START,
    GAME_START,
    GAME_PLAYING,
    GAME_ENDED,
    GAME_OVER_SCREEN,
    GAME_OVER_WAITEXIT,
};

#define SHARED_4BPP_TRANSP_TILE_ID_START 0u  // First tile
#define SHARED_4BPP_TRANSP_TILE_COUNT     1u
extern const uint16_t shared_4bpp_transparent_tile[32];

extern uint16_t game_state;
extern uint16_t vbl_count;

#endif