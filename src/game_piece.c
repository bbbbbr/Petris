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

#include <gbdk/platform.h>
#include <rand.h>

#include "common.h"
#include "platform_compat.h"

#ifdef NINTENDO
    #include "serial_link.h"
#endif

#include "options.h"
#include "gfx.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "player_info.h"
#include "player_gfx.h"


UINT8 game_piece_next = 0;
UINT8 game_piece_next_stash = GAME_PIECE_STASH_NONE;


void  game_piece_next_reset(void) {
    game_piece_next_stash = GAME_PIECE_STASH_NONE;
}


// Generates a new game piece, either randomly or from enqueued.
//
// Game pieces are a collection of bit-packed
// values which map to pet/segment/rotation attributes.
//
// For example:
// game_piece_next = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
//                    (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
//                     GP_ROTATE_0 << GP_ROT_UPSHIFT);// + TILES_PET_START;
//
// OPTIONAL: IMPROVE NEW PIECE SELECTION
void game_piece_next_generate(void) {

    // Retrieve stashed next game piece if present
    if (game_piece_next_stash != GAME_PIECE_STASH_NONE) {

        game_piece_next = game_piece_next_stash;
        // Clear stashed next game piece
        game_piece_next_stash = GAME_PIECE_STASH_NONE;

    } else {
        // Otherwise generate a single random new pet tile piece

        #ifdef NINTENDO
            // If connected by link then only use rand() so that the
            // games follow the same random number sequence
            if (link_status == LINK_STATUS_CONNECTED) {
                // 2-Player mode
                game_piece_next = ((UINT8)rand() & GP_PET_BITS_MASK);
            } else {
                // In 1-player mode mix in DIV_REG for a little more variety to the random number sequence
                game_piece_next = ((UINT8)(rand() ^ DIV_REG) & GP_PET_BITS_MASK);
            }
        #elif defined(GAMEGEAR)
            game_piece_next = ((UINT8)(rand() ^ DIV_REG) & GP_PET_BITS_MASK);
        #endif // GAMEGEAR


        // == New piece adjusments to improve player experience in various modes ==

        // A SPECIAL piece should NEVER make it in here, but just in
        // case, this will future proof it against unwitting changes.
        // (SPECIAL pieces would get mangled to invalid values)
        if (!(game_piece_next & GP_SPECIAL_MASK)) {

            // Tail Cleanup : suppress adding tail pieces to make it less frustrating
            if ((option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) &&
                ((game_piece_next & GP_SEG_MASK) == GP_SEG_TAIL_BITS)) {

                // Translate Tail pieces to Head pieces
                game_piece_next = (game_piece_next & ~GP_SEG_MASK) | GP_SEG_HEAD_BITS;

            // Crunch-up mode: suppress all L-Turn pieces to make it less frustrating
            } else if ((option_game_type == OPTION_GAME_TYPE_CRUNCH_UP) &&
                ((game_piece_next & GP_SEG_MASK) == GP_SEG_TURN_BITS)) {

                // Translate L-Turn pieces to Torso
                game_piece_next = (game_piece_next & ~GP_SEG_MASK) | GP_SEG_TORSO_BITS;
            }
        }
    }


    // Done generating the new piece.
    // Now count it and queue up a special piece if applicable
    // WARNING: This needs to be done AFTER and OUTSIDE of any
    //          of the game-type piece massaging that happens above

    // Increase the count of total pieces used in the game
    // (prevent wraparound)
    if (player_numpieces < PLAYER_NUMPIECES_MAX)
        player_numpieces++;

    // Check to see whether a special piece (merge) should be delivered
    if ((player_numpieces & p_game_settings->spec_merge_threshold_pieces) == p_game_settings->spec_merge_threshold_pieces)
        game_piece_next_set(GP_SPECIAL_LIGHTENING);
}



UINT8 game_piece_next_get(void) {
    return game_piece_next;
}



void game_piece_next_set(UINT8 override_piece) {

    // Store current next piece
    game_piece_next_stash = game_piece_next;

    // Override the next piece
    game_piece_next = override_piece;

    // Preview display is updated elsewhere (game_piece_next_show(TRUE)
}



void game_piece_next_show(UINT8 do_show) {

    UINT8 attrib;

    if ((do_show) && (option_game_preview_next == NEXT_PREV_ON)) {

        // Update player rotation to default (0 degrees) (clear rotate bits and then set)
        set_sprite_tile(SPR_PLAYER_NEXT, (game_piece_next & ~GP_ROT_MASK) | GP_ROTATE_DEFAULT_BITS);

        // Set palette based on pet type (CGB Pal bits are 0x07)
        // And mirror bits based on rotation setting from LUT
        if (game_piece_next & GP_SPECIAL_MASK) {
            // Special sprites have one palette and no rotation/etc
            attrib = GP_PAL_SPECIAL;

        } else {
            attrib = (game_piece_next & GP_PET_MASK) >> GP_PET_UPSHIFT; // Palette based on pet
        }

        // Override attributes and force second palette
        #if defined(GAMEGEAR)
            attrib = GG_TILE_ATTR_PAL_1;
        #endif // GAMEGEAR

        set_sprite_prop(SPR_PLAYER_NEXT, attrib);

        // Make sure the sprite is visible (this could probs be optimized out with better planning / logic)
        move_sprite(SPR_PLAYER_NEXT,
                    GAME_PIECE_NEXT_PREVIEW_X,
                    GAME_PIECE_NEXT_PREVIEW_Y);
    } else {
        // Hide sprite
        // TODO: GAMEGEAR- CHECK
        move_sprite(SPR_PLAYER_NEXT, 0, 0);
    }
}
