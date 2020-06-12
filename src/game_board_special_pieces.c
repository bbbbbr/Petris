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

// game_board_special_pieces.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "audio_common.h"
#include "common.h"

#include "game_board.h"
#include "game_board_special_pieces.h"
#include "game_piece_data.h"
#include "gfx.h"
#include "sound.h"
#include "player_info.h"
#include "player_hinting.h"

const INT8 BRD_BOMB_HINT_OFFSET_LUT_X[] = {-1,  0, 1, 0};
const INT8 BRD_BOMB_HINT_OFFSET_LUT_Y[] = { 0, -1, 0, 1};

void board_handle_special_bomb(INT8 x,INT8 y) {

    UINT8 piece, connect;
    UINT8 c;
    UINT8 tx, ty;

    // TODO: visual effects and sound effects on the tiles
    //       maybe expand: board_clear_tile_xy() to set arbitray pieces and use it to flash the area
    //
    // Clear adjacent 4 tiles

    // TODO: OPTIMIZE: make a loop?
    // TODO: move the effects into board_clear_tile_xy?

    // Remove hinting for any pets BOMB_HINT_OFFSET_LUT_X will get shortened by bomb
    c = 0;
    // TODO: FIXME: Compiler problems with the for loop: Multiple definition of C$game_board_special_pieces.c$47�G��.�gO��@
    // for (c = 0; c < ARRAY_LEN(BRD_BOMB_HINT_OFFSET_LUT_X); c++) {
    while (c < ARRAY_LEN(BRD_BOMB_HINT_OFFSET_LUT_X)) {

        tx = x + BRD_BOMB_HINT_OFFSET_LUT_X[c];
        ty = y + BRD_BOMB_HINT_OFFSET_LUT_Y[c];

        // Retrieve piece at adjacent location to bomb
        // and clear it of any special piece hinting
        board_piece_get_xy(tx, ty, &piece, &connect);
        board_check_completed_pet_xy(tx, ty, piece, connect,
                                     BRD_CHECK_FLAGS_DONT_ADD_POINTS
                                     | BRD_CHECK_FLAGS_DONT_HANDLE_PET_COMPLETED);

        // Remove the piece
        PLAY_SOUND_TILE_CLEAR_BOMB;
        board_clear_tile_xy(tx, ty);

        c++;
    }

    // PLAY_SOUND_TILE_CLEAR_BOMB;
    // board_clear_tile_xy(x - 1, y);

    // PLAY_SOUND_TILE_CLEAR_BOMB;
    // board_clear_tile_xy(x + 1, y);

    // PLAY_SOUND_TILE_CLEAR_BOMB;
    // board_clear_tile_xy(x, y - 1);

    // PLAY_SOUND_TILE_CLEAR_BOMB;
    // board_clear_tile_xy(x, y + 1);
}