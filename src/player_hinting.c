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

#include "options.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_types.h"

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

UINT8 hint_special_tile = GP_EMPTY;

INT8  hinting_petlength_x[SPR_LONG_PET_HINT_POOL_SIZE];
INT8  hinting_petlength_y[SPR_LONG_PET_HINT_POOL_SIZE];
UINT8 hinting_petlength_num_1[SPR_LONG_PET_HINT_POOL_SIZE];
UINT8 hinting_petlength_num_2[SPR_LONG_PET_HINT_POOL_SIZE];
UINT8 hinting_petlength_size[SPR_LONG_PET_HINT_POOL_SIZE];

UINT8 hinting_petlength_enabled = FALSE;
UINT8 hinting_petlength_slot = 0;
UINT8 hinting_petlength_last_removed = HINT_PET_LENGTH_SLOT_NONE;


// Apply sprite flicker to special piece hinting and drop hinting if needed
void player_hinting_flicker_update() {

    if ((sys_time & 0x0F) == 0x00)
        player_hinting_special_show(TRUE);
    else if ((sys_time & 0x0F) == 0x08)
        player_hinting_special_show(FALSE);
}



// Hide special piece hinting sprites by moving them off-screen
void player_hinting_special_reset(void) {

        INT8 c;

        for (c=SPR_SPECIAL_HINT_1; c <= SPR_SPECIAL_HINT_4; c++) {
            move_sprite(c, 0, 0);
        }
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
        // Hide using a clear tile, maybe it's a little faster
        set_sprite_tile(SPR_SPECIAL_HINT_1, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_2, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_3, GP_EMPTY);
        set_sprite_tile(SPR_SPECIAL_HINT_4, GP_EMPTY);
    }
}


// Move the hinting sprites that surround the player (Special) piece
// NOTE: expects to only be called if (player_piece & GP_SPECIAL_MASK)
void player_hinting_special_move(void) {

        INT8 spr_x, spr_y;

        spr_x = (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET;
        spr_y = (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET;

        move_sprite(SPR_SPECIAL_HINT_1, spr_x + SPR_HINT_OFFSET_LUT_X[0],
                                        spr_y + SPR_HINT_OFFSET_LUT_Y[0]);

        move_sprite(SPR_SPECIAL_HINT_2, spr_x + SPR_HINT_OFFSET_LUT_X[1],
                                        spr_y + SPR_HINT_OFFSET_LUT_Y[1]);

        move_sprite(SPR_SPECIAL_HINT_3, spr_x + SPR_HINT_OFFSET_LUT_X[2],
                                        spr_y + SPR_HINT_OFFSET_LUT_Y[2]);

        move_sprite(SPR_SPECIAL_HINT_4, spr_x + SPR_HINT_OFFSET_LUT_X[3],
                                        spr_y + SPR_HINT_OFFSET_LUT_Y[3]);
}


// NOTE: expects to only be called if (player_piece & GP_SPECIAL_MASK)
void player_hinting_special_update_gfx() {

        // Load the corresponding special tile (for 4-direction preview)
        // The LUT is zero based, so get an offset from the start of the special tiles
        hint_special_tile = GP_SPECIAL_HINT_LUT[ player_piece - GP_SPECIAL_START ];

        // Setting the sprite is now redundant since
        // player_hinting_special_show() is called immediately
        // after the only time this is called
        // set_sprite_tile(SPR_SPECIAL_HINT_1, hint_special_tile);
        // set_sprite_tile(SPR_SPECIAL_HINT_2, hint_special_tile);
        // set_sprite_tile(SPR_SPECIAL_HINT_3, hint_special_tile);
        // set_sprite_tile(SPR_SPECIAL_HINT_4, hint_special_tile);

        // Set Pal and attribs
        set_sprite_prop(SPR_SPECIAL_HINT_1, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_2, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_3, GP_PAL_SPECIAL);
        set_sprite_prop(SPR_SPECIAL_HINT_4, GP_PAL_SPECIAL);
}



// Hide player drop hint sprite by moving it off-screen
void player_hinting_drop_reset(void) {
    move_sprite(SPR_DROP_HINT, 0, 0);
}



void player_hinting_drop_show(UINT8 do_show) {

    if (do_show) {

        // Update sprite to use visible tile
        set_sprite_tile(SPR_DROP_HINT, GP_SPECIAL_DROP_HINT);
        set_sprite_prop(SPR_DROP_HINT, GP_PAL_DROPHINT);

    } else {
        // Hide sprite
        // move_sprite(SPR_DROP_HINT, 0,0);
        // Hide using a clear tile, maybe it's a little faster
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


// ==== PET LENGTH HINTING ====
//
// Sprite IDs
// sprite_idx + 0  = Left Length Digit
// sprite_idx + 1  = Right Length Digit
// sprite_idx + 2  = Cross / Penultiamte length marer
//


void hinting_petlength_reset(void) {

    UINT8 c;
    UINT8 sprite_idx;

    // Make sure all the sprites are hidden
    // and the location cache is cleared

    hinting_petlength_last_removed = HINT_PET_LENGTH_SLOT_NONE;
    hinting_petlength_enabled = FALSE;
    hinting_petlength_slot = 0;

    sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    for (c = 0; c < SPR_LONG_PET_HINT_POOL_SIZE; c++) {

        // Reset positions
        // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
        hinting_petlength_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

        // Move sprites off-screen
        // and set sprites to non-visible (transparent tile)

        // First Digit
        move_sprite(sprite_idx,     0,0);
        set_sprite_tile(sprite_idx, GP_EMPTY);
        sprite_idx++;

        // Second Digit
        move_sprite(sprite_idx, 0,0);
        set_sprite_tile(sprite_idx, GP_EMPTY);
        sprite_idx++;


        // Hide and set tile for size hint sprite
        move_sprite(sprite_idx, 0,0);
        // These two could probably just be called once at the
        // start of a game instead of every level
        set_sprite_tile(sprite_idx, GP_CROSS);
        set_sprite_prop(sprite_idx, GP_PAL_CROSS);
        sprite_idx++;

        // Move to next sprite
        // sprite_idx += SPR_LONG_PET_HINT_NUM_TILES_PER;
    }

}


void hinting_petlength_turn_on(void) {

    hinting_petlength_enabled = HINT_PET_LENGTH_TIMEOUT;

    // Update display
    hinting_petlength_showhide();
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
        if (hinting_petlength_slot >= SPR_LONG_PET_HINT_POOL_SIZE) {
            hinting_petlength_slot = 0;
        }
    }

    sprite_idx = SPR_LONG_PET_HINT_NUM_START + (slot * SPR_LONG_PET_HINT_NUM_TILES_PER);

    // Cache board x,y and sprite digits
    hinting_petlength_x[slot] = board_x;
    hinting_petlength_y[slot] = board_y;

    // Set sprite palettes
    set_sprite_prop(sprite_idx    , ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT));
    set_sprite_prop(sprite_idx + 1, ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT));

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
        // hinting_petlength_showhide();
        set_sprite_tile(sprite_idx    , hinting_petlength_num_1[slot]);
        set_sprite_tile(sprite_idx + 1, hinting_petlength_num_2[slot]);
    }

    // If pet length is greater than required length -1 then
    // add a size hint, regardless of overlay status
    //
    // PET_LENGTH_PREVIEW_ENABLED_FOR_ALL_MODES
    if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
        if (length >= (game_type_long_pet_required_size - 1)) {
            move_sprite(sprite_idx + 2,
                        (board_x * BRD_UNIT_SIZE) + SPR_LONG_PET_SIZE_OFFSET_X,
                        (board_y * BRD_UNIT_SIZE) + SPR_LONG_PET_SIZE_OFFSET_Y);
        }
    }
}



// Used to also move sprites on/off screen to show/hide them
// but now only relies on transparent vs opaque tiles to do that
//
// Note: Using a separate counter for the sprite index instead of
//       multiplying (c * 3) was producing compiler problems (below).
//       The compiler also seems to produce shorter, faster code with the multiply...
//       SDCC:
//       * Warning: Non-connected liverange found and extended to connected component of the CFG:iTemp33. Please contact sdcc authors with source code to reproduce.
//       * Warning: Non-connected liverange found and extended to connected component of the CFG:iTemp0. Please contact sdcc authors with source code to reproduce.
void hinting_petlength_showhide(void) {

    UINT8 c;
    UINT8 sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    for (c = 0; c < SPR_LONG_PET_HINT_POOL_SIZE; c++) {

        // Show the entry if it's populated
        // If it's not populated ** OR ** overlay is disabled,
        // then the sprite for the entry will get hidden (below via else {})
        if ((hinting_petlength_enabled)
            && (hinting_petlength_x[c] != HINT_PET_LENGTH_SLOT_EMPTY))
        {

            // Reveal first two sprites and skip every third (cross/size marker sprite)
            set_sprite_tile(sprite_idx,     hinting_petlength_num_1[c]);
            set_sprite_tile(sprite_idx + 1, hinting_petlength_num_2[c]);

            // // Move the sprite into view at board position
            // move_sprite(sprite_idx,
            //             (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X,
            //             (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);

            // // Move the sprite into view at board position
            // move_sprite(sprite_idx + 1,
            //             (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X + 8,
            //             (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);
        } else {
            // Otherwise entry is not visible

            // Hide the entry
            // move_sprite(sprite_idx,    0,0);
            // move_sprite(sprite_idx + 1, 0,0);

            // Hide first two sprites and skip every third (cross/size marker sprite)
            set_sprite_tile(sprite_idx, GP_EMPTY);
            set_sprite_tile(sprite_idx + 1, GP_EMPTY);
        }

        // Update sprite index offset
        sprite_idx += SPR_LONG_PET_HINT_NUM_TILES_PER;
    }
}


void hinting_petlength_remove(INT8 board_x, INT8 board_y) {

    UINT8 c;
    UINT8 sprite_idx = SPR_LONG_PET_HINT_NUM_START;

// Only search within max number of added hints
    for (c = 0; c < SPR_LONG_PET_HINT_POOL_SIZE; c++) {

        if ((board_x == hinting_petlength_x[c]) &&
            (board_y == hinting_petlength_y[c])) {

            // Clear location cache entry
            // Just update X, since Y doesn't get tested for EMPTY X since that's what gets
            hinting_petlength_x[c] = HINT_PET_LENGTH_SLOT_EMPTY;

            // Hide sprite
            // move_sprite(sprite_idx    , 0,0);
            // move_sprite(sprite_idx + 1, 0,0);
            set_sprite_tile(sprite_idx    , GP_EMPTY);
            set_sprite_tile(sprite_idx + 1, GP_EMPTY);

            // Hide size hint cross sprite
            move_sprite(sprite_idx + 2, 0,0);

            // Flag this slot as last removed for use on next add()
            // to avoid needlessly overwriting other entries.
            // Esp since if pet length increased this remove()
            // will be followed by an add()
            hinting_petlength_last_removed = c;

            // stop search
            return;
        }
        // Update sprite index offset
        sprite_idx += SPR_LONG_PET_HINT_NUM_TILES_PER;
    }
}


// Scroll the pet length overlays upward
// (to compensate for board shifts upward via crunch-ups)
//
// Note: hinting_petlength_refreshxy() should
// eventually be called afterward to refresh sprite locations
void hinting_petlength_scrollup(void) {

    UINT8 c;
    UINT8 sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    // Only search within max number of added hints
    for (c = 0; c < SPR_LONG_PET_HINT_POOL_SIZE; c++) {

        if (hinting_petlength_x[c] != HINT_PET_LENGTH_SLOT_EMPTY) {
            // Don't have to worry about wraparound here since
            // negative Y values are unlikely and would just scroll
            // off-screen, which is fine
            hinting_petlength_y[c]--;

            // Scroll cross sprite up by -1 tile
            scroll_sprite(sprite_idx + 2 ,0, -BRD_UNIT_SIZE);
        }

        // Update sprite index offset
        sprite_idx += SPR_LONG_PET_HINT_NUM_TILES_PER;
    }
}


// Refresh the pet length overlay sprite locations
void hinting_petlength_refreshxy(void) {

    UINT8 c;
    UINT8 sprite_idx = SPR_LONG_PET_HINT_NUM_START;

    // Only search within max number of added hints
    for (c = 0; c < SPR_LONG_PET_HINT_POOL_SIZE; c++) {

        if (hinting_petlength_x[c] != HINT_PET_LENGTH_SLOT_EMPTY) {

            // Move the sprite into view at board position
            move_sprite(sprite_idx,
                        (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X,
                        (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);

            // Move the sprite into view at board position
            move_sprite(sprite_idx + 1,
                        (hinting_petlength_x[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_X + 8,
                        (hinting_petlength_y[c] * BRD_UNIT_SIZE) + SPR_LONG_PET_HINT_OFFSET_Y);

        }

        // Update sprite index offset
        sprite_idx += SPR_LONG_PET_HINT_NUM_TILES_PER;
    }
}