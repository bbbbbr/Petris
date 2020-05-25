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

INT8  hinting_petlength_x[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
INT8  hinting_petlength_y[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 hinting_petlength_num_1[SPR_LONG_PET_HINT_NUM_POOL_SIZE];
UINT8 hinting_petlength_num_2[SPR_LONG_PET_HINT_NUM_POOL_SIZE];

UINT8 hinting_petlength_enabled;
UINT8 hinting_petlength_slot;
UINT8 hinting_petlength_last_removed;


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



void hinting_petlength_reset(void) {

    UINT8 c;
    UINT8 sprite_idx;

    // Make sure all the sprites are hidden
    // and the location cache is cleared

    hinting_petlength_last_removed = HINT_PET_LENGTH_SLOT_NONE;
    hinting_petlength_enabled = FALSE;
    hinting_petlength_slot = 0;

    sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    for (c = 0; c < SPR_LONG_PET_HINT_NUM_POOL_SIZE; c++) {

        // Reset positions
        // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
        hinting_petlength_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

        // Move sprites off-screen
        move_sprite(sprite_idx,     0,0);
        move_sprite(sprite_idx + 1, 0,0);

        // Set sprites to non-visible (transparent tile)
        set_sprite_tile(sprite_idx, GP_EMPTY);
        set_sprite_tile(sprite_idx + 1, GP_EMPTY);

        // Move to next sprite
        sprite_idx += 2;
    }

}


// TODO optimize this out
void hinting_petlength_toggle_enabled(void) {

    hinting_petlength_enabled ^= 1;

    // Update display
    hinting_petlength_show();
}


// Adds a pet length hint sprite overlay
// for pets that almost meeting required Long Pet length
void hinting_petlength_add(INT8 board_x, INT8 board_y, UINT8 length, UINT8 piece) {

    UINT8 slot;
    UINT8 sprite_idx;

    // Try to use the last freed slot,
    // otherwise continue with rotating through slots sequentially
    if (hinting_petlength_last_removed != HINT_PET_LENGTH_SLOT_NONE) {

        slot = hinting_petlength_last_removed;
        hinting_petlength_last_removed = HINT_PET_LENGTH_SLOT_NONE;
    }
    else {
        slot = hinting_petlength_slot;

        // Move to next slot
        hinting_petlength_slot++;
        // Wrap slot around if needed, overwriting previous old entries
        if (hinting_petlength_slot >= SPR_LONG_PET_HINT_NUM_POOL_SIZE) {
            hinting_petlength_slot = 0;
        }
    }

    sprite_idx = SPR_LONG_PET_HINT_NUM_START + (slot * 2);

    // Cache board x,y and sprite digits
    hinting_petlength_x[slot] = board_x;
    hinting_petlength_y[slot] = board_y;

    // Set sprite palettes
    set_sprite_prop(sprite_idx   , ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT));
    set_sprite_prop(sprite_idx +1, ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT));

    // Calculate numeric sprites and save for later display
    if (length <= 9) {
        hinting_petlength_num_1[slot] = SPRITE_TILE_FONT_DIGITS_START + length;
        hinting_petlength_num_2[slot] = GP_EMPTY;
    } else if (length <= 99) {
        hinting_petlength_num_1[slot] = SPRITE_TILE_FONT_DIGITS_START + (length / 10);
        hinting_petlength_num_2[slot] = SPRITE_TILE_FONT_DIGITS_START + (length % 10);
    }

    // Move sprites to board position
    move_sprite(sprite_idx,
                (board_x * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X,
                (board_y * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);
    move_sprite(sprite_idx + 1,
                (board_x * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X + 8,
                (board_y * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);


    // Render sprite visible if enabled (via setting tile)
    if (hinting_petlength_enabled) {
        // hinting_petlength_show(); // TODO: change to a define NO_INCREMENT
        set_sprite_tile(sprite_idx    , hinting_petlength_num_1[slot]);
        set_sprite_tile(sprite_idx + 1, hinting_petlength_num_2[slot]);
    }

}



void hinting_petlength_hide(void) {

    hinting_petlength_enabled = FALSE;

    // Update display
    hinting_petlength_show();
}



// TODO Convert this to just show ALL numbers and not loop through them
void hinting_petlength_show(void) {

    UINT8 c;
    UINT8 sprite_idx; // TODO use a pointer instead?

    // Set initial offset for hint sprites
    sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    for (c = 0; c < SPR_LONG_PET_HINT_NUM_POOL_SIZE; c++) {

        // Show the entry if it's populated
        if ((hinting_petlength_enabled)
            && (hinting_petlength_x[c] != HINT_PET_LENGTH_SLOT_EMPTY))
        {

            // Reveal sprite
            set_sprite_tile(sprite_idx++, hinting_petlength_num_1[c]);
            set_sprite_tile(sprite_idx++, hinting_petlength_num_2[c]);

            // // Move the sprite into view at board position
            // move_sprite(sprite_idx++,
            //             (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X,
            //             (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);

            // // Move the sprite into view at board position
            // move_sprite(sprite_idx++,
            //             (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X + 8,
            //             (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);
        } else {
            // Otherwise entry is not visible

            // Hide the entry
            // move_sprite(sprite_idx++, 0,0);
            // move_sprite(sprite_idx++, 0,0);

            set_sprite_tile(sprite_idx++, GP_EMPTY);
            set_sprite_tile(sprite_idx++, GP_EMPTY);
        }
    }
}


void hinting_petlength_remove(INT8 board_x, INT8 board_y) {

    UINT8 c;
    UINT8 sprite_idx;

    // Only search within max number of added hints
    for (c = 0; c <= SPR_LONG_PET_HINT_NUM_POOL_SIZE; c++) {

        if ((board_x == hinting_petlength_x[c]) &&
            (board_y == hinting_petlength_y[c])) {

            // Clear location cache entry
            // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
            hinting_petlength_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

            // Hide sprite
            sprite_idx = SPR_LONG_PET_HINT_NUM_START + (c * 2);
            // move_sprite(sprite_idx    , 0,0);
            // move_sprite(sprite_idx + 1, 0,0);
            set_sprite_tile(sprite_idx    , GP_EMPTY);
            set_sprite_tile(sprite_idx + 1, GP_EMPTY);

            // Flag this slot as last removed for use on next add()
            // to avoid needlessly overwriting other entries.
            // Esp since if pet length increased this remove()
            // will be followed by an add()
            hinting_petlength_last_removed = c;

            // stop search
            return;
        }
    }
}

