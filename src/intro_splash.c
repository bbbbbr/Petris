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

// intro_splash.h

#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

// #include "gfx.h"
#include "common.h"
#include "input.h"
// #include "gfx_print.h"
// #include "fade.h"

#include "intro_splash.h"
#include "intro_cat_out.h"


#define TILES_SPLASH_START  0
#define TILE_COUNT_SPLASH  23

uint8_t scroll_x_amount = 0;
uint8_t  effect_y_line = 0;

#define EFFECT_Y_LINE_MAX      143U
#define EFFECT_START_Y          16U
#define SCX_INVERT_WAVE_BITS  0xFAU

static void init_gfx(void);

/*
void hblank_effect_isr(void) {

    // Don't start until line [effect_y_line]
    if (LY_REG > effect_y_line) {

        // Horizontal waves effect

        // For every line after [effect_y_line] scroll
        // left or right by the number of lines since
        // [effect_y_line] * 4, alternate left/right
        // every other line.
        scroll_x_amount += 6U; // Other values that look good: 4U, 6U

        if (LY_REG & 0x01U)
            // equiv to: SCX_REG = (LY_REG - effect_y_line) << 2;
            SCX_REG = scroll_x_amount;
        else
            // equiv to: SCX_REG = 255U - ((LY_REG - effect_y_line) << 2);
            SCX_REG = 255U - scroll_x_amount;
    }
}
*/

static void init_hblank_isr(void) {

    // Not sure this is supported in the Emulator, so skip this for now

    // VDP.INTERRUPT_CTRL
    // VDP.IRQ0_HCMP
}

static void init_gfx(void) {

    // TODO: Due to scrolling, will probably need to fill entire BG buffer with black tile.
    //       For example: fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, 0);

    HIDE_SPRITES;
    bios_vsync();

    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);

    set_bkg_4bpp_palette(PAL_0, intro_cat_out_PALETTE_COUNT, intro_cat_out_palettes);
    set_bkg_4bpp_data(TILE_NUM_0, intro_cat_out_TILE_COUNT, intro_cat_out_tiles);
    set_bkg_based_tiles((DEVICE_SCREEN_WIDTH - intro_cat_out_TILES_WIDTH) / 2,  // Tile centered X
                  (DEVICE_SCREEN_HEIGHT - intro_cat_out_TILES_HEIGHT) / 2,  // Tile centered Y
                  intro_cat_out_TILES_WIDTH, intro_cat_out_TILES_HEIGHT, intro_cat_out_map, TILE_NUM_0);
}


void intro_splash(void) {


    init_gfx();

/*
    // Scroll so graphics are off-screen to start
    VDP.BG_SCROLL[BG0_SCROLL_Y] = DEVICE_SCREEN_PX_HEIGHT;

    fade_start(FADE_IN);

    // ========== START EFFECT ==========

    effect_y_line = 0U + EFFECT_START_Y;

    // Add the hblank ISR and enable it
    disable_interrupts();
    STAT_REG |=  STATF_MODE00; // (H-Blank)
    add_LCD(hblank_effect_isr);
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
    enable_interrupts();

    // Repeat until effect_y_line reaches the bottom of the screen
    while (effect_y_line <= EFFECT_Y_LINE_MAX) {
        vsync();

        // Reset scroll registers to zero
        // at the start of every frame
        scroll_x_amount = 0U;
        SCY_REG = 0U;
        SCX_REG = 0U;

        // Increment effect position downward by one
        // scanline every other frame
        if (sys_time & 0x01)
            effect_y_line++; // Move effect down screen
    }

    // Disable HBlank interrupt
    disable_interrupts();
    set_interrupts(VBL_IFLAG);
    enable_interrupts();

    // ========== START EFFECT ==========

    delay(500);
    fade_start(FADE_OUT);
*/

    waitpadticked_lowcpu(J_ANY);
}
