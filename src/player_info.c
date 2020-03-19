// player_info.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "gfx.h"
#include "gfx_print.h"
#include "player_info.h"

UINT16 player_score;
UINT16 player_numtiles;
UINT16 player_numpets;
UINT16 player_level;

#define DISPLAY_LEVEL_X 2
#define DISPLAY_LEVEL_y 2

#define DISPLAY_SCORE_X 2
#define DISPLAY_SCORE_y 4

#define SCORE_SCALE_FACTOR 2 //10
#define SCORE_RESET        0
#define PLY_LEVEL_RESET    1
#define PLY_NUMTILES_RESET 0
#define PLY_NUMPETS_RESET  0


extern UINT8 tick_frame_speed;


void score_update(UINT16 num_tiles) {

    // Increase number of pets if this is called with sufficient tiles
    if (num_tiles > 1)
        player_numpets++;
    player_numtiles += num_tiles;

    if (player_numtiles >= 10) {
        level_increment();
    }

    player_score += num_tiles * num_tiles * SCORE_SCALE_FACTOR * player_level; // TODO: support x 10 scoring? Need to use a 24 bit Num

    // Display the score
    print_num_u16(DISPLAY_SCORE_X, DISPLAY_SCORE_y, player_score);

    // Display number of pet segments completed
    print_num_u16(DISPLAY_SCORE_X, DISPLAY_SCORE_y + 2, player_numtiles);

    // Display number of pets completed
    print_num_u16(DISPLAY_SCORE_X, DISPLAY_SCORE_y + 4, player_numpets);
}


void level_increment(void) {

    player_level++;
    // TODO: reset or NOT reset number of pets/tiles completed per level? May depend on play mode
    // player_numpets  = PLY_NUMPETS_RESET;
    // player_numtiles = PLY_NUMTILES_RESET;

    // TODO: level_handle_next()
    // TODO: level_speed_get()
    // incerase level speed
    if (tick_frame_speed)
        tick_frame_speed--;     // TODO : Use a proper accessor/setter

    print_num_u16(DISPLAY_LEVEL_X, DISPLAY_LEVEL_y, player_level);
}


void level_show(void) {

    print_num_u16(DISPLAY_LEVEL_X, DISPLAY_LEVEL_y, player_level);
}


void score_reset(void) {
    player_score = 0;
    score_update(0); // TODO: move this out of here?
}

void level_reset(void) {
    player_numpets  = PLY_NUMPETS_RESET;
    player_numtiles = PLY_NUMTILES_RESET;
    player_level    = PLY_LEVEL_RESET;
    level_show();   // TODO: move this out of here?
}