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

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

// #include "bgb_emu.h" // Used for debugging only, see BGB_MESSAGE

#include "common.h"
#include "audio_common.h"

#include "fade.h"

// #include "game_board.h"
// #include "game_board_gfx.h"
#include "gameplay.h"
#include "intro_screen.h"
#include "options_screen.h"

#include "player_hinting.h"

#include "input.h"
#include "gfx.h"
#include "gfx_print.h"

#include "../res/font_tiles.h"

void init (void);
void init_interrupts(void);
void vbl_update(void);
void init_sound(void);

UINT8 vbl_count;
UINT8 global_frame_count; // TODO


#define ASM_HALT \
__asm \
  halt \
__endasm

void handle_non_cgb() {
    // TODO: what happens here on a GBA?
    if (_cpu != CGB_TYPE) {
        // BGB_MESSAGE("DMG DETECTED");
        enable_interrupts(); // Make sure interrupts are enabled before calling HALT
        set_bkg_data(TILES_FONT_START, TILE_COUNT_FONT, font_tiles);
        PRINT(3,8,"GAMEBOY COLOR\nREQUIRED :(", 0);
        SHOW_BKG;
        DISPLAY_ON;
        // Loop forever
        while(1) {
            ASM_HALT; // Use less power while idle
        }
    }
}


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

    // Require CGB, otherwise display a warning (DMG/Pocket)
    handle_non_cgb();

    // OPTIONAL: Extra speed doesn't seem to be required right now
    #ifdef CPU_FAST_ENABLED
        // Switch CGB to fast speed mode
        cpu_fast();
    #endif

    init_sound();

    // fade_start(FADE_OUT); // TODO: this can probably be skipped

    init_interrupts();

    game_state = GAME_INTRO_INIT;
    global_frame_count = 0;

    DISPLAY_ON;
}


void main(void){
    init();

    while(1) {
        // Wait for vertical blank
        // (skip if already happened)
        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;
        global_frame_count++; // TODO: share this with game_player

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
                if (KEY_TICKED(J_START)) {
                    game_state = GAME_OPTIONS_INIT;
                    fade_start(FADE_OUT);
                }
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

                game_state = GAME_PLAYING;
                break;


            case GAME_PLAYING:
                gameplay_update();
                break;


            case GAME_ENDED:
                // TODO: add some animation / sounds for game ended
                gameplay_handle_gameover_screen();
                PLAY_SOUND_GAME_OVER;

                game_state = GAME_OVER_SCREEN;
                break;


            case GAME_OVER_SCREEN:

                if (KEY_TICKED(J_START)) {
                    gameplay_exit_cleanup();
                    game_state = GAME_INTRO_INIT;
                }
                break;
        }

    }
}