// player_info.h


#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H


#define PLAYER_SPEED_TICK_COUNT__RESET    0
#define PLAYER_SPEED_TICK_COUNT__DEFAULT 45 // 12 //30 // 15 // 60 frames per second default speed


#define DISPLAY_NEXT_PIECE_TEXT_X 2
#define DISPLAY_NEXT_PIECE_TEXT_Y 9

#define DISPLAY_LEVEL_X 2
#define DISPLAY_LEVEL_Y 3

#define DISPLAY_SCORE_X 2
#define DISPLAY_SCORE_Y 6

#define DISPLAY_NUMPETS_X 2
#define DISPLAY_NUMPETS_Y 12

#define DISPLAY_NUMTILES_X 2
#define DISPLAY_NUMTILES_Y 15


#define SCORE_SCALE_FACTOR 2 //10
#define SCORE_RESET        0
#define PLAYER_LEVEL_RESET    1
#define PLAYER_NUMTILES_RESET 0
#define PLAYER_NUMPETS_RESET  0

#define PLAYER_TILES_TILL_NEXT_LEVEL  20


void score_update(UINT16 num_tiles);
void level_increment(void);

void level_show(void);

void score_reset(void);
void level_reset(void);



#endif // PLAYER_INFO_H

