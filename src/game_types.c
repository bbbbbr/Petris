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

#include "game_board.h"
#include "gameplay.h"

#include "player_info.h"
#include "player_hinting.h"

#include "options.h"

#include "game_types.h"


UINT8 game_type_cleanup_tail_count;
UINT8 game_type_long_pet_required_size;


#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN 3
#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX 20

#define GAME_TYPE_PET_LONG_PET_SIZE_MIN 3
#define GAME_TYPE_PET_LONG_PET_SIZE_MAX 40

void game_types_init(void) {

    game_type_cleanup_tail_count = 0;
    game_type_long_pet_required_size = 0;
}

// TODO: should probably be moved to gameplay_handle_level_transition()
void game_types_handle_level_transition(void) {

    // Handle level transition
    // TODO: reverse or logic and just test for marathon
    if ((option_game_type == OPTION_GAME_TYPE_LEVEL_UP) ||
        (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) ||
        (option_game_type == OPTION_GAME_TYPE_LONG_PET)) {

        // TODO: consolidate with gameplay_init() ??
        // TODO: move this into something like gameplay_level_init()
        //  level_end() and level_start()
        HIDE_SPRITES;

        // PET_LENGTH_PREVIEW_ENABLED_FOR_ALL_MODES
        //
        // if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
            // Remove player pet length hinting sprites
            hinting_petlength_reset();
        // }

        PLAY_SOUND_LEVEL_UP;
        board_hide_all(BRD_CLR_DELAY_CLEAR_MED);

        // Flash a get ready message to the player
        board_flash_message(MSG_LEVEL_UP_X, MSG_LEVEL_UP_Y,
                            MSG_LEVEL_UP_TEXT, MSG_LEVEL_UP_CTEXT,
                            MSG_LEVEL_UP_REPEAT);

        gameplay_prepare_board();

        SHOW_SPRITES;
    }
    else if (option_game_type == OPTION_GAME_TYPE_MARATHON) {
        PLAY_SOUND_LEVEL_UP;
    }
}


UINT8 game_type_pet_cleanup_get_tail_count(UINT8 player_level) {

    UINT8 num_pets;

    num_pets = (player_level / 2) + GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN;

    if (num_pets > GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX) {
        num_pets = GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX;
    }

    return num_pets;
}



void game_type_pet_cleanup_increment_tail_count(void) {

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        if (game_type_cleanup_tail_count < 255) {
            game_type_cleanup_tail_count++;
        }
        // TODO: deduplicate this with player_info_display
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);
    }
}



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



void game_type_long_pet_set_pet_size(UINT8 player_level) {

    game_type_long_pet_required_size = player_level + GAME_TYPE_PET_LONG_PET_SIZE_MIN;

    if (game_type_long_pet_required_size > GAME_TYPE_PET_LONG_PET_SIZE_MAX) {
        game_type_long_pet_required_size = GAME_TYPE_PET_LONG_PET_SIZE_MAX;
    }

    // Display required Pet Size
    // TODO: deduplicate this with player_info_display
    print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_long_pet_required_size, DIGITS_3);
}



void game_type_long_pet_check_size(UINT8 tile_count) {

    if (tile_count >= game_type_long_pet_required_size) {
        // Don't increment the level now since it's
        // in the middle of clearing pieces off the baord
        level_increment_enqueue = TRUE;
    }
}
