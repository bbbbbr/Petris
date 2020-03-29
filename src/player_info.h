// player_info.h


#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H


#define PLAYER_SPEED_TICK_COUNT__RESET    0
#define PLAYER_SPEED_TICK_COUNT__DEFAULT 45 // 12 //30 // 15 // 60 frames per second default speed


#define DISPLAY_NEXT_PIECE_TEXT_X 1
#define DISPLAY_NEXT_PIECE_TEXT_Y 10
// TODO: handle offset?
#define GAME_PIECE_NEXT_PREVIEW_X ((DISPLAY_NEXT_PIECE_TEXT_X + 5) * 8)
#define GAME_PIECE_NEXT_PREVIEW_Y ((DISPLAY_NEXT_PIECE_TEXT_Y + 2) * 8)

#define DISPLAY_LEVEL_X 1
#define DISPLAY_LEVEL_Y 4

#define DISPLAY_SCORE_X 1
#define DISPLAY_SCORE_Y 7

#define DISPLAY_NUMPETS_X 1
#define DISPLAY_NUMPETS_Y 13

#define DISPLAY_NUMTILES_X 1
#define DISPLAY_NUMTILES_Y 16


#define SCORE_SCALE_FACTOR 2 //10
#define SCORE_RESET        0
#define PLAYER_LEVEL_RESET    1
#define PLAYER_NUMTILES_RESET 0
#define PLAYER_NUMPETS_RESET  0
#define PLAYER_NEW_PIECE_COUNT_RESET 0

#define PLAYER_TILES_TILL_NEXT_LEVEL  20


void score_update(UINT16 num_tiles);
void score_reset(void);

void level_increment(void);
void level_reset(void);
void level_show(void);

void player_info_newgame_reset(void);

void new_piece_count_increment(void);

#endif // PLAYER_INFO_H

