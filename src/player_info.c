// player_info.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "audio_common.h"
#include "gfx.h"
#include "gfx_print.h"
#include "player_info.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board_special_pieces.h"

UINT16 player_score;
UINT16 player_numtiles;
UINT16 player_numpets;
UINT16 player_level;


extern UINT8 tick_frame_speed;


void score_update(UINT16 num_tiles) {

    if (num_tiles >= PIECE_SPECIAL_THRESHOLD_BOMB)
        game_piece_next_set(GP_SPECIAL_BOMB);

    // Increase number of pets if this is called with sufficient tiles
    if (num_tiles > 1)
        player_numpets++;
    player_numtiles += num_tiles;

    // TODO move to level_check_update()
    // TODO: this is just a hack for now, large pets could skip a level (player_level = (player_level / PLAYER_TILES_TILL_NEXT_LEVEL))
    if (player_numtiles >= (PLAYER_TILES_TILL_NEXT_LEVEL * (player_level + 1))) {
        level_increment();
    }

    player_score += num_tiles * num_tiles * SCORE_SCALE_FACTOR * player_level; // TODO: support x 10 scoring? Need to use a 24 bit Num

    // Display the score
    print_num_u16(DISPLAY_SCORE_X, DISPLAY_SCORE_Y, player_score);

    // Display number of pet segments completed
    print_num_u16(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES_Y, player_numtiles);

    // Display number of pets completed
    print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, player_numpets);
}


void level_increment(void) {

//     PLAY_SOUND_LEVEL_UP; // TODO: this needs a delay after last piece clear sound
    player_level++;
    // TODO: reset or NOT reset number of pets/tiles completed per level? May depend on play mode
    // player_numpets  = PLAYER_NUMPETS_RESET;
    // player_numtiles = PLAYER_NUMTILES_RESET;

    // TODO: level_handle_next()
    // TODO: level_speed_get()
    // incerase level speed
    if (tick_frame_speed)
        tick_frame_speed--;     // TODO : Use a proper accessor/setter

    print_num_u16(DISPLAY_LEVEL_X, DISPLAY_LEVEL_Y, player_level);
}


void level_show(void) {

    print_num_u16(DISPLAY_LEVEL_X, DISPLAY_LEVEL_Y, player_level);
}


void score_reset(void) {
    player_score = 0;
    score_update(0); // TODO: move this out of here?
}

void level_reset(void) {

    // TODO: move to level_init() ?
    PRINT(DISPLAY_NEXT_PIECE_TEXT_X,    DISPLAY_NEXT_PIECE_TEXT_Y - 1,    "NEXT:", 0);

    PRINT(DISPLAY_LEVEL_X,    DISPLAY_LEVEL_Y - 1,    "LEVEL", 0);
    PRINT(DISPLAY_LEVEL_X,    DISPLAY_LEVEL_Y - 1,    "LEVEL", 0);
    PRINT(DISPLAY_SCORE_X,    DISPLAY_SCORE_Y - 1,    "SCORE", 0);
    PRINT(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES_Y - 1, "TILES", 0);
    PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "PETS", 0);

    player_numpets  = PLAYER_NUMPETS_RESET;
    player_numtiles = PLAYER_NUMTILES_RESET;
    player_level    = PLAYER_LEVEL_RESET;
    level_show();   // TODO: move this out of here?
}