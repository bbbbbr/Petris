// player_piece.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "common.h"
#include "input.h"
#include "sound.h"
#include "gfx_print.h"

#include "player_info.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "gameplay.h"

#include "player_piece.h"


 INT8 player_x;
UINT8 player_y;
 INT8 player_rotate; // Uses wraparound, so allow negative nums
UINT8 player_piece;
UINT8 player_attrib;


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



void player_piece_reset(void) {
    // Locate new piece at default position
    // Top of board, in the middle, no rotation
    player_x      = BRD_NEWPIECE_X;
    player_y      = BRD_NEWPIECE_Y;

    player_attrib = 0; // zero out here, updated below in player_piece_update_gfx()
    player_rotate = GP_ROTATE_DEFAULT;

    // Get a new piece
    player_piece = game_piece_next_get();

    // Generate the next upcoming piece and display it
    game_piece_next_generate();

    // Show a preview of the next piece, if applicable
    game_piece_next_show(TRUE);

    player_piece_update_gfx();
    player_piece_update_xy(PLAYER_PIECE_SHOW);
}



// UINT8 player_piece_connect_get(UINT8 piece, UINT8 rotate) {
UINT8 player_piece_connect_get() {

    UINT8 connect;

    // Get connection info for piece
    connect = GP_CONNECT_LUT[ (player_piece & GP_SEG_MASK) ];
    // Handle rotation (rotate bits up, then handle nybble wraparound)
    connect = connect << player_rotate;
    connect = (connect | (connect >> GP_CONNECT_WRAP_DOWNSHIFT)) & GP_CONNECT_MASK;

    return (connect);
}


// TODO: optimize out this function unless it starts doing more
void player_piece_set_on_board(void) {

    board_set_tile_xy(player_x, player_y,
                      player_piece,
                      player_attrib,
                      player_piece_connect_get());
}



void player_piece_rotate_apply(UINT8 dir) {

    // Rotate sound
    //PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x24, 0xC6, 0x86);
    //PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x24, 0xA4, 0x86);
      PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x14, 0x94, 0x86);
    // TODO: For larger pieces test whether they can turn
    if (dir == PLAYER_ROT_LEFT)
        player_rotate--;
    else if (dir == PLAYER_ROT_RIGHT)
        player_rotate++;

    // Handle wraparound
    if (player_rotate < GP_ROTATE_MIN)
        player_rotate = GP_ROTATE_MAX;
    else if (player_rotate > GP_ROTATE_MAX)
        player_rotate = GP_ROTATE_MIN;

    // TODO: only need to update GFX if rotation succeeded
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

    // TODO: optimize?
    if ((new_x >= BRD_MIN_X) &&
        (new_x <= BRD_MAX_X) &&
        (new_y <= BRD_MAX_Y) &&
         board_check_open_xy(new_x, new_y)) {

        // TODO: TICK sound??
        // PlayFx(CHANNEL_1, 30, 0x00, 0xC0, 0x31, 0x8F, 0x85);
        // PlayFx(CHANNEL_1, 30, 0x30, 0x81, 0x33, 0x37, 0x87);
        // PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x22, 0xD6, 0x86);
        // PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x12, 0xD6, 0x86);
        // PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x12, 0xA6, 0x86);

        player_x = new_x;
        player_y = new_y;

        player_piece_update_xy(PLAYER_PIECE_SHOW);

        return (MOVE_OK);
    }

    // If the move failed and the piece
    if ((player_y == BRD_NEWPIECE_Y) && (dir_y == 1))
        return (MOVE_BLOCKED_GAME_OVER);
    else
        return (MOVE_BLOCKED);
}


// TODO: move to player_piece.c
// TODO: change up allow preview update : ??
//       -> Then how to handle that it updates the local global vars for player_piece and player_attrib?
// void player_piece_update_gfx(UINT8 sprite_id, UINT8 t_player_piece, UINT8 t_player_rotate) {
void player_piece_update_gfx() {

    if (player_piece & GP_SPECIAL_MASK) {
        // player_piece = player_piece;
        player_attrib = GP_PAL_DOG; // TODO : Assign a "special" pal?

    } else {
        // Update player rotation (clear rotate bits and then set)
        player_piece = ((player_piece & ~GP_ROT_MASK)
                        | GP_ROT_LUT_TILE[player_rotate]);

        // Set palette based on pet type (CGB Pal bits are 0x07)
        // And mirror bits based on rotation setting from LUT
        player_attrib = ((player_piece & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                         | GP_ROT_LUT_ATTR[player_rotate];               // Rotation sprite mirror bits

        // TODO: This could be simplified by using a full set of pieces instead of 1/2 set and mirroring them
        // L Turn pieces require mirror X and Y
        // when rotation is 180 and 270
        if (((player_piece & GP_SEG_MASK) ==  GP_SEG_TURN_BITS) &&
             (player_rotate & GP_ROTATE_SEG_TURN_MIRROR_BITS))
            player_attrib |= (GP_MIRROR_X | GP_MIRROR_Y);
    }

    // set_sprite_tile(SPR_PLAYER, (player_piece & GP_PET_BITS_MASK));
    set_sprite_tile(SPR_PLAYER, player_piece);
    set_sprite_prop(SPR_PLAYER, player_attrib);
}
