// gameplay.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "common.h"
#include "input.h"
#include "sound.h"
#include "gfx_print.h"

#include "player_info.h"
#include "player_piece.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"

#include "gameplay.h"


UINT8 tick_frame_count;
UINT8 tick_frame_speed = PLAYER_SPEED_TICK_COUNT__DEFAULT; // TODO : rename and move this


#define KEY_REPEAT_START               0
#define KEY_REPEAT_DOWN_RELOAD         1
#define KEY_REPEAT_DOWN_THRESHOLD      (KEY_REPEAT_DOWN_RELOAD + 2)
#define KEY_REPEAT_LEFTRIGHT_RELOAD    8
#define KEY_REPEAT_LEFTRIGHT_THRESHOLD (KEY_REPEAT_LEFTRIGHT_RELOAD + 4) //6
UINT8 key_down_repeat_needs_release;
UINT8 gameplay_piece_drop_requested;

UINT8 piece_state;


void gameplay_handle_gameover_screen(void) {

    // TODO: add some animation?
    PRINT(BRD_ST_X + 1,
          BRD_ST_Y + 5,
          "GAME OVER",0);
}



void gameplay_exit_cleanup(void) {

    HIDE_SPRITES;
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);
}



void gameplay_init(void) {

    board_gfx_init();

    // Mediocre-initialize the random number generator
    initarand(DIV_REG);

    board_reset();

    // Generate the very first piece
    game_piece_next_generate();

    // Hide the player piece and preview sprites initially
    // They will get displayed after PLAYER_NEWPIECE is handled
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);

    // Update the player piece to retrieve it and generate the next
//    player_piece_reset();


    SHOW_SPRITES;

    piece_state = PLAYER_START;

    key_down_repeat_needs_release = FALSE;
    gameplay_piece_drop_requested = FALSE;
    tick_frame_count = PLAYER_SPEED_TICK_COUNT__RESET;

    score_reset();
    level_reset();
}



void gameplay_handle_pause(void) {

    // Hide the game board and player piece
    board_hide_all();
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);

    PRINT(BRD_ST_X + 2,
          BRD_ST_Y + 5,
          "PAUSED",0);


    // TODO: This could probably be done better (interrupts?)
    // Need a delay for joypad to start returning updated values
    while (joypad() & J_START); // Wait until Start is released

    delay(100);
    while (!(joypad() & J_START)); // Wait for start

    delay(100);
    while (joypad() & J_START); // Wait until Start is released

    UPDATE_KEYS(); // refresh key state to make sure it's in sync

    // Redraw the board and player piece
    board_redraw_all();
    player_piece_update_xy(PLAYER_PIECE_SHOW);
    game_piece_next_show(TRUE);
}


void gameplay_handle_input(void) {

    // Move piece left/right
    //
    // TODO: OPTIMIZE: consolidate this into a single left/right test function
    if (KEY_PRESSED(J_LEFT)) {
        if (key_repeat_count == KEY_REPEAT_START)
            player_piece_move( -1, 0);
        else if (key_repeat_count >= KEY_REPEAT_LEFTRIGHT_THRESHOLD) {
            player_piece_move( -1, 0);
            RESET_KEY_REPEAT(KEY_REPEAT_LEFTRIGHT_RELOAD);
        }
    }
    else if (KEY_PRESSED(J_RIGHT)) {
        if (key_repeat_count == KEY_REPEAT_START)
            player_piece_move( 1, 0);
        else if (key_repeat_count >= KEY_REPEAT_LEFTRIGHT_THRESHOLD) {
            player_piece_move( 1, 0);
            RESET_KEY_REPEAT(KEY_REPEAT_LEFTRIGHT_RELOAD);
        }
    }


    // Pressing Down Accelerate dropping the current piece
    if (KEY_PRESSED(J_DOWN)) {

        // TODO: could this be handled more gracefully by setting
        //       key_repeat_count = 0 on a new piece instead?
        // * Requires down key to be released on a new piece
        //   before key repeat can take effect again.
        //   That protects against accidental down
        //   repeat on a new piece
        if (!key_down_repeat_needs_release) {

            if (key_repeat_count == KEY_REPEAT_START) {
                gameplay_piece_drop_requested = TRUE;// Request moving a piece down earlier than tick time
            } else if (key_repeat_count >= KEY_REPEAT_DOWN_THRESHOLD) {
                gameplay_piece_drop_requested = TRUE; // Request moving a piece down earlier than tick time
                RESET_KEY_REPEAT(KEY_REPEAT_DOWN_RELOAD);
            }
        }
    } else {
        key_down_repeat_needs_release = FALSE;  // Clear pending requirement for down key to be released before it can repeat again
    }


    // Rotate piece
    if (KEY_TICKED(J_A)) {
        player_piece_rotate_apply(PLAYER_ROT_RIGHT);
    }
    else if (KEY_TICKED(J_B)) {
        player_piece_rotate_apply(PLAYER_ROT_LEFT);
    }


    // Pause
    if (KEY_TICKED(J_START)) {

        gameplay_handle_pause();
    }
}



void gameplay_update(void) {

    switch (piece_state) {
        case PLAYER_START:

            piece_state = PLAYER_NEWPIECE;
            break;

        case PLAYER_NEWPIECE:
            player_piece_reset();

            // require down key to be released before it can repeat again
            key_down_repeat_needs_release = TRUE;
            piece_state = PLAYER_INPLAY;
            break;

        case PLAYER_INPLAY:
            gameplay_handle_input();
            gameplay_gravity_update();
            break;

        case PLAYER_PIECE_LANDED:

            PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);

            // Sets the piece on the board
            // Then checks for completed pet removal
            player_piece_set_on_board();
            piece_state = PLAYER_NEWPIECE;
            break;
    }
}



void gameplay_gravity_update(void) {

    // Move the piece down automatically every N ticks
    tick_frame_count++;

    if ((tick_frame_count >= tick_frame_speed)
        || (gameplay_piece_drop_requested)) {

        tick_frame_count = PLAYER_SPEED_TICK_COUNT__RESET;
        gameplay_piece_drop_requested = FALSE;

        // Try to move the piece down one tile
        switch (player_piece_move( 0, 1)) {

            case MOVE_BLOCKED_GAME_OVER:
                // Game is over
                game_state = GAME_ENDED;
                break;

            case MOVE_BLOCKED:
                // Piece landed on board
                piece_state = PLAYER_PIECE_LANDED;
                break;
        }
    }
}