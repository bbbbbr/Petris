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


INT8 spr_long_pet_hint_x[SPR_LONG_PET_HINT_COUNT_MAX];
INT8 spr_long_pet_hint_y[SPR_LONG_PET_HINT_COUNT_MAX];
UINT8 spr_long_pet_hint_max;


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



#define HINT_PET_LENGTH_SLOT_EMPTY -1


// ISSUES:
//
// x * BUG: hinting doesn't get removed when pet shortened by Bomb
// x * BUG: hinting doesn't get removed when pet removed by Merge
// x * BUG: hint gets placed at 1 location PAST end of pet when piece landed was a head/tail
// x * BUG: doesn't add hint if adding piece in middle of pet to make it > length
//   *   BUG: hinting should get re-calculated when any segment removed by bomb, even if it creates 2 separate pets
//
// * ISSUE: hinting doesn't help for split-pets
//
//

void player_hinting_pet_length_reset(void) {

    UINT8 c;

    // Make sure all the sprites are hidden
    // and the location cache is cleared

    spr_long_pet_hint_max = 0;

    for (c = 0; c < SPR_LONG_PET_HINT_COUNT_MAX; c++) {

        // Hide sprite
        move_sprite(SPR_LONG_PET_HINT_START + c, 0,0);

        spr_long_pet_hint_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;
        spr_long_pet_hint_y[c] = HINT_PET_LENGTH_SLOT_EMPTY;

        // Set sprite tile and pal
        set_sprite_tile(SPR_LONG_PET_HINT_START + c, GP_CROSS);
        set_sprite_prop(SPR_LONG_PET_HINT_START + c, GP_PAL_CROSS);
    }

}



// Adds a pet length hint sprite overlay
// for pets that almost meeting required Long Pet length
void player_hinting_pet_length_add(INT8 board_x, INT8 board_y) {

    UINT8 c;

    c = 0;
    // Search for first open slot
    while (spr_long_pet_hint_x[c] != HINT_PET_LENGTH_SLOT_EMPTY) {
        c++;

        // Abort if no slots are open
        if (c >= SPR_LONG_PET_HINT_COUNT_MAX) {
            return;
        }
    }

    if (c > spr_long_pet_hint_max) {
        spr_long_pet_hint_max = c;
    }
    // set_sprite_tile(SPR_LONG_PET_HINT_START + spr_long_pet_hint_count, GP_CROSS);
    // set_sprite_prop(SPR_LONG_PET_HINT_START + spr_long_pet_hint_count, GP_PAL_CROSS);

    // Move the sprite into view at board position
    move_sprite(SPR_LONG_PET_HINT_START + c,
                (board_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                (board_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);

    // Cache it's x/y location for
    // later removal testing and show/hide on pause
    spr_long_pet_hint_x[c] = board_x;
    spr_long_pet_hint_y[c] = board_y;
}



// Note: This just hides sprites but doesn't prune the list.
//       So if enough are removed and added again then it will run out
//       even if only 1 or 2 were on active display
void player_hinting_pet_length_remove(INT8 board_x, INT8 board_y)
 {

    UINT8 c;

    // Only search within max number of added hints
    // Ignore sdcc bug: warning 94: comparison is always false due to limited range of data type
    // TODO: FIXME: SDCC generating a warning here?
    for (c = 0; c <= spr_long_pet_hint_max; c++) {

        if ((board_x == spr_long_pet_hint_x[c]) &&
            (board_y == spr_long_pet_hint_y[c])) {

            // Clear location cache entry
            spr_long_pet_hint_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;
            spr_long_pet_hint_y[c] = HINT_PET_LENGTH_SLOT_EMPTY;

            // Hide sprite by moving off screen
            move_sprite(SPR_LONG_PET_HINT_START + c, 0,0);

            // set_sprite_tile(SPR_LONG_PET_HINT_START + c, GP_EMPTY);

            // stop search
            return;
        }
    }
}



// Hides all sprites and resets location cache counter
void player_hinting_pet_length_all_show(UINT8 do_show) {

    UINT8 c;
    for (c = 0; c < SPR_LONG_PET_HINT_COUNT_MAX; c++) {

        if ((do_show) && (spr_long_pet_hint_x[c] != HINT_PET_LENGTH_SLOT_EMPTY)) {

                move_sprite(SPR_LONG_PET_HINT_START + c,
                            (spr_long_pet_hint_x[c] * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                            (spr_long_pet_hint_y[c] * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);

            //set_sprite_tile(SPR_LONG_PET_HINT_START + c, GP_CROSS);
            //set_sprite_prop(SPR_LONG_PET_HINT_START + c, GP_PAL_CROSS);

        } else {
            // Hide sprite by moving off screen
            move_sprite(SPR_LONG_PET_HINT_START + c, 0,0);

            // set_sprite_tile(SPR_LONG_PET_HINT_START + c, GP_EMPTY);
        }
    }
}





/*



#define BRD_PET_LENGTH_POPUP_OFFSET_X 10
#define BRD_PET_LENGTH_POPUP_OFFSET_Y 10

void pet_length_popup_show(int8 board_x, int8 board_y, UINT8 tile_count) {

    UINT8 c;

    for (c = 0; c < 3; c++) {
        move_sprite(SPR_PET_LEN_POPUP_DIGIT[c],
                    (board_x * BRD_UNIT_SIZE) + BRD_PET_LENGTH_POPUP_OFFSET_X,
                    (board_y * BRD_UNIT_SIZE) + BRD_PET_LENGTH_POPUP_OFFSET_Y);
        set_sprite_tile(SPR_PET_LEN_POPUP_DIGIT[c], pet_length_popup_digits[c] + TILES_FONT_NUMS_START);
    }
}
*/