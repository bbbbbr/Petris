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

// player_piece.c

#include <gbdk/platform.h>

#include "common.h"
#include "platform_compat.h"
#include "input.h"
#include "audio_common.h"
#include "sound.h"
#include "gfx_print.h"

#include "options.h"

#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"

#include "player_piece.h"
#include "player_hinting.h"
#include "player_info.h"
#include "player_gfx.h"

#include "gameplay.h"

 INT8 player_x = BRD_NEWPIECE_X;
 INT8 player_y = BRD_NEWPIECE_Y;
 INT8 player_rotate = GP_ROTATE_DEFAULT; // Uses wraparound, so allow negative nums
UINT8 player_piece = 0;
UINT8 player_attrib = 0;




void player_piece_update_xy(UINT8 do_show) {

    if (do_show) {
        // Update sprite to current coordinates
        // Will reveal sprite if hidden at 0,0
        move_sprite(SPR_PLAYER,
                    (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);
    } else {
        // Hide sprite
        move_sprite(SPR_PLAYER, 0,0);
    }
}



UINT8 player_piece_try_reload(void) {

    // If the board already has a tile in the default load location then
    // that means game over. Signal failure
    if (!board_check_open_xy(BRD_NEWPIECE_X, BRD_NEWPIECE_Y)) {

        return (FALSE);
    }


    // Locate new piece at default position
    // Top of board, in the middle, no rotation
    player_x      = BRD_NEWPIECE_X;
    player_y      = BRD_NEWPIECE_Y;

    player_attrib = 0; // zero out here, updated below in player_piece_update_gfx()
    player_rotate = GP_ROTATE_DEFAULT;

    // Override attributes and force second palette
    #if defined(GAMEGEAR)
        player_attrib = GG_TILE_ATTR_MSB_PAL_1;
    #endif // GAMEGEAR

    // Get a new piece
    player_piece = game_piece_next_get();

    // Generate the next upcoming piece and display it
    game_piece_next_generate();

    // Show a preview of the next piece, if applicable
    game_piece_next_show(TRUE);

    player_piece_update_gfx();
    player_piece_update_xy(PLAYER_PIECE_SHOW);


    if (option_game_visual_hints == OPTION_VISUAL_HINTS_ON) {

        if (player_piece & GP_SPECIAL_MASK) {
            player_hinting_special_update_gfx();
            player_hinting_special_move();
            player_hinting_special_show(TRUE);
        } else
            player_hinting_special_show(FALSE);

        player_hinting_drop_show(TRUE);
        player_hinting_drop_update();
    }

    // Signal successful load of a new piece
    return (TRUE);
}



UINT8 player_piece_connect_get(UINT8 piece, UINT8 rotate) {

    UINT8 connect;

    // Get connection info for piece and downshift to use as a LUT index
    connect = GP_CONNECT_LUT[ (piece & GP_SEG_MASK) >> GP_SEG_UPSHIFT ];

    // Handle rotation (rotate bits up, then handle nybble wraparound)
    connect = connect << rotate;
    connect = (connect | (connect >> GP_CONNECT_WRAP_DOWNSHIFT)) & GP_CONNECT_MASK;

    return (connect);
}



void player_piece_set_on_board(void) {

    UINT8 player_connect;

    player_connect = player_piece_connect_get(player_piece, player_rotate);

    // Hide sprite based piece before setting on the board
    // (except for *specials*, which don't get placed).
    // This happens here so it doesn't obscure board animations
    // under the sprite based piece
    if (!(player_piece & GP_SPECIAL_MASK)) {
        player_piece_update_xy(PLAYER_PIECE_HIDE);

        // Now set the piece onto the board
        board_set_tile_xy(player_x, player_y,
                          player_piece,
                          player_attrib,
                          player_connect);
    }

    // Make sure special piece hinting is turned off once landed
    if (option_game_visual_hints == OPTION_VISUAL_HINTS_ON) {
        player_hinting_special_show(FALSE);
    }

    board_handle_new_piece(player_x, player_y,
                           player_piece,
                           player_connect);
 }


void player_piece_rotate_apply(UINT8 dir) {

    // Rotate sound
    PLAY_SOUND_PIECE_ROTATE;

    if (dir == PLAYER_ROT_LEFT) {
        if (player_rotate == GP_ROTATE_MIN) player_rotate = GP_ROTATE_MAX;
        else player_rotate--;
    } else if (dir == PLAYER_ROT_RIGHT) {
        if (player_rotate == GP_ROTATE_MAX) player_rotate = GP_ROTATE_MIN;
        else player_rotate++;
    }

    // // Handle wraparound
    // if (player_rotate < GP_ROTATE_MIN) // <-------- GAMEGEARTODO - PROBLEM?
    //     player_rotate = GP_ROTATE_MAX;
    // else if (player_rotate > GP_ROTATE_MAX)
    //     player_rotate = GP_ROTATE_MIN;

    player_piece_update_gfx();
}


// Tests if direction is available
// then updates sprite if it moved
//
// Returns MOVE_OK if piece was moved
//
UINT8 player_piece_move(INT8 dir_x, INT8 dir_y) {

    INT8 new_x = (player_x + dir_x);
    INT8 new_y = (player_y + dir_y);

    if ((new_x >= BRD_MIN_X) &&
        (new_x <= BRD_MAX_X) &&
        (new_y >= BRD_MIN_Y) &&
        (new_y <= BRD_MAX_Y) &&
         board_check_open_xy(new_x, new_y)) {

        // OPTIONAL: play movement tick sound
        // PLAY_SOUND_PIECE_MOVE;

        player_x = new_x;
        player_y = new_y;

        player_piece_update_xy(PLAYER_PIECE_SHOW);

        if (option_game_visual_hints == OPTION_VISUAL_HINTS_ON) {

            if (player_piece & GP_SPECIAL_MASK)
                player_hinting_special_move();

            player_hinting_drop_update();
    }

        return (MOVE_OK);
    }

    return (MOVE_BLOCKED);
}


void player_piece_update_gfx() {

    if (player_piece & GP_SPECIAL_MASK) {
        // player_piece = player_piece;
        player_attrib = GP_PAL_SPECIAL;

    } else {
        // Update player rotation to default (0 degrees) (clear rotate bits and then set)
        player_piece = (player_piece & ~GP_ROT_MASK) | (player_rotate << GP_ROT_UPSHIFT);

        // Set palette based on pet type (CGB Pal bits are 0x07)
        // And mirror bits based on rotation setting from LUT
        player_attrib = (player_piece & GP_PET_MASK) >> GP_PET_UPSHIFT; // Palette based on pet
    }

    // Override attributes and force second palette
    #if defined(GAMEGEAR)
        player_attrib = GG_TILE_ATTR_MSB_PAL_1;
    #endif // GAMEGEAR

    // set_sprite_tile(SPR_PLAYER, (player_piece & GP_PET_BITS_MASK));
    set_sprite_tile(SPR_PLAYER, player_piece);
    set_sprite_prop(SPR_PLAYER, player_attrib);
}


// Cycle through Pet types
void player_piece_cycle_pet_types(void) {

    if (!(player_piece & GP_SPECIAL_MASK)) {

        PLAY_SOUND_SQUEEK;

        // Extract, increment pet type bits only then re-apply them
        player_piece = (player_piece & ~GP_PET_MASK)
                       | ((((player_piece >> GP_PET_UPSHIFT) + 1) << GP_PET_UPSHIFT) & GP_PET_MASK);

        player_piece_update_gfx();
    }
}
