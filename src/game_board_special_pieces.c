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

    // Clear adjacent 4 tiles (no piece clearing player credit from this)
    c = 0;
    // This isn't a for-loop because the compiler had some transient problem with that
    while (c < ARRAY_LEN(BRD_BOMB_HINT_OFFSET_LUT_X)) {

        // Retrieve piece at adjacent location to bomb
        tx = x + BRD_BOMB_HINT_OFFSET_LUT_X[c];
        ty = y + BRD_BOMB_HINT_OFFSET_LUT_Y[c];
        board_piece_get_xy(tx, ty, &piece, &connect);

        // Clear any pet-length hinting from it since
        // removal of piece would alter length
        board_check_completed_pet_xy(tx, ty, piece, connect,
                                     BRD_CHECK_FLAGS_DONT_ADD_POINTS
                                     | BRD_CHECK_FLAGS_DONT_HANDLE_PET_COMPLETED);

        // Remove the piece
        PLAY_SOUND_TILE_CLEAR_BOMB;
        board_clear_tile_xy(tx, ty);

        c++;
    }
}