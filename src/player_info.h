// player_info.h


#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H


#define GAME_SPEED_LAUNCH_DELAY_FRAMES         6 // 1/10 of a second (60/10) in practice its n+1 frames due to the state machine lag
#define GAME_SPEED_LAUNCH_DELAY_RESET          0
// https://tetris.wiki/ARE
// ARE, also called entry delay, appearance delay, or spawn delay, is the period of time between the lockdown of one tetromino and the appearance of the next tetromino.

#define GAME_SPEED_DROP_FRAME_COUNTER_RESET    0
// #define GAME_SPEED_FRAMES_PER_DROP_DEFAULT  6 // 12 //30 // 15 // 60 frames per second default speed

#define LINK_STATUS_X 1
#define LINK_STATUS_Y 0

#define DISPLAY_NEXT_PIECE_TEXT_X 1
#define DISPLAY_NEXT_PIECE_TEXT_Y 10

#define GAME_PIECE_NEXT_PREVIEW_BG_X (DISPLAY_NEXT_PIECE_TEXT_X + 4) // Offset 4 tiles from start of text label
#define GAME_PIECE_NEXT_PREVIEW_BG_Y (DISPLAY_NEXT_PIECE_TEXT_Y)

#define GAME_PIECE_NEXT_PREVIEW_X ( (GAME_PIECE_NEXT_PREVIEW_BG_X + 1) * 8)
#define GAME_PIECE_NEXT_PREVIEW_Y ( (GAME_PIECE_NEXT_PREVIEW_BG_Y + 2) * 8)

#define DISPLAY_LEVEL_X 1
#define DISPLAY_LEVEL_Y 4
#define DISPLAY_LEVEL_READOUT_X (DISPLAY_LEVEL_X + 2)

#define DISPLAY_DIFF_X 1
#define DISPLAY_DIFF_Y DISPLAY_LEVEL_Y

#define DISPLAY_SCORE_X 1
#define DISPLAY_SCORE_Y 7

#define DISPLAY_NUMPETS_X 1
#define DISPLAY_NUMPETS_Y 13

#define DISPLAY_NUMTILES_X 1
#define DISPLAY_NUMTILES_Y 16

// #define SCORE_SCALE_FACTOR 4 //10
#define SCORE_RESET        0
#define SCORE_MAX          0xFFFF
#define SCORE_LEVEL_FACTOR_SHIFT 2
#define SCORE_LENGTH_FACTOR_SHIFT 2
#define PLAYER_LEVEL_RESET    1
#define PLAYER_LEVEL_MAX      999
#define PLAYER_NUMTILES_RESET 0
#define PLAYER_NUMPETS_RESET  0
#define PLAYER_NUMPETS_MAX    0xFFFF
#define PLAYER_NUMPIECES_RESET 0
#define PLAYER_NUMPIECES_MAX   0xFFFF

// Transition to a new palette every N levels
#define LEVEL_CHANGE_PAL_NUM 5 // (5 alternate palettes total)

// #define PLAYER_TILES_PER_LEVEL  20 // 25
#define PLAYER_PETS_PER_LEVEL   20 // 10

extern UINT16 player_level;
extern UINT16 player_numpieces;
extern UINT8 level_increment_enqueue;

void score_update(UINT16 num_tiles);
void score_reset(void);

void level_check_update(void);
void level_increment(void);
void level_counters_reset(void);
void level_show(void);

void player_info_display(void);
void player_info_newgame_reset(void);

#endif // PLAYER_INFO_H

