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

#include "audio_common.h"
#include "gbt_player.h"
#include "sound.h"

#include "input.h"
#include "gfx_print.h"
#include "fade.h"

#include "magic_code.h"

#include "player_piece.h"
#include "player_hinting.h"
#include "player_info.h"
#include "player_gfx.h"

#include "options.h"
#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"
#include "game_types.h"

#include "gameplay.h"

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


// Turning this off for now since it's just a pass-through
// void gameplay_handle_gameover_screen(void) {
//
//     // OPTIONAL:
//     board_gameover_animate();
// }



void gameplay_exit_cleanup(void) {

    fade_start(FADE_OUT);
    HIDE_SPRITES;

    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);
    player_hinting_special_show(FALSE);
    player_hinting_drop_show(FALSE);
    hinting_petlength_reset();
    board_gameover_animate_reset();
}



void gameplay_init(void) {

    // Initialize the random number generator
    initarand(DIV_REG);

    game_types_init(); // Call before board_gfx_init()

    board_init();
    board_gfx_init();
    hinting_petlength_reset();

    options_player_settings_apply();

    // Should be called before gameplay_prepare_board() ... -> game_board_fill_random_tails()
    player_info_newgame_reset();

    // Flash a get ready message to the player
    // TODO: function or struct to select game_start_message[option_game_type]
    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {

        board_flash_message(MSG_GET_READY_X, MSG_GET_READY_Y,
                            MSG_CLEANUP_START_TEXT, MSG_CLEANUP_START_CTEXT,
                            MSG_CLEANUP_START_REPEAT);
    } else if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {

        board_flash_message(MSG_LONG_PET_X, MSG_LONG_PET_Y,
                            MSG_LONG_PET_START_TEXT, MSG_LONG_PET_START_CTEXT,
                            MSG_LONG_PET_START_REPEAT);
    } else {
        // Default Get Ready Message
        board_flash_message(MSG_GET_READY_X, MSG_GET_READY_Y,
                            MSG_GET_READY_TEXT, MSG_GET_READY_CTEXT,
                            MSG_GET_READY_REPEAT);

    }

    gameplay_prepare_board();

    SHOW_SPRITES;

    // TODO: move into player_info_init/reset
    // game_speed_frames_per_drop = GAME_SPEED_FRAMES_PER_DROP_RESET;

    piece_state = PLAYER_START;

    key_down_repeat_needs_release = FALSE;

    gameplay_piece_drop_requested = FALSE;

    game_speed_drop_frame_counter = GAME_SPEED_DROP_FRAME_COUNTER_RESET;
}



// Called on new game and during transition to new level
// TODO: could be moved to board_prepare_new_level()
void gameplay_prepare_board(void) {

    board_reset();

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        // This will (indirectly) auto-increment game_type_cleanup_tail_count
        game_board_fill_random_tails( game_type_pet_cleanup_get_tail_count( (UINT8)player_level ));

        // Give the player a brief moment to see the board
        // before gameplay starts. Helpful on higher levels
        // and high difficulty settings
        delay(500);
    }

    // Generate the very first piece
    game_piece_next_reset();
    game_piece_next_generate();

    // Hide the player piece and preview sprites initially
    // They will get displayed after PLAYER_NEWPIECE is handled
    // which then calls player_piece_try_reload()
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    game_piece_next_show(FALSE);
}


void gameplay_handle_pause(void) {

    if (option_game_music != OPTION_MUSIC_OFF) {
        gbt_pause(1);
        // Mute output of all sound since gpt_pause just freezes the sound and holds the current note
        //NR50_REG = 0x00; // Max volume .[2..0] Right Main Vol, .[6..4] Left Main Volume
        NR51_REG = 0x00; // Turn off all channels (left and right)
    }

    // Hide the game board and player piece (except in long-pet mode)
    // TODO: allow in all other modes?
    if (option_game_type != OPTION_GAME_TYPE_LONG_PET) {
        board_hide_all(BRD_CLR_DELAY_NONE);
    }

    // TODO: CONSOLIDATE: these hides are basically a dupe of gameplay_exit_cleanup()
    game_piece_next_show(FALSE);
    player_piece_update_xy(PLAYER_PIECE_HIDE);
    player_hinting_special_show(FALSE);
    player_hinting_drop_show(FALSE);

    PRINT(BRD_ST_X + 2,
          BRD_ST_Y + 2,
          "PAUSED",0);



    waitpad_lowcpu(J_START, J_WAIT_ALL_RELEASED); // Wait until Start is released

    waitpad_lowcpu(J_START, J_WAIT_ANY_PRESSED);  // Wait for start and then wait again until it's released
    waitpad_lowcpu(J_START, J_WAIT_ALL_RELEASED);

    UPDATE_KEYS(); // refresh key state to make sure it's in sync

    // Redraw the board and player piece
    board_redraw_all();
    player_piece_update_xy(PLAYER_PIECE_SHOW);
    game_piece_next_show(TRUE);

    if (option_game_music != OPTION_MUSIC_OFF) {
        gbt_pause(0);
        //NR50_REG = 0x77; // Max volume .[2..0] Right Main Vol, .[6..4] Left Main Volume
        NR51_REG = 0xFF; // Turn ON all channels (left and right)
    }
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
    else if (KEY_TICKED(J_UP)) {

        // Cycle through different pet types if allowed
        if (magic_code_state == MAGIC_CODE_STATE_ACTIVATED) {
            player_piece_cycle_pet_types();
        }
    }


    // Pause
    if (KEY_TICKED(J_START)) {

        gameplay_handle_pause();
    }


    // Toggle Pet Length overlay
    if (KEY_TICKED(J_SELECT)) {

        // PET_LENGTH_PREVIEW_ENABLED_FOR_ALL_MODES
        //
        // if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
            hinting_petlength_turn_on();
        // }
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
            PLAY_SOUND_PIECE_LANDED;

            // Sets the piece on the board
            // Then checks for completed pet removal
            player_piece_set_on_board();

            piece_state = PLAYER_NEWPIECE_PRELAUNCH;
            break;
    }

    // Handle board animation updates
    board_gfx_tail_animate();

    // Handle timeout of Pet Length Overlay if applicable
    if (hinting_petlength_enabled) {

        hinting_petlength_enabled--;

        if (hinting_petlength_enabled == 0)
            hinting_petlength_show();
    }
}



void gameplay_gravity_update(void) {

    // Move the piece down automatically every N ticks
    game_speed_drop_frame_counter++;

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
    player_hinting_flicker_update();
}
