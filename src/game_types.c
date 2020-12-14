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

// game_types.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "audio_common.h"
#include "gfx.h"
#include "gfx_print.h"
#include "fade2pal.h"


#include "game_board.h"
#include "gameplay.h"

#include "player_info.h"
#include "player_hinting.h"

#include "options.h"

#include "game_types.h"


UINT8 game_type_cleanup_tail_count = 0;
UINT8 game_type_long_pet_required_size = 0;


void game_types_init(void) {

    game_type_cleanup_tail_count = 0;
    game_type_long_pet_required_size = 0;
}


void game_types_handle_level_transition(void) {

    // Handle level transition

    PLAY_SOUND_LEVEL_UP;

    // Minimal transition for marathon long play mode
    if (option_game_type == OPTION_GAME_TYPE_MARATHON) {

        // Update level indicator
        level_show();

        // Do a color palette fade every N levels
        if ((player_level % LEVEL_CHANGE_PAL_NUM) == 0) {

            fade2pal_start_next();
        }

    } else {

        // Implied:
        // if ((option_game_type == OPTION_GAME_TYPE_LEVEL_UP) ||
        //     (option_game_type == OPTION_GAME_TYPE_CRUNCH_UP) ||
        //     (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) ||
        //     (option_game_type == OPTION_GAME_TYPE_LONG_PET)) {

        // NOTE: The specific sequence in here is important for user experience

        HIDE_SPRITES;

        // Remove player pet length hinting sprites
        hinting_petlength_reset();

        board_hide_all(BRD_CLR_DELAY_CLEAR_MED);

        PRINT(MSG_LEVEL_UP_X, MSG_LEVEL_UP_Y, MSG_LEVEL_UP_CTEXT,0);

        // Update level indicator before populating the board
        level_show();

        // Do a color palette fade every N levels
        if ((player_level % LEVEL_CHANGE_PAL_NUM) == 0) {

            fade2pal_start_next();
        }

        // Show preview of upcoming piece so player has time to check it out
        // Wait one frame (vbl) after the update so there is time for the player
        // piece sprite location to be updated before re-showing the sprites.
        gameplay_prepare_piece();
        wait_vbl_done();
        SHOW_SPRITES;

        // Flash a get ready message to the player
        board_flash_message(MSG_LEVEL_UP_X, MSG_LEVEL_UP_Y,
                            MSG_LEVEL_UP_TEXT, MSG_LEVEL_UP_CTEXT,
                            MSG_LEVEL_UP_REPEAT);

        gameplay_prepare_board();

    }
}



// Calculate how many tails to add to the board based on current level
UINT8 game_type_pet_cleanup_get_tail_count(void) {

    UINT8 num_pets;

    num_pets = (player_level / 2) + GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN;

    if (num_pets > GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX) {
        num_pets = GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX;
    }

    return num_pets;
}



// Keeps track of how many tail pieces have been added to the board.
// That value gets decremented. Once zero level is complete (in tail cleanup mode)
void game_type_pet_cleanup_increment_tail_count(void) {

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        if (game_type_cleanup_tail_count < 255) {
            game_type_cleanup_tail_count++;
        }

        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);
    }
}




// Log removal of tail pieces from the board.
// Once zero level is complete (in tail cleanup mode)
void game_type_pet_cleanup_decrement_tail_count(void) {

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {

        if (game_type_cleanup_tail_count > 0) {
            game_type_cleanup_tail_count--;
        }

        if (game_type_cleanup_tail_count == 0) {
            // Don't increment the level now since it's
            // in the middle of clearing pieces off the baord
            level_increment_enqueue = TRUE;
        }
    }
}



// Calculate length of pet required to complete level, based on current player level
void game_type_long_pet_set_pet_size(UINT8 player_level) {

    game_type_long_pet_required_size = player_level + GAME_TYPE_PET_LONG_PET_SIZE_MIN;

    if (game_type_long_pet_required_size > GAME_TYPE_PET_LONG_PET_SIZE_MAX) {
        game_type_long_pet_required_size = GAME_TYPE_PET_LONG_PET_SIZE_MAX;
    }

    // Display required Pet Size
    print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_long_pet_required_size, DIGITS_3);
}



// Check to see whether the size requirement has been met in
// long pet mode. If so, flag the level as complete
void game_type_long_pet_check_size(UINT8 tile_count) {

    if (tile_count >= game_type_long_pet_required_size) {
        // Don't increment the level now since it's
        // in the middle of clearing pieces off the baord
        level_increment_enqueue = TRUE;
    }
}
