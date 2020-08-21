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
#include "serial_link.h"

#include "sound.h"
#include "gbt_player.h"
#include "audio_common.h"

#include "fade.h"

// #include "game_board.h"
#include "game_board_gfx.h"
#include "gameover_message.h"

#include "gameplay.h"

#include "intro_splash.h"
#include "intro_screen.h"
#include "options_screen.h"

#include "options.h"
#include "player_hinting.h"

#include "input.h"
#include "gfx.h"
#include "gfx_print.h"
#include "serial_link.h"

#include "magic_code.h"

#include "../res/font_tiles.h"


#define DEBUG_SKIP_INTRO

void init (void);
void init_interrupts(void);
void vbl_update(void);
void init_sound(void);

UINT8 vbl_count;

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

    // TODO: animate can be called from here instead to make it independent of game pause/etc
    // if (game_state == GAME_PLAYING)
    //     board_gfx_tail_animate();

    // OPTIONAL: Now that crt0.s is patched for ISR related VRAM exceptions,
    // this could be moved back to the TIM interrupt
    update_gbt_music();

    if(music_mute_frames != 0) {

        music_mute_frames --;

        if(music_mute_frames == 0) {
            gbt_enable_channels(0xF);
        }
    }
}



void init_sound(void) {
    NR52_REG = 0x80; // Enables sound, always set this first
    NR51_REG = 0xFF; // Enables all channels (left and right)
    NR50_REG = 0x77; // Max volume
}

void init_interrupts() {
    disable_interrupts();
    add_VBL(vbl_update);
    // add_TIM(update_gbt_music); // Moved this into vbl_udpate() as workaround for occasional gfx glitches

    //#ifdef CGB
    #ifdef CPU_FAST_ENABLED
        TMA_REG = _cpu == CGB_TYPE ? 120U : 0xBCU;
    #else
        TMA_REG = 0xBCU;
    #endif
        TAC_REG = 0x04U;

    // set_interrupts(VBL_IFLAG | TIM_IFLAG);
    set_interrupts(VBL_IFLAG);

    enable_interrupts();
}

void init (void) {
    music_mute_frames = 0;
    game_state = GAME_INTRO_INIT;

    // Require CGB, otherwise display a warning (DMG/Pocket)
    handle_non_cgb();

    // OPTIONAL: Extra speed doesn't seem to be required right now
    #ifdef CPU_FAST_ENABLED
        // Switch CGB to fast speed mode
        cpu_fast();
    #endif

    init_link();

    init_sound();

    // fade_start(FADE_OUT); // TODO: this can probably be skipped

    init_interrupts();

    DISPLAY_ON;
}


void main(void){
    init();

    magic_code_reset();

    #ifdef DEBUG_SKIP_INTRO
        game_state = GAME_OPTIONS_INIT;
    #else
        intro_splash();
    #endif


    while(1) {
        // Wait for vertical blank (end of the frame)
        // before starting to process the next frame
        // (skip if already happened)
        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;

        // Handle keyboard input
        UPDATE_KEYS();
        UPDATE_KEY_REPEAT((J_LEFT | J_RIGHT | J_DOWN));

        switch (game_state) {

            case GAME_INTRO_INIT:

                intro_screen_init();
                MusicPlay(boss_fight_mod, boss_fight_mod_Data, GBT_LOOP_YES);
                game_state = GAME_INTRO;
                break;


            case GAME_INTRO:
                intro_screen_handle();
                // Done with intro screen, now start game
                if (KEY_TICKED(J_START)) {
                    game_state = GAME_OPTIONS_INIT;
                    fade_start(FADE_OUT);
                    intro_clouds_cleanup();
                }
                break;


            case GAME_OPTIONS_INIT:
                options_screen_init();
                // Options screen will re-start music if music option = ON

                game_state = GAME_OPTIONS;
                break;


            case GAME_OPTIONS:
                options_screen_handle();
                break;


            case GAME_READY_TO_START:
                gameplay_init();
                MusicUpdateStatus();

                game_state = GAME_PLAYING;
                break;


            case GAME_PLAYING:
                gameplay_update();
                break;


            case GAME_WON_LINK_VERSUS:
                MusicStop();
                // TODO: Better sound for 2 Player link versus won
                PLAY_SOUND_LEVEL_UP;
                gameover_message_animate(SPR_YOU_WON_CHARS);

                // Disconnect link (should be try if we're here)
                if (link_status == LINK_STATUS_CONNECTED) {
                    link_reset();
                }

                game_state = GAME_OVER_SCREEN;
                break;


            case GAME_ENDED:
                // If in 2 player versus mode, notify other player they won
                //
                // The command is sent from here since GAME_ENDED can be
                // triggered in multiple locations based on game type
                if (link_status == LINK_STATUS_CONNECTED) {
                    LINK_SEND(LINK_CMD_OPPONENT_LOST);
                }

                MusicStop();
                PLAY_SOUND_GAME_OVER;

                // Disconnect link
                if (link_status == LINK_STATUS_CONNECTED) {
                    link_reset();
                    gameover_message_animate(SPR_YOU_LOST_CHARS);
                } else {
                    gameover_message_animate(SPR_GAMEOVER_CHARS);
                }

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