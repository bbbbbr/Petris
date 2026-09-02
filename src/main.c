// Copyright 2026 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include <gbdk/platform.h>


#include "common.h"

// #include "sound.h"
// #include "gbt_player.h"
// #include "audio_common.h"

// #include "fade.h"

// // #include "game_board.h"
// #include "game_board_gfx.h"
// #include "gameover_message.h"
// #include "game_stats.h"

// #include "gameplay.h"

#include "intro_splash.h"
// #include "intro_screen.h"
// #include "options_screen.h"

// #include "options.h"
// #include "player_hinting.h"

#include "input.h"
// #include "gfx.h"
// #include "gfx_print.h"


#include "magic_code.h"

// #include "../res/font_tiles.h"


// #define DEBUG_SKIP_INTRO

void init (void);
void init_interrupts(void);
void vbl_update(void);
void init_sound(void);
void init_gfx_hardware(void);


// TODO: Wire up vblank callback, increment it
void vbl_update(void) {
    vbl_count++;

    // TODO: Init Play/Pause sound
/*
    update_gbt_music();

    if(music_mute_frames != 0) {

        music_mute_frames --;

        if(music_mute_frames == 0) {
            gbt_enable_channels(0xF);
        }
    }
*/
}


void init_sound(void) {

    // music_mute_frames = 0; // TODO
    // Setup sound hardware (takes a few frames)
    bios_soundChannels(SOUND_CHANS_4CH);
    bios_soundVolume(SOUND_VOL_CH2_3, SOUND_VOL_100);
    bios_soundVolume(SOUND_VOL_CH4,   SOUND_VOL_100);
    bios_initSoundTransmission();
}


void init_interrupts(void) {
    // Enable interrupts and DMA for music
    sys_setInterruptPriority(INT_PRIO_ITU0, 0xF);
    sys_setInterruptMask(0xE);
    sys_setDmaEnabled(true);
}


void init_gfx_hardware(void) {
    bios_vsync();

    VDP.BACKDROP_A      = RGB888(0,0,0);
    VDP.BACKDROP_B      = RGB888(0,0,0);

    // Blend style
    VDP.BLEND           = BLEND_MATH;

    VDP.BG_SCROLL[BG0_SCROLL_X] = 0;
    VDP.BG_SCROLL[BG0_SCROLL_Y] = 0;
    VDP.BG_CTRL         = BG_TILESIZE(BG_TILESIZE_8X8, BG_TILESIZE_8X8) | BG0_FORMAT_4BPP | BG_LAYOUT_64X64_SPLIT;
    VDP.BG_SUBPAL[0]    = BG_PAL_SETUP(PAL_0, PAL_1, PAL_2, PAL_3);  // BG0
    VDP.BG_SUBPAL[1]    = BG_PAL_SETUP(PAL_4, PAL_5, PAL_6, PAL_7);  // BG1

    VDP.SCREENPRIO      = BLEND_MATH_ADD | SCREEN_A_ENABLE | SCREEN_B_ENABLE | PRIORITY_BM_A | PRIORITY_BG0_A | PRIORITY_OBJ0_A;
    VDP.LAYER_CTRL      = LAYER_SCREEN(LAYER_SCREEN_A, LAYER_SCREEN_A, LAYER_SCREEN_A, LAYER_SCREEN_A) | LAYER_ENABLE_BG0 | LAYER_ENABLE_BG1 | LAYER_ENABLE_OBJ0;


    // Set up split between 8bpp and 4bpp tile patterns
    #define  RESERVE_8BPP_TILE_ROWS   0u  // Number of tile pattern rows to reserve for 8bpp tiles (8 per row)
    // AKA CHAR_SPLIT, Tile Base, VDP.TILEBASE
    VDP.CHARBASE        = RESERVE_8BPP_TILE_ROWS;
    set_4bpp_tile_patterns_base_address(CHAR_VRAM_4BPP_START());
}


void init(void) {

    vbl_count = 0;
    game_state = GAME_INTRO_INIT;

    init_sound();
    init_interrupts();
    init_gfx_hardware();

    // Set the appropriate controller scanning mode and video height
    bios_vdpMode(CONTROL_MODE_GAMEPAD, VIDEO_HEIGHT_224P);    
}


int main() {
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
            vsync();
        vbl_count = 0;

        // Handle keyboard input
        UPDATE_KEYS();
        UPDATE_KEY_REPEAT((J_LEFT | J_RIGHT | J_DOWN));

        if KEY_PRESSED(J_LEFT) {
            VDP.BG_SCROLL[BG0_SCROLL_X]--;
        }
        else if KEY_PRESSED(J_RIGHT) {
            VDP.BG_SCROLL[BG0_SCROLL_X]++;
        }
        VDP.BG_SCROLL[BG0_SCROLL_Y]--;

/*
        switch (game_state) {

            case GAME_INTRO_INIT:
                intro_screen_init();
                //  MusicPlay(boss_fight_mod, boss_fight_mod_Data, GBT_LOOP_YES); // TODO
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

            case GAME_ENDED:
                // TODO: 2 Player mode handling (need loopy hardware 4 player controller breakout)

                GAMEOVER_MESSAGE_SET(SPR_GAMEOVER_CHARS);
                game_state = GAME_OVER_SCREEN;
                break;


            case GAME_OVER_SCREEN:
                gameplay_handle_gameover_screen();
                game_state = GAME_OVER_WAITEXIT;
                break;


            case GAME_OVER_WAITEXIT:
                if (KEY_TICKED(J_START | J_A | J_B)) {
                    // Turn sprites off and then fade out
                    HIDE_SPRITES;
                    fade_start(FADE_OUT);
                    game_state = GAME_INTRO_INIT;
                }
                break;
        }
*/
    }
}