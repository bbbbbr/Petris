#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "common.h"

// #include "game_board.h"
// #include "game_board_gfx.h"
#include "gameplay.h"
#include "intro_screen.h"
#include "options_screen.h"

#include "input.h"
#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"

void init (void);
void init_interrupts(void);
void vbl_update(void);
void init_sound(void);


UINT8 vbl_count;
// UINT8 frame_count; // TODO


void vbl_update() {
    vbl_count ++;
}


void init_sound(void) {
    NR52_REG = 0x80; // Enables sound, always set this first
    NR51_REG = 0xFF; // Enables all channels (left and right)
    NR50_REG = 0x77; // Max volume
}

void init_interrupts() {
    disable_interrupts();
    add_VBL(vbl_update);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();
}

void init (void) {
    // TODO: POWER: is this speed needed? if not, make it more power efficient
    #ifdef CPU_FAST_ENABLED
        // Switch CGB to fast speed mode
        cpu_fast();
    #endif

    init_sound();

    DISPLAY_ON;

    game_state = GAME_INTRO_INIT;

    //frame_count = 0;
}


void main(void){
    init();

    while(1) {
        // Wait for vertical blank
        // (skip if already happened)
        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;
        //frame_count++; // TODO: share this with game_player

        // Handle keyboard input
        UPDATE_KEYS();
        UPDATE_KEY_REPEAT((J_LEFT | J_RIGHT | J_DOWN));

        switch (game_state) {

            case GAME_INTRO_INIT:
                intro_screen_init();
                game_state = GAME_INTRO;
                break;


            case GAME_INTRO:
                intro_screen_handle();
                // Done with intro screen, now start game
                if (KEY_TICKED(J_START))
                    game_state = GAME_READY_TO_START;
                    // game_state = GAME_OPTIONS_INIT; // TODO : enable options
                break;


            case GAME_OPTIONS_INIT:
                options_screen_init();
                game_state = GAME_OPTIONS;
                break;


            case GAME_OPTIONS:
                options_screen_handle();
                break;


            case GAME_READY_TO_START:
                // board_gfx_init(); // moved to gameplay_init
                //     game_state = GAME_START; // TODO: remove this state
                //     break;
                // case GAME_START: // TODO: move to GAME_BOARD_INIT
                gameplay_init();

                // TODO: give the player time to get ready (maybe flash "READY")
                delay(500);
                game_state = GAME_PLAYING;
                break;


            case GAME_PLAYING:
                gameplay_update();
                break;


            case GAME_ENDED:
                // TODO: add some animation / sounds for game ended
                // gameplay_handle_game_ended();
                game_state = GAME_OVER_SCREEN;
                break;


            case GAME_OVER_SCREEN:

                gameplay_handle_gameover_screen();

                if (KEY_TICKED(J_START)) {
                    gameplay_exit_cleanup();
                    game_state = GAME_INTRO_INIT;
                }
                break;
        }

    }
}