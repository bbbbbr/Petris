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
#include "gameplay.h"
#include "options.h"

UINT16 player_score;
UINT16 player_numtiles; // UINT32?
UINT16 player_numpets;
UINT16 player_level;
UINT16 player_numpieces;

const UINT8 NEXT_PIECE_BG_TILE = TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG;
const UINT8 NEXT_PIECE_BG_PAL  = BG_PAL_BOARD_NEXT_PIECE_PREVIEW;


void new_piece_count_increment(void) {
    player_numpieces++;

    // Check to see whether a special piece (merge) should be delivered
    if ((player_numpieces & p_game_settings->spec_merge_threshold_pieces) == p_game_settings->spec_merge_threshold_pieces)
        game_piece_next_set(GP_SPECIAL_LIGHTENING);

}

extern game_speed_frames_per_drop; // TODO: DEBUG: REMOVE


void score_update(UINT16 num_tiles) {

    // Check to see whether a special piece (bomb) should be delivered
    if (num_tiles >= p_game_settings->spec_bomb_threshold_pettiles)
        game_piece_next_set(GP_SPECIAL_BOMB);

    // Increase number of pets if this is called with sufficient tiles
    if (num_tiles > 1)
        player_numpets++;

    // Increment the total title count
    // TODO: player_numtiles_this_level
    player_numtiles += num_tiles;

    level_check_update();

    // TODO: support x 10 scoring? Need to use a 24 bit Num
    // Scoring:
    // * Increases exponentially per number of tiles in the pet
    // * Multiplied by (player_level / 10)
    // * Multiplied by game difficulty type factor: score_bonus
    // * Multiplied by a general scale factor: SCORE_SCALE_FACTOR
    player_score += (num_tiles * num_tiles)
                    * ((player_level >> SCORE_LEVEL_FACTOR_SHIFT) + 1) // | 1 to make sure it's not zero
                    * (UINT16)p_game_settings->score_bonus
                    * SCORE_SCALE_FACTOR;

    // Display the score
    print_num_u16(DISPLAY_SCORE_X, DISPLAY_SCORE_Y, player_score, DIGITS_5);

    // // Display number of pet segments completed
    // print_num_u16(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES__Y, player_numtiles);

    // Display number of pets completed
    print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, player_numpets, DIGITS_5);
}



void score_reset(void) {
    player_score = 0;
    score_update(0); // TODO: move this out of here?
}



void level_check_update(void) {


    // TODO: finalize on whether levels increment based on pets or pet tile count
    //if (player_numtiles >= (PLAYER_TILES_PER_LEVEL * (player_level + 1))) {
    //  OR could do newlevel = (player_numtiles / PLAYER_TILES_PER_LEVEL) = (PLAYER_TILES_PER_LEVEL * (player_level + 1))
    if (player_numpets >= (PLAYER_PETS_PER_LEVEL * (player_level + 1))) {
        level_increment();
    }

}



void level_increment(void) {

    // TODO:    PLAY_SOUND_LEVEL_UP; // TODO: this needs a delay after last piece clear sound
    // TODO: ???? reset or NOT reset number of pets/tiles completed per level? May depend on play mode

    if (player_level < PLAYER_LEVEL_MAX) {
        player_level++;

        // TODO: level_handle_next() ?? Do more things?

        // TODO: ?? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
        game_speed_frames_per_drop_set( options_frames_per_drop_get((UINT8)player_level) );

        level_show();


// TODO: Debug: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y + 2, (UINT16)game_speed_frames_per_drop, DIGITS_5);

    }
}



void level_show(void) {

    print_num_u16(DISPLAY_LEVEL_READOUT_X, DISPLAY_LEVEL_Y, player_level, DIGITS_3);
}



void level_counters_reset(void) {

    player_level     = PLAYER_LEVEL_RESET;
    player_numpets   = PLAYER_NUMPETS_RESET;
    player_numtiles  = PLAYER_NUMTILES_RESET;
    player_numpieces = PLAYER_NUMPIECES_RESET;

    level_show();   // TODO: move this out of here?
}


void player_info_newgame_reset(void) {
    PRINT(DISPLAY_NEXT_PIECE_TEXT_X,    DISPLAY_NEXT_PIECE_TEXT_Y - 1,    "NEXT:", 0);

    PRINT(DISPLAY_LEVEL_X,    DISPLAY_LEVEL_Y - 1,    "LEVEL", 0);
     // On same line as level readout
    PRINT(DISPLAY_DIFF_X,     DISPLAY_DIFF_Y,         options_difficulty_abbrev_text_get(), 0);

    PRINT(DISPLAY_SCORE_X,    DISPLAY_SCORE_Y - 1,    "SCORE", 0);
    PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "PETS", 0);
//    PRINT(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES_Y - 1, "TILES", 0);

    // Set the preview tile area to a white background

    VBK_REG = 1; // Select BG tile attribute map
    set_bkg_tiles(GAME_PIECE_NEXT_PREVIEW_BG_X, GAME_PIECE_NEXT_PREVIEW_BG_Y,
                  1, 1, &NEXT_PIECE_BG_PAL);

    VBK_REG = 0; // Re-Select regular BG tile map
    set_bkg_tiles(GAME_PIECE_NEXT_PREVIEW_BG_X, GAME_PIECE_NEXT_PREVIEW_BG_Y,
                  1, 1, &NEXT_PIECE_BG_TILE);

    score_reset();
    level_counters_reset();

    // Should be called after level_counters_reset()
    game_speed_frames_per_drop_set( options_frames_per_drop_get((UINT8)player_level) );
    // TODO: ???? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
    // OR, level_update_speed()
    // OR, gameplay_speed_update() <------ ???

// TODO: Debug: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y + 2, (UINT16)game_speed_frames_per_drop, DIGITS_5);
}