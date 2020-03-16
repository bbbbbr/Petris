// game_player.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "game_player.h"

#include "game_pieces.h"
#include "game_board.h"
#include "input.h"
#include "sound.h"

// struct player_info {
//     UINT8 x;
//     UINT8 y;
//     UINT8 piece;
//     UINT8 rotate;
// };


// TODO: ??? Rename all this "game_player" to "game_piece"???
// TODO: ??? Rename all this "player_x" to "piece_x" ETC ???

// TODO: ??fix these: move to game_play.c???
#define TICK_COUNT_RESET    0
#define TICK_COUNT_DEFAULT 30 // 15 // 60 frames per second default speed
UINT8 tick_frame_count;


#define KEY_REPEAT_START               0
#define KEY_REPEAT_DOWN_RELOAD         1
#define KEY_REPEAT_DOWN_THRESHOLD      (KEY_REPEAT_DOWN_RELOAD + 2)
#define KEY_REPEAT_LEFTRIGHT_RELOAD    8
#define KEY_REPEAT_LEFTRIGHT_THRESHOLD (KEY_REPEAT_LEFTRIGHT_RELOAD + 4) //6
UINT8 key_down_repeat_got_release = FALSE;


#define SPR_PLAYER 0 // Player is sprite "0"

UINT8 piece_state;

 INT8 player_x;
UINT8 player_y;
 INT8 player_rotate; // Uses wraparound, so allow negative nums
UINT8 player_piece;
UINT8 player_attrib;


// TODO: Sprite rotation (use LUT or DEFINES for rotation/mirroring bits)
void player_init(void) {

    // Mediocre-initialize the random number generator
    initarand(DIV_REG);
    player_piece_reset();

    SHOW_SPRITES;

    piece_state = PLAYER_START;

    tick_frame_count = TICK_COUNT_RESET;
}


void player_piece_reset(void) {
    // Locate new piece at default position
    // Top of board, in the middle, no rotation
    player_x      = BRD_NEWPIECE_X;
    player_y      = BRD_NEWPIECE_Y;

    player_attrib = 0; // zero out here, updated below in player_update_gfx()
    player_rotate = GP_ROTATE_DEFUALT;

    // TODO: IMPROVE NEW PIECE SELECTION
    // For now, choose single random pet tile
    player_piece = ((UINT8)DIV_REG & 0x1F);
    // player_piece = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
    //                 (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
    //                  GP_ROT_HORZ);// + TILES_PET_START;

    player_update_gfx();

    // TODO FIX or move to function player_piece_show(false)
    // Hide player sprite by moving it offscreen
    // move_sprite(SPR_PLAYER, 0, 0);

    move_sprite(SPR_PLAYER,
                (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);
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


void player_rotate_apply(UINT8 dir) {

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
    player_update_gfx();
}


// Tests if direction is available
// then updates sprite if it moved
//
// Returns MOVE_OK if piece was moved
//
UINT8 player_move(INT8 dir_x, INT8 dir_y) {

    INT8 tx = (player_x + dir_x);
    INT8 ty = (player_y + dir_y);

    if ((tx >= BRD_MIN_X) &&
        (tx <= BRD_MAX_X) &&
        (ty <= BRD_MAX_Y) &&
         board_check_open_xy(tx, ty)) {

        // TICK sound
        // PlayFx(CHANNEL_1, 30, 0x00, 0xC0, 0x31, 0x8F, 0x85);
        // PlayFx(CHANNEL_1, 30, 0x30, 0x81, 0x33, 0x37, 0x87);
        // PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x22, 0xD6, 0x86);
        // PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x12, 0xD6, 0x86);
//        PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x12, 0xA6, 0x86);

        player_x = tx;
        player_y = ty;
        // TODO: ?? optimize out sprite coordinate offset compensation
        move_sprite(SPR_PLAYER,
                    (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                    (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);

        return (MOVE_OK);
    }

    // If the move failed and the piece
    if ((player_y == BRD_NEWPIECE_Y) && (dir_y == 1))
        return (MOVE_BLOCKED_GAME_OVER);
    else
        return (MOVE_BLOCKED);
}


void player_update_gfx() {

        // Update player rotation (clear bit and then set)
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

        set_sprite_tile(SPR_PLAYER, (player_piece & GP_TILE_MASK));
        set_sprite_prop(SPR_PLAYER, player_attrib);
}



// TODO: split out to player_input.c or game_input.c

void player_handle_input(void) {

    switch (piece_state) {
        case PLAYER_START:

            piece_state = PLAYER_NEWPIECE;
            break;

        case PLAYER_NEWPIECE:
            player_piece_reset();
            piece_state = PLAYER_INPLAY;
            break;

        case PLAYER_INPLAY:

            // TODO: OPTIMIZE: consolidate this into a single left/right test function
            // TODO: move this out of game_player.c
            if (KEY_PRESSED(J_LEFT)) {
                if (key_repeat_count == KEY_REPEAT_START)
                    player_move( -1, 0);
                else if (key_repeat_count >= KEY_REPEAT_LEFTRIGHT_THRESHOLD) {
                    player_move( -1, 0);
                    RESET_KEY_REPEAT(KEY_REPEAT_LEFTRIGHT_RELOAD);
                }
            }
            else if (KEY_PRESSED(J_RIGHT)) {
                if (key_repeat_count == KEY_REPEAT_START)
                    player_move( 1, 0);
                else if (key_repeat_count >= KEY_REPEAT_LEFTRIGHT_THRESHOLD) {
                    player_move( 1, 0);
                    RESET_KEY_REPEAT(KEY_REPEAT_LEFTRIGHT_RELOAD);
                }
            }



            // Requires a key_up event on a new piece
            // before down repeat can take effect.
            // That protects against accidental down repeat on a new piece
            if (KEY_PRESSED(J_DOWN)) {
                // TODO: optimize this (but can't put it in test above)
                if (key_down_repeat_got_release) {
                    if (key_repeat_count == KEY_REPEAT_START)
                        player_gravity_update(); // Call this to allow the player to LAND a piece earlier than tick time
                    else if (key_repeat_count >= KEY_REPEAT_DOWN_THRESHOLD) {
                        player_gravity_update();
                        RESET_KEY_REPEAT(KEY_REPEAT_DOWN_RELOAD);
                    }
                }
            } else {
                key_down_repeat_got_release = TRUE; // TODO: optimize so this only gets reset when needed, not every frame
            }


            // Rotation
            if (KEY_TICKED(J_A)) {
                player_rotate_apply(PLAYER_ROT_RIGHT);
//                PlayFx(CHANNEL_1, 30, 0x3F, 0x4A, 0x67, 0x46, 0xC7);
            }
            else if (KEY_TICKED(J_B)) {
                player_rotate_apply(PLAYER_ROT_LEFT);
//                PlayFx(CHANNEL_2, 30, 0x85, 0xA2, 0xA0, 0x86);
            }


            if (KEY_TICKED(J_SELECT)) {
                board_debug_show_connect_entire_board();

                waitpadup();
                waitpad(J_SELECT);
                waitpadup();

            }


            // Pause
            if (KEY_TICKED(J_START)) {
                // Wait for the player to Start
                waitpadup();
                waitpad(J_START);
                waitpadup();

                // Now reset the key state to avoid passing a keypress into gameplay
                UPDATE_KEYS();
                // TODO: Pause/Resume
            }

            // Move the piece down automatically every N ticks
            tick_frame_count++;

            if (tick_frame_count >= TICK_COUNT_DEFAULT) {
                tick_frame_count = TICK_COUNT_RESET;
                player_gravity_update();
            }

            break;

        case PLAYER_PIECE_LANDED:

            // PlayFx(CHANNEL_1, 30, 0x4A, 0xc4, 0x74, 0x19, 0x86);
            // PlayFx(CHANNEL_1, 30, 0x4A, 0xc4, 0x94, 0x19, 0x86);
            // PlayFx(CHANNEL_1, 30, 0x1B, 0x81, 0x33, 0x73, 0x86);
            PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);

            player_piece_set_on_board();
            piece_state = PLAYER_CHECK_BOARD;
            break;

        case PLAYER_CHECK_BOARD:
            piece_state = PLAYER_NEWPIECE;
            key_down_repeat_got_release = FALSE;
            break;
    }
}


extern UINT8 game_state; // TODO: fix this

// TODO: move/rename ?? game_gracity_update()?
void player_gravity_update(void) {

    // Try to move the piece down one tile
    switch (player_move( 0, 1)) {
        case MOVE_BLOCKED_GAME_OVER:
            // Game is over
            game_state = GAME_ENDED;
            break;

        case MOVE_BLOCKED:
            // Piece landed on board
            // TODO : ?? player_piece_set_on_board();
            piece_state = PLAYER_PIECE_LANDED;
            // piece_state = PLAYER_NEWPIECE;
            break;
    }
}


//void player_visible(UINT8 do_show, INT8 X, INT8 Y) {
    // Show hide on/offscreen
    // move_sprite(SPR_PLAYER,
    //     stars_x[c],
    //     stars_y[c]);
//}


// #define S_FLIPX      0x20U
// #define S_FLIPY      0x40U
// #define S_CGB_PALETTE    0x03U




