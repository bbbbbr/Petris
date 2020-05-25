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

// player_hinting.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "common.h"
#include "input.h"
#include "sound.h"
#include "gfx.h"
#include "gfx_print.h"

#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"

#include "player_piece.h"
#include "player_hinting.h"
#include "player_info.h"
#include "player_gfx.h"

#include "gameplay.h"

extern  INT8 player_x;
extern  INT8 player_y;
extern  INT8 player_rotate; // Uses wraparound, so allow negative nums
extern UINT8 player_piece;
extern UINT8 player_attrib;


// Hint sprites are arranged clockwise around player piece
// --> Left, Top, Right, Bottom
const INT8 SPR_HINT_OFFSET_LUT_X[] = {-8,  0, 8, 0};
const INT8 SPR_HINT_OFFSET_LUT_Y[] = { 0, -8, 0, 8};

const UINT8 GP_SPECIAL_HINT_LUT[] = {GP_SPECIAL_BOMB_HINT, // for GP_SPECIAL_BOMB
                                     GP_SPECIAL_MERGE_HINT}; // for GP_SPECIAL_LIGHTENING

UINT8 hint_special_tile;

#define HINT_PET_LENGTH_SLOT_EMPTY -1

// INT8  spr_long_pet_hint_x[SPR_LONG_PET_HINT_COUNT_MAX];
// INT8  spr_long_pet_hint_y[SPR_LONG_PET_HINT_COUNT_MAX];
// UINT8 spr_long_pet_hint_max;



// Apply sprite flicker to special piece hinting and drop hinting if needed
void player_hinting_flicker_update(UINT8 frame_count) {

    if ((frame_count & 0x0F) == 0x00)
        player_hinting_special_show(TRUE);
    else if ((frame_count & 0x0F) == 0x08)
        player_hinting_special_show(FALSE);
}



void player_hinting_special_show(UINT8 do_show) {

    if ((do_show) && (player_piece & GP_SPECIAL_MASK)) {

        // Update sprite to use visible tiles
        set_sprite_tile(SPR_SPECIAL_HINT_1, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_2, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_3, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_4, hint_special_tile);

    } else {
        // Hide sprite
        // move_sprite(SPR_SPECIAL_HINT_1, 0,0);
        // move_sprite(SPR_SPECIAL_HINT_2, 0,0);
        // move_sprite(SPR_SPECIAL_HINT_3, 0,0);
        // move_sprite(SPR_SPECIAL_HINT_4, 0,0);
        //
        // Hide using a clear tile, maybe it's a little faster? // TODO
        set_sprite_tile(SPR_SPECIAL_HINT_1, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_2, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_3, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_4, GP_EMPTY);
    }
}


// NOTE: expects to only be called if (player_piece & GP_SPECIAL_MASK)
//
//
// TODO: OPTIMIZE: if there is CPU time left, roll this into a loop?
void player_hinting_special_move(void) {

        // TODO : OPTIMIZE : Better to avoid re-multiplying and adding all of these each time
        move_sprite(SPR_SPECIAL_HINT_1,
                    (player_x * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_X[0] + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_Y[0] + BRD_PIECE_Y_OFFSET);

        move_sprite(SPR_SPECIAL_HINT_2,
                    (player_x * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_X[1] + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_Y[1] + BRD_PIECE_Y_OFFSET);

        move_sprite(SPR_SPECIAL_HINT_3,
                    (player_x * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_X[2] + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_Y[2] + BRD_PIECE_Y_OFFSET);

        move_sprite(SPR_SPECIAL_HINT_4,
                    (player_x * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_X[3] + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + SPR_HINT_OFFSET_LUT_Y[3] + BRD_PIECE_Y_OFFSET);
}


// NOTE: expects to only be called if (player_piece & GP_SPECIAL_MASK)
//
//
// TODO: OPTIMIZE: if there is CPU time left, roll this into a loop?
void player_hinting_special_update_gfx() {

        hint_special_tile = GP_SPECIAL_HINT_LUT[ player_piece - GP_SPECIAL_START ];

        set_sprite_tile(SPR_SPECIAL_HINT_1, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_2, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_3, hint_special_tile);
        set_sprite_tile(SPR_SPECIAL_HINT_4, hint_special_tile);
        // Set Pal and attribs
        set_sprite_prop(SPR_SPECIAL_HINT_1, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_2, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_3, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_4, GP_PAL_SPECIAL);
}



void player_hinting_drop_show(UINT8 do_show) {

    if (do_show) {

        // Update sprite to use visible tile
        set_sprite_tile(SPR_DROP_HINT, GP_SPECIAL_DROP_HINT);
        set_sprite_prop(SPR_DROP_HINT, GP_PAL_DROPHINT);

    } else {
        // Hide sprite
        // move_sprite(SPR_DROP_HINT, 0,0);
        // Hide using a clear tile, maybe it's a little faster? // TODO
        set_sprite_tile(SPR_DROP_HINT, GP_EMPTY);
    }
}



void player_hinting_drop_update(void) {

    INT8 drop_hint_y;

    drop_hint_y = board_find_lowest_open_in_column(player_x, player_y);

    // Only update the hint if it's not overlapping with the player
    // otherwise, hide it
    if (player_y != drop_hint_y) {
        move_sprite(SPR_DROP_HINT,
                    (player_x * BRD_UNIT_SIZE)    + BRD_PIECE_X_OFFSET,
                    (drop_hint_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);
        // Need to re-show the sprite, in case it got hidden when the player
        // came close to a stack of pieces, but then moved to be over a gap again
        player_hinting_drop_show(TRUE);
    } else {
        player_hinting_drop_show(FALSE);
    }
}


#define SPR_LONG_PET_HINT_OFFSET_X (BRD_PIECE_X_OFFSET + 3)
#define SPR_LONG_PET_HINT_OFFSET_Y (BRD_PIECE_Y_OFFSET + 3)


INT8 spr_long_pet_hint_num_x[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
INT8 spr_long_pet_hint_num_y[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 spr_long_pet_hint_num_1[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 spr_long_pet_hint_num_2[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 spr_long_pet_hint_num_attrib[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 spr_long_pet_hint_num_counter;
UINT8 spr_long_pet_hint_num_slot;
UINT8 spr_long_pet_hint_num_enabled;





void hinting_petlength_reset(void) {

    UINT8 c;

    // Make sure all the sprites are hidden
    // and the location cache is cleared

    // spr_long_pet_hint_num_enabled = FALSE;
    spr_long_pet_hint_num_enabled = TRUE;
    spr_long_pet_hint_num_counter = 0;
    spr_long_pet_hint_num_slot = 0;

    for (c = 0; c < SPR_LONG_PET_HINT_NUM_POOL_SIZE; c++) {

        // Hide sprite
        move_sprite(SPR_LONG_PET_HINT_NUM_1, 0,0);
        move_sprite(SPR_LONG_PET_HINT_NUM_2, 0,0);

        // Reset positions
        // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
        spr_long_pet_hint_num_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

        // Set sprite
        set_sprite_tile(SPR_LONG_PET_HINT_NUM_1, GP_CROSS);
        set_sprite_tile(SPR_LONG_PET_HINT_NUM_2, GP_CROSS);

    // TODO: fix palette (add special for this, or use pet pals OR fix font transparency
       // set_sprite_prop(SPR_LONG_PET_HINT_NUM_1, GP_PAL_SPECIAL);
       // set_sprite_prop(SPR_LONG_PET_HINT_NUM_2, GP_PAL_SPECIAL);

    }

}


void hinting_petlength_toggle_enabled(void) {

    spr_long_pet_hint_num_enabled ^= 1;

    // Hide hinting if not enabled
    if (spr_long_pet_hint_num_enabled == FALSE) {
        hinting_petlength_hide();
    }
}


// Adds a pet length hint sprite overlay
// for pets that almost meeting required Long Pet length
void hinting_petlength_add(INT8 board_x, INT8 board_y, UINT8 length, UINT8 piece) {

    // Cache board x,y and sprite digits
    spr_long_pet_hint_num_x[spr_long_pet_hint_num_slot] = board_x;
    spr_long_pet_hint_num_y[spr_long_pet_hint_num_slot] = board_y;
    spr_long_pet_hint_num_attrib[spr_long_pet_hint_num_slot] = ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT); // Palette

    if (length <= 9) {
        spr_long_pet_hint_num_1[spr_long_pet_hint_num_slot] = SPRITE_TILE_FONT_DIGITS_START + length;
        spr_long_pet_hint_num_2[spr_long_pet_hint_num_slot] = GP_EMPTY;
    } else if (length <= 99) {
        spr_long_pet_hint_num_1[spr_long_pet_hint_num_slot] = SPRITE_TILE_FONT_DIGITS_START + (length / 10);
        spr_long_pet_hint_num_2[spr_long_pet_hint_num_slot] = SPRITE_TILE_FONT_DIGITS_START + (length % 10);
    }

    // Update sprite counter to show current sprite
    spr_long_pet_hint_num_counter = spr_long_pet_hint_num_slot;

    hinting_petlength_show(HINT_LONG_PET_INCREMENT_NO); // TODO: change to a define NO_INCREMENT


    // // Find next open slot
    // while (spr_long_pet_hint_num_x[spr_long_pet_hint_num_slot] != HINT_PET_LENGTH_SLOT_EMPTY) {

        // Move to next slot
        spr_long_pet_hint_num_slot++;

        // If there is no slot open, overwrite the first
        // Wrap slot around is needed, overwriting previous old entries
        if (spr_long_pet_hint_num_slot >= SPR_LONG_PET_HINT_NUM_POOL_SIZE) {
            spr_long_pet_hint_num_slot = 0;
//                break;
        }
//    }

}



void hinting_petlength_hide(void) {

    // Hide sprite
    move_sprite(SPR_LONG_PET_HINT_NUM_1, 0,0);
    move_sprite(SPR_LONG_PET_HINT_NUM_2, 0,0);
}



// TODO Convert this to just show ALL numbers and not loop through them
void hinting_petlength_show(UINT8 do_increment) {

    if (spr_long_pet_hint_num_enabled) {

        if (do_increment) {

            do {
                spr_long_pet_hint_num_counter++;

                if (spr_long_pet_hint_num_counter >= SPR_LONG_PET_HINT_NUM_POOL_SIZE) {
                    spr_long_pet_hint_num_counter = 0;
                    break; // TODO: FIXME: breaking here instead of doing a full 1-pass loop is sub-optimal, should be SPR_LONG_PET_HINT_NUM_POOL_SIZE loops MAX
                }
            } while (spr_long_pet_hint_num_x[spr_long_pet_hint_num_counter] == HINT_PET_LENGTH_SLOT_EMPTY);
        }

        // Show the entry if it's populated
        if (spr_long_pet_hint_num_x[spr_long_pet_hint_num_counter] != HINT_PET_LENGTH_SLOT_EMPTY) {
            // Set sprite tiles to show cached digits
            set_sprite_tile(SPR_LONG_PET_HINT_NUM_1, spr_long_pet_hint_num_1[spr_long_pet_hint_num_counter]);
            set_sprite_tile(SPR_LONG_PET_HINT_NUM_2, spr_long_pet_hint_num_2[spr_long_pet_hint_num_counter]);

            set_sprite_prop(SPR_LONG_PET_HINT_NUM_1, spr_long_pet_hint_num_attrib[spr_long_pet_hint_num_counter]);
            set_sprite_prop(SPR_LONG_PET_HINT_NUM_2, spr_long_pet_hint_num_attrib[spr_long_pet_hint_num_counter]);

            // Move the sprite into view at board position
            move_sprite(SPR_LONG_PET_HINT_NUM_1,
                        (spr_long_pet_hint_num_x[spr_long_pet_hint_num_counter] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X,
                        (spr_long_pet_hint_num_y[spr_long_pet_hint_num_counter] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);
            // Move the sprite into view at board position
            move_sprite(SPR_LONG_PET_HINT_NUM_2,
                        (spr_long_pet_hint_num_x[spr_long_pet_hint_num_counter] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X + 8,
                        (spr_long_pet_hint_num_y[spr_long_pet_hint_num_counter] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);
        }
        // Otherwise just leave the existing sprite
        // else {
        //     // Otherwise hide it
        //     hinting_petlength_hide();
        // }
    }
}


void hinting_petlength_remove(INT8 board_x, INT8 board_y) {

    UINT8 c;

//    if (spr_long_pet_hint_num_enabled) {
    // Only search within max number of added hints
    for (c = 0; c <= SPR_LONG_PET_HINT_NUM_POOL_SIZE; c++) {

        if ((board_x == spr_long_pet_hint_num_x[c]) &&
            (board_y == spr_long_pet_hint_num_y[c])) {

            // Clear location cache entry
            // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
            spr_long_pet_hint_num_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

            // Hide sprite by moving off screen if currently selected
            if ((c == spr_long_pet_hint_num_counter) &&
                (spr_long_pet_hint_num_enabled)) {
                hinting_petlength_hide();
            }

            // stop search
            return;
        }
    }
//    }
}


