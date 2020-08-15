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

// game_piece.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "common.h"

#include "options.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "player_info.h"
#include "player_gfx.h"


UINT8 game_piece_next;
UINT8 game_piece_next_stash;


void  game_piece_next_reset(void) {
    game_piece_next_stash = GAME_PIECE_STASH_NONE;
}


void game_piece_next_generate(void) {

    // Retrieve stashed next game piece if present
    if (game_piece_next_stash != GAME_PIECE_STASH_NONE) {

        game_piece_next = game_piece_next_stash;
        // Clear stashed next game piece
        game_piece_next_stash = GAME_PIECE_STASH_NONE;

    } else {
        // Otherwise generate a new piece

        // OPTIONAL: IMPROVE NEW PIECE SELECTION
        // For now, choose single random pet tile
        // Use rand() ^ DIV_REG to add more variety to the random number sequence
        // game_piece_next = ((UINT8)DIV_REG & 0x1F);
        game_piece_next = ((UINT8)(rand() ^ DIV_REG) & 0x1F);

        // game_piece_next = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
        //                    (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
        //                     GP_ROT_HORZ << GP_ROT_UPSHIFT);// + TILES_PET_START;

        // If in Tail Cleanup mode then
        // suppress tail pieces to make it less frustrating
        if ((option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) &&
            ((game_piece_next & GP_SEG_MASK) == GP_SEG_TAIL_BITS)) {

            // Translate Tail pieces to Head pieces
            game_piece_next = (game_piece_next & ~GP_SEG_MASK) | GP_SEG_HEAD_BITS;

        // If in Crunch-up mode then
        // suppress *some* L-Turn pieces to make it less frustrating
        } else if ((option_game_type == OPTION_GAME_TYPE_CRUNCH_UP) &&
            ((game_piece_next & GP_SEG_MASK) == GP_SEG_TURN_BITS)) {

            // Generate a new piece. Leaving about a 1 in 4 chance
            // that it's a L-Turn pieces
//            game_piece_next = ((UINT8)(rand() ^ DIV_REG) & 0x1F);

            // Translate L-Turn pieces to Torso
            game_piece_next = (game_piece_next & ~GP_SEG_MASK) | GP_SEG_TORSO_BITS;

        }
    }
}



UINT8 game_piece_next_get(void) {
    return game_piece_next;
}



void game_piece_next_set(UINT8 override_piece) {

    // Store current next piece
    game_piece_next_stash = game_piece_next;

    // Override the next piece
    game_piece_next = override_piece;

    // Update the next preview display
    // TODO: It looks kind of strange when the override shows up
    //       for only a second in the preview and then pops up top
    // game_piece_next_show(TRUE);
}



void game_piece_next_show(UINT8 do_show) {

    UINT8 attrib;
    // TODO: this could get animated to launch the next piece
    //       to the top of the board

    // TODO: OPTIMIZE: ?? would be less overhead to test option_game_preview_next before calling function?
    if ((do_show) && (option_game_preview_next == NEXT_PREV_ON)) {

        // TODO: consolidate sprite calculation with player_piece_update_gfx() where this is duplicated from?

        // Update player rotation (clear rotate bits and then set)
        set_sprite_tile(SPR_PLAYER_NEXT,
                        (game_piece_next & ~GP_ROT_MASK)
                        | GP_ROT_LUT_TILE[GP_ROTATE_DEFAULT]);

        // Set palette based on pet type (CGB Pal bits are 0x07)
        // And mirror bits based on rotation setting from LUT
        if (game_piece_next & GP_SPECIAL_MASK) {
            // Special sprites have one palette and no rotation/etc
            attrib = GP_PAL_SPECIAL;

        } else {
            attrib = ((game_piece_next & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                      | GP_ROT_LUT_ATTR[GP_ROTATE_DEFAULT];               // Rotation sprite mirror bits
        }

        set_sprite_prop(SPR_PLAYER_NEXT, attrib);

        // Make sure the sprite is visible (this could probs be optimized out with better planning / logic)
        move_sprite(SPR_PLAYER_NEXT,
                    GAME_PIECE_NEXT_PREVIEW_X,
                    GAME_PIECE_NEXT_PREVIEW_Y);
    } else {
        // Hide sprite
        move_sprite(SPR_PLAYER_NEXT, 0,0);
    }
}
