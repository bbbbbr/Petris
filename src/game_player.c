// game_player.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "game_player.h"

#include "game_pieces.h"
#include "game_board.h"
#include "input.h"

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
#define TICK_COUNT_DEFAULT 60 // 60 frames per second default speed
UINT8 tick_frame_count;


#define SPR_PLAYER 0 // Player is sprite "0"

UINT8 piece_state;

UINT8 player_x;
UINT8 player_y;
UINT8 player_rotate;
UINT8 player_piece;


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

    player_rotate = GP_ROTATE_DEFUALT;

    // TODO: IMPROVE NEW PIECE SELECTION
    // For now, choose single random pet tile
    player_piece = ((UINT8)DIV_REG & 0x1F);

    player_update_gfx(player_piece);

    // TODO FIX or move to function player_piece_show(false)
    // Hide player sprite by moving it offscreen
    // move_sprite(SPR_PLAYER, 0, 0);

    move_sprite(SPR_PLAYER,
                (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET,
                (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET);
}



// TODO: optimize out this function unless it starts doing more
void player_piece_set_on_board(void) {

    board_set_tile(player_x, player_y, player_piece);
}


// Tests if direction is available
// then updates sprite if it moved
//
// Returns TRUE if piece was moved
//
UINT8 player_move(INT8 dir_x, INT8 dir_y) {

    UINT8 tx = (player_x + dir_x);
    UINT8 ty = (player_y + dir_y);

    if ((tx >= BRD_MIN_X) &&
        (tx <= BRD_MAX_X) &&
        (ty <= BRD_MAX_Y) &&
         board_check_open(tx, ty)) {

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


void player_update_gfx(UINT8 new_piece) {

        set_sprite_tile(SPR_PLAYER, (new_piece & GP_TILE_MASK));

        set_sprite_prop(SPR_PLAYER, ((new_piece & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                          // TODO: Sprite mirror bits
                           );
}



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

            // TODO: allow key repeat (may need KEY_TICKED() -> STATE=MOVING -> still?KEY_PRESSED() -> COUNTER -> THRESHOLD -> MOVE PIECE -> THRESHOLD_FAST ->  STATE=MOVINGFAST
            if (KEY_TICKED(J_LEFT)) {
                player_move( -1, 0);
            }
            else if (KEY_TICKED(J_RIGHT)) {
                player_move( 1, 0);
            }
            else if (KEY_TICKED(J_DOWN)) {
                player_move( 0, 1);
                // TODO: allow DOWN key repeat while LEFT/RIGHT repeating? need separate var if so
            }


            if (KEY_TICKED(J_START)) {
                // while( !KEY_TICKED(J_START) ) {
                //     UPDATE_KEYS();
                // }
                // TODO: Pause/Resume
            }

            player_gravity_update();

            break;

        case PLAYER_PIECE_LANDED:
            player_piece_set_on_board();
            piece_state = PLAYER_CHECK_BOARD;
            break;

        case PLAYER_CHECK_BOARD:
            piece_state = PLAYER_NEWPIECE;
            break;
    }
}


extern UINT8 game_state; // TODO: fix this

// TODO: move/rename ?? game_gracity_update()?
void player_gravity_update(void) {

    // Move the piece down automatically every N ticks
    tick_frame_count++;

    if (tick_frame_count >= TICK_COUNT_DEFAULT) {
        tick_frame_count = TICK_COUNT_RESET;

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




