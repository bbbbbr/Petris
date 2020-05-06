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

// gameplay.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "common.h"

#include "input.h"
#include "sound.h"
#include "gfx_print.h"
#include "fade.h"

#include "player_piece.h"
#include "player_hinting.h"
#include "player_info.h"
#include "player_gfx.h"

#include "options.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"

#include "gameplay.h"

extern UINT8 global_frame_count;

UINT8 game_speed_drop_frame_counter;
UINT8 game_speed_frames_per_drop;


#define KEY_REPEAT_START               0
#define KEY_REPEAT_DOWN_RELOAD         1
#define KEY_REPEAT_DOWN_THRESHOLD      (KEY_REPEAT_DOWN_RELOAD + 2)
#define KEY_REPEAT_LEFTRIGHT_RELOAD    8
#define KEY_REPEAT_LEFTRIGHT_THRESHOLD (KEY_REPEAT_LEFTRIGHT_RELOAD + 4) //6
UINT8 key_down_repeat_needs_release;
UINT8 gameplay_piece_drop_requested;

UINT8 piece_state;
UINT8 new_piece_launch_delay;


void game_speed_frames_per_drop_set(UINT8 new_val) {
    game_speed_frames_per_drop = new_val;
}



void gameplay_handle_gameover_screen(void) {

    // TODO: add some animation?
    PRINT(BRD_ST_X,
          BRD_ST_Y + 5,
          "GAME  OVER",0);
}



void gameplay_exit_cleanup(void) {

    fade_start(FADE_OUT);
    HIDE_SPRITES;
    // TODO: CLEANUP: hide all extra player sprites
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);
    player_hinting_special_show(FALSE);
    player_hinting_drop_show(FALSE);
}



void gameplay_init(void) {

    board_gfx_init();

    options_player_settings_apply();

    // Mediocre-initialize the random number generator
    initarand(DIV_REG);

    // Flash a get ready message to the player
    board_flash_message(MSG_GET_READY_X, MSG_GET_READY_Y,
                        MSG_GET_READY_TEXT, MSG_GET_READY_CTEXT,
                        MSG_GET_READY_REPEAT);

    board_reset();

    // Generate the very first piece
    game_piece_next_reset();
    game_piece_next_generate();

    // Hide the player piece and preview sprites initially
    // They will get displayed after PLAYER_NEWPIECE is handled
    // which then calls player_piece_try_reload()
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);


    SHOW_SPRITES;

    // TODO: move into player_info_init/reset
    // game_speed_frames_per_drop = GAME_SPEED_FRAMES_PER_DROP_RESET;

    piece_state = PLAYER_START;

    key_down_repeat_needs_release = FALSE;

    gameplay_piece_drop_requested = FALSE;

    game_speed_drop_frame_counter = GAME_SPEED_DROP_FRAME_COUNTER_RESET;

    player_info_newgame_reset();
}


void gameplay_handle_pause(void) {

    // Hide the game board and player piece
    board_hide_all(BRD_CLR_DELAY_NONE);
    // TODO: CONSOLIDATE: these hides are basically a dupe of gameplay_exit_cleanup()
    game_piece_next_show(FALSE);
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    player_hinting_special_show(FALSE);
    player_hinting_drop_show(FALSE);

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
            piece_state = PLAYER_NEWPIECE_PRELAUNCH;
            break;


        case PLAYER_NEWPIECE_PRELAUNCH:
            new_piece_launch_delay = GAME_SPEED_LAUNCH_DELAY_FRAMES;
            piece_state = PLAYER_NEWPIECE_LAUNCH;
            break;


        case PLAYER_NEWPIECE_LAUNCH:
            if (new_piece_launch_delay) {
                new_piece_launch_delay--;
            } else {

                if (player_piece_try_reload()) {
                    // require down key to be released before it can repeat again
                    key_down_repeat_needs_release = TRUE;
                    piece_state = PLAYER_INPLAY;
                } else {
                    // failed to load the piece -> game over
                    game_state = GAME_ENDED;
                }
            }
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

            piece_state = PLAYER_NEWPIECE_PRELAUNCH;
            break;
    }
}



void gameplay_gravity_update(void) {

    // Move the piece down automatically every N ticks
    game_speed_drop_frame_counter++; // TODO: possible to merge in with global_frame_count?

    if ((game_speed_drop_frame_counter >= game_speed_frames_per_drop)
        || (gameplay_piece_drop_requested)) {

        // Reset the counter
        game_speed_drop_frame_counter = GAME_SPEED_DROP_FRAME_COUNTER_RESET;

        // Clear current request to move a piece down earlier than tick time
        // (will get re-enabled next pass if player continues to hold drop key down)
        gameplay_piece_drop_requested = FALSE;

        // Try to move the piece down one tile
        if (player_piece_move( 0, 1) == MOVE_BLOCKED) {

            // Piece landed on board
            piece_state = PLAYER_PIECE_LANDED;
        }
    }

    // Update any flickering hint sprite elements
    // NOTE: This should happen after player_piece_move()
    player_hinting_flicker_update(global_frame_count);
}
