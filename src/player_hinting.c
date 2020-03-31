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
extern UINT8 player_y;
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


// Apply sprite flicker to special piece hinting and drop hinting if needed
// TODO: REORGANIZE: move to a different source file
void player_hinting_flicker_update(UINT8 frame_count) {

    if ((frame_count & 0x0F) == 0x00)
        player_hinting_special_show(TRUE);
    else if ((frame_count & 0x0F) == 0x08)
        player_hinting_special_show(FALSE);
}


// TODO: REORGANIZE: move to a different source file
void player_hinting_special_show(UINT8 do_show) {

    if ((do_show) && (player_piece & GP_SPECIAL_MASK)) {

        // Update sprite to current coordinates
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

        player_hinting_special_move();
}
