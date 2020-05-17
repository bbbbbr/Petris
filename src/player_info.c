// Copyright 2020 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

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
#include "game_board.h"
#include "game_board_special_pieces.h"

#include "gameplay.h"
#include "gameplay.h"

#include "game_types.h"

#include "options.h"

UINT16 player_score;
UINT16 player_numtiles; // UINT32?
UINT16 player_numpets;
UINT16 player_level;
UINT16 player_numpieces;

UINT8 level_increment_enqueue;

void new_piece_count_increment(void) {
    player_numpieces++;

    // Check to see whether a special piece (merge) should be delivered
    if ((player_numpieces & p_game_settings->spec_merge_threshold_pieces) == p_game_settings->spec_merge_threshold_pieces)
        game_piece_next_set(GP_SPECIAL_LIGHTENING);

}

// extern game_speed_frames_per_drop; // TODO: DEBUG: REMOVE


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


    // == UPDATE DISPLAY INFO AREA ==

    // Display number of pets completed
    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        // Display Tail remaining count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);
    } else {
        // Display Pet compelted count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, player_numpets, DIGITS_5);
    }

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

    // TODO: remove?
    // // Display number of pet segments completed
    // print_num_u16(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES__Y, player_numtiles);


    // == CHECK FOR LEVEL CHANGES ==
    //
    // (was consolidated from level_check_update() )
    //
    // Should be called after displaying tail/pet count
    // so they display correctly during potential level changes

    // Check for level updates
    if ((level_increment_enqueue == TRUE) ||
        ((option_game_type != OPTION_GAME_TYPE_PET_CLEANUP) &&
         (player_numpets >= (PLAYER_PETS_PER_LEVEL * (player_level + 1))))) {

        level_increment();
    }
}


void score_reset(void) {
    player_score = 0;
    score_update(0); // TODO: move this out of here?
}


// TODO: remove?
// // TODO: Optimize: ? convert this to not being a function?
// void level_check_update(void) {

//     // Trigger a new level either if it's enqueued (by pet cleanup),
//     // or if the level threshold is crossed and game type *is not* pet cleanup
//     if ((option_game_type != OPTION_GAME_TYPE_PET_CLEANUP) &&
//          (player_numpets >= (PLAYER_PETS_PER_LEVEL * (player_level + 1)))) {

//         level_increment_enqueue = TRUE;
//     }


//     if (level_increment_enqueue == TRUE) {

//         level_increment();
//     }

// }



void level_increment(void) {

    // Clear possible queued level increment
    level_increment_enqueue = FALSE;

    if (player_level < PLAYER_LEVEL_MAX) {
        player_level++;
    }

    // TODO: ?? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
    game_speed_frames_per_drop_set( options_frames_per_drop_get((UINT8)player_level) );

    game_types_handle_level_transition();

    level_show();


    // TODO: Debug: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
    #ifdef DEBUG_SHOW
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y + 1, (UINT16)game_speed_frames_per_drop, DIGITS_5);
    #endif

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

    score_reset();
    level_counters_reset();
    level_increment_enqueue = FALSE;

    // TODO: ???? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
    // OR, level_update_speed()
    // OR, gameplay_speed_update() <------ ???
    // Should be called after level_counters_reset()
    game_speed_frames_per_drop_set( options_frames_per_drop_get( (UINT8)player_level) );
}
