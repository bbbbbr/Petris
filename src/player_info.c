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
UINT16 player_score_last;
UINT16 player_numtiles;
UINT16 player_numpets;
UINT16 player_level;
UINT16 player_numpieces;

UINT8 level_increment_enqueue;



void player_info_display(void) {

    // Display the score
    //
    // There is a static trailing zero after the actual score to
    // reduce size of var needed to show 10x actual score
    if (player_score == SCORE_MAX) {
        PRINT(DISPLAY_SCORE_X -1, DISPLAY_SCORE_Y,"max!:)", 0);
    }
    else if (player_score > 0) {
        print_num_u16(DISPLAY_SCORE_X - 1, DISPLAY_SCORE_Y, player_score, DIGITS_5);
    }

    // Display number of pets completed
    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        // Display Tail remaining count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);

    } else if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
        // Display required Pet Size
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_long_pet_required_size, DIGITS_3);

    } else if ((option_game_type == OPTION_GAME_TYPE_LEVEL_UP)||
               (option_game_type == OPTION_GAME_TYPE_CRUNCH_UP)) {
        // Display pets remaining to complete level
        print_num_u16(DISPLAY_NUMPETS_X + 2, DISPLAY_NUMPETS_Y,
            (PLAYER_PETS_PER_LEVEL * player_level) - player_numpets, DIGITS_3);
    } else {
        // Display total Pet completed count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, player_numpets, DIGITS_5);
    }

    // DISABLED:
    // // Display number of pet segments completed
    // print_num_u16(DISPLAY_NUMTILES_X, DISPLAY_NUMTILES__Y, player_numtiles);
}



void score_update(UINT16 num_tiles) {

    // Check to see whether a special piece (bomb) should be delivered
    if (num_tiles >= p_game_settings->spec_bomb_threshold_pettiles)
        game_piece_next_set(GP_SPECIAL_BOMB);

    // Increase number of pets if this is called with sufficient tiles
    if ((num_tiles > 1) && (player_numpets < PLAYER_NUMPETS_MAX))
        player_numpets++;

    // Increment the total tile count
    player_numtiles += num_tiles;

    // == UPDATE DISPLAY INFO AREA ==

    // Note: Displayed score has an artificial "0" added to the
    // end so that it shows a larger 10x score without having
    // to use a larger variable.
    //
    // Scoring:
    // * Increases semi-exponentially per number of tiles in the pet
    // * Multiplied by (player_level >> N) + 1
    // * Multiplied by game difficulty type factor: score_bonus
    // * Multiplied by a general scale factor: SCORE_SCALE_FACTOR
    player_score_last = player_score;
    player_score += num_tiles
                    * ((num_tiles >> SCORE_LENGTH_FACTOR_SHIFT) + 1)  // + Scale up based on pet length (add 1 to make sure it's not zero)
                    * ((player_level >> SCORE_LEVEL_FACTOR_SHIFT) + 1) // | 1 to make sure it's not zero
                    * (UINT16)p_game_settings->score_bonus;
                    // * SCORE_SCALE_FACTOR;
    // Prevent wraparound
    if (player_score < player_score_last)
        player_score = SCORE_MAX;



    player_info_display();

    // == CHECK FOR LEVEL CHANGES ==
    //
    // (was consolidated from level_check_update() )
    //
    // Should be called after displaying tail/pet count
    // so they display correctly during potential level changes

    // Queue Check for level change in game types based on
    // number of pets completed.
    // Long Pet and Pet Cleanup don't need it since they
    // get triggered by specific completed tasks
    if ((option_game_type == OPTION_GAME_TYPE_MARATHON) ||
        (option_game_type == OPTION_GAME_TYPE_LEVEL_UP) ||
        (option_game_type == OPTION_GAME_TYPE_CRUNCH_UP)) {

        if (player_numpets >= (PLAYER_PETS_PER_LEVEL * player_level)) {
            level_increment_enqueue = TRUE;
        }
    }

    // Check for level updates
    // (either triggered directly above OR elsewhere)
    if (level_increment_enqueue == TRUE) {
        level_increment();

        // After the new level transition is complete
        // update the player info again to show the new level
        player_info_display();
    }
}


void score_reset(void) {

    player_score_last = player_score = SCORE_RESET;
}



void level_increment(void) {

    // Clear possible queued level increment
    level_increment_enqueue = FALSE;

    if (player_level < PLAYER_LEVEL_MAX) {
        player_level++;
    }

    // Update game piece speed
    gameplay_drop_speed_update();

    game_types_handle_level_transition();

    if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
        game_type_long_pet_set_pet_size( (UINT8)player_level );
    }

    // DEBUG: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
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
}


void player_info_newgame_reset(void) {

    score_reset();
    level_counters_reset();
    level_increment_enqueue = FALSE;

    // Should be called after level_counters_reset()
    // Set initial game piece speed
    gameplay_drop_speed_update();

    if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
        game_type_long_pet_set_pet_size( (UINT8)player_level );
    }

    level_show();
    player_info_display();
}
