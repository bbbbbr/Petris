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
#include "options.h"

UINT16 player_score;
UINT16 player_numtiles; // UINT32?
UINT16 player_numpets;
UINT16 player_level;
UINT16 player_numpieces;

const UINT8 NEXT_PIECE_BG_TILE = TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG;
const UINT8 NEXT_PIECE_BG_PAL  = BG_PAL_BOARD_NEXT_PIECE_PREVIEW;

// TODO: move into game_type.c/h ??
UINT8 game_type_cleanup_tail_count;

UINT8 level_increment_enqueue;

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

    // Display number of pets completed
    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        // Display Tail remaining count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);
    } else {
        // Display Pet compelted count
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, player_numpets, DIGITS_5);
    }

    // Should be called after displaying tail/pet count
    // so they display correctly during potential level changes
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


}



void score_reset(void) {
    player_score = 0;
    score_update(0); // TODO: move this out of here?
}



void level_check_update(void) {


    // TODO: finalize on whether levels increment based on pets or pet tile count
    //if (player_numtiles >= (PLAYER_TILES_PER_LEVEL * (player_level + 1))) {
    //  OR could do newlevel = (player_numtiles / PLAYER_TILES_PER_LEVEL) = (PLAYER_TILES_PER_LEVEL * (player_level + 1))
    if (level_increment_enqueue == TRUE) {

        level_increment();

    } else if ((option_game_type != OPTION_GAME_TYPE_PET_CLEANUP) &&
               (player_numpets >= (PLAYER_PETS_PER_LEVEL * (player_level + 1)))) {

        level_increment();
    }

}



void level_increment(void) {

    // TODO:    PLAY_SOUND_LEVEL_UP; // TODO: this needs a delay after last piece clear sound
    // TODO: ???? reset or NOT reset number of pets/tiles completed per level? May depend on play mode

    // Clear possible queued level increment
    level_increment_enqueue = FALSE;

    if (player_level < PLAYER_LEVEL_MAX) {
        player_level++;

        // TODO: level_handle_next() ?? Do more things?

        // TODO: ?? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
        game_speed_frames_per_drop_set( options_frames_per_drop_get((UINT8)player_level) );

        level_show();

        // TODO: move to game_type_handle_level_increment() ? game_type.c
        if (option_game_type == OPTION_GAME_TYPE_LEVEL_UP) {
            // if ((level % MODE_LEVELUP_LEVEL_INCREMENT) == 0) {

                // TODO: move this into something like gameplay_level_init()
                HIDE_SPRITES;

                PLAY_SOUND_LEVEL_UP;
                board_hide_all(BRD_CLR_DELAY_CLEAR_MED);
                // Flash a get ready message to the player
                board_flash_message(MSG_LEVEL_UP_X, MSG_LEVEL_UP_Y,
                                    MSG_LEVEL_UP_TEXT, MSG_LEVEL_UP_CTEXT,
                                    MSG_LEVEL_UP_REPEAT);

                board_reset();

                // Generate the very first piece
                game_piece_next_reset();
                game_piece_next_generate();

                SHOW_SPRITES;
        }
        else if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {

                // TODO: move this into something like gameplay_level_init()
                HIDE_SPRITES;

                PLAY_SOUND_LEVEL_UP;
                board_hide_all(BRD_CLR_DELAY_CLEAR_MED);
                // Flash a get ready message to the player
                board_flash_message(MSG_LEVEL_UP_X, MSG_LEVEL_UP_Y,
                                    MSG_LEVEL_UP_TEXT, MSG_LEVEL_UP_CTEXT,
                                    MSG_LEVEL_UP_REPEAT);

                board_reset();

                // Generate the very first piece
                game_piece_next_reset();
                game_piece_next_generate();

                // TODO: consolidate with other call of same
                // This will auto-increment game_type_cleanup_tail_count
                game_board_fill_random_tails( game_type_pet_cleanup_get_tail_count( (UINT8)player_level ));

                SHOW_SPRITES;
        }

    // TODO: Debug: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
    #ifdef DEBUG_SHOW
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y + 1, (UINT16)game_speed_frames_per_drop, DIGITS_5);
    #endif
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

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "TAILS", 0);
    } else {
        PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "PETS", 0);
    }
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
    level_increment_enqueue = FALSE;

    // Should be called after level_counters_reset()
    game_speed_frames_per_drop_set( options_frames_per_drop_get( (UINT8)player_level) );
    // TODO: ???? change this to options_frames_per_drop_update() call -> game_speed_frames_per_drop_set()
    // OR, level_update_speed()
    // OR, gameplay_speed_update() <------ ???

// TODO CONSOLIDATE ME
    switch (option_game_type) {
        case OPTION_GAME_TYPE_PET_CLEANUP:
            // TODO: consolidate with other call of same
            // This will auto-increment game_type_cleanup_tail_count
            game_board_fill_random_tails( game_type_pet_cleanup_get_tail_count( (UINT8)player_level ));
            break;
    }

    // TODO: Debug: frames per drop (requires extern UINT8 game_speed_frames_per_drop;)
    #ifdef DEBUG_SHOW
        print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y + 1, (UINT16)game_speed_frames_per_drop, DIGITS_5);
    #endif
}

#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN 3
#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX 20


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
        if (game_type_cleanup_tail_count < 255)
            game_type_cleanup_tail_count++;
    }

    // TODO: remove?
    print_num_u16(DISPLAY_NUMPETS_X, DISPLAY_NUMPETS_Y, (UINT16)game_type_cleanup_tail_count, DIGITS_5);
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