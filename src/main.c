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

#include "game_board.h"
// #include "game_board_gfx.h"
// #include "gameover_message.h"
// #include "game_stats.h"

#include "gameplay.h"

#include "intro_splash.h"
#include "intro_screen.h"
#include "options_screen.h"

#include "options.h"
// #include "player_hinting.h"

#include "input.h"
#include "gfx.h"
#include "gfx_print.h"


#include "magic_code.h"


#define DEBUG_SKIP_INTRO


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

    for (int c = 0; c < 4; c++) {
        VDP.BM_SCREENX[c]   = 0u;
        VDP.BM_SCREENY[c]   = 0u;
        VDP.BM_WIDTH[c]     = (0u << 8) | 255u;
        VDP.BM_HEIGHT[c]    = 223u;
        VDP.BM_SCROLLX[c]   = 0u;
        VDP.BM_SCROLLY[c]   = 0u;
    }
    VDP.BG_SCROLL[BG0_SCROLL_X] = 0;
    VDP.BG_SCROLL[BG0_SCROLL_Y] = 0;

    VDP.OBJ_CTRL                  = OBJ_LAYER_SPLIT_ALL_OBJ0 | OBJ_FORMAT_4BPP | OBJ0_USE_TILEGROUP_512_767;
    VDP.BM_CTRL                   = BM_MODE_4BPP_SHARED; /* 512 x 512 shared */
    VDP.BG_CTRL                   = BG_TILESIZE(BG_TILESIZE_8X8, BG_TILESIZE_8X8) | BG0_FORMAT_4BPP | BG_LAYOUT_64X64_SPLIT;  // 2x 64x64 tilemaps

    VDP.OBJ_SUBPAL[OBJ0_SUBPALS]  = BG_PAL_SETUP(PAL_ASSIGN_OBJ_0, PAL_ASSIGN_OBJ_1, PAL_ASSIGN_OBJ_2, PAL_ASSIGN_OBJ_3);
    VDP.BM_SUBPAL                 = BM_SUBPAL(PAL_ASSIGN_BM_0, PAL_ASSIGN_BM_1, PAL_ASSIGN_BM_2, PAL_ASSIGN_BM_3);
    VDP.BG_SUBPAL[BG0_SUBPALS]    = BG_PAL_SETUP(PAL_ASSIGN_BG0_0, PAL_ASSIGN_BG0_1, PAL_ASSIGN_BG0_2, PAL_ASSIGN_BG0_3);
    VDP.BG_SUBPAL[BG1_SUBPALS]    = BG_PAL_SETUP(PAL_ASSIGN_BG1_0, PAL_ASSIGN_BG1_1, PAL_ASSIGN_BG1_2, PAL_ASSIGN_BG1_3);

    VDP.SCREENPRIO      = BLEND_MATH_ADD | SCREEN_A_ENABLE | SCREEN_B_ENABLE | PRIORITY_BM_B | PRIORITY_BG0_A | PRIORITY_OBJ0_A;
    VDP.LAYER_CTRL      = LAYER_SCREEN(LAYER_SCREEN_A, LAYER_SCREEN_B, LAYER_SCREEN_A, LAYER_SCREEN_A) | LAYER_ENABLE_BG0 | LAYER_ENABLE_BG1 | LAYER_ENABLE_OBJ0 | LAYER_ENABLE_BM2 | LAYER_ENABLE_BM3;

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


extern volatile uint16_t timeout;

int main() {
    init();
    magic_code_reset();

    #ifdef DEBUG_SKIP_INTRO
        game_state = GAME_INTRO_INIT;
    #else
        intro_splash();
    #endif

    // enable_interrupt_nmi_vblank();  // WARNING: Cannot be enabled when using DMA!
    enable_interrupt_irq0_vblank();

    while(1) {
        // Wait for vertical blank (end of the frame)
        // before starting to process the next frame
        // (skip if already happened)
        if(!vbl_count) // TODO: fixme with vbl_count increment in interrupt
            vsync();
        vbl_count = 0;
        print_num_u16(0,0, sys_time, STR_DIGIT_LEN_5); // DEBUG
        // print_num_u16(0,1, INTC_ICR, STR_DIGIT_LEN_5); // DEBUG

        // Handle keyboard input
        UPDATE_KEYS();
        UPDATE_KEY_REPEAT((J_LEFT | J_RIGHT | J_DOWN));

        // Debug logging of DMA Controller via Right Trigger button
        if (KEY_TICKED(J_RTRIG)) {
            print_hex_u16(0,2, (uint16_t)(DMAC_SAR0 >> 16)); print_hex_u16(4,2, DMAC_SAR0);
            print_hex_u16(0,4, (uint16_t)(DMAC_DAR0 >> 16)); print_hex_u16(4,4, DMAC_DAR0);
            print_hex_u16(0,6, DMAC_TCR0);
            print_hex_u16(0,8, DMAC_CHCR0);
            print_hex_u16(0,10, DMAC_DMAOR);
        }

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
                   intro_screen_cleanup();
                }
                break;

            case GAME_OPTIONS_INIT:
                game_state = GAME_OPTIONS;
                options_screen_init();
                game_state = GAME_OPTIONS;
                break;

            case GAME_OPTIONS:
                options_screen_handle();
                break;

            case GAME_READY_TO_START:
                gameplay_init();
                // MusicUpdateStatus();  // TODO
                game_state = GAME_PLAYING;

                break;

            case GAME_PLAYING:
                gameplay_update();
                break;

            case GAME_ENDED:
                // TODO: 2 Player mode handling (need loopy hardware 4 player controller breakout)

                // GAMEOVER_MESSAGE_SET(SPR_GAMEOVER_CHARS);
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
                    // fade_start(FADE_OUT);
                    game_state = GAME_INTRO_INIT;
                }
                break;

        }        
    }
}