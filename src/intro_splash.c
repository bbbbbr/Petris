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

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "gfx.h"
#include "gfx_print.h"
#include "input.h"
#include "fade.h"

#include "intro_splash.h"

#include "../res/intro_cat_tiles.h"
#include "../res/intro_cat_map.h"

#define TILES_SPLASH_START  0
#define TILE_COUNT_SPLASH  23

UINT8 scroll_x_amount;
UINT8  effect_y_line;

#define EFFECT_Y_LINE_MAX      143U
#define EFFECT_START_Y          16U
#define SCX_INVERT_WAVE_BITS  0xFAU

void hblank_effect_isr() {


    // Don't start until line [effect_y_line]
    if (LY_REG > effect_y_line) {

        // #define EFFECT_DIAG_SLIDE_1
        // #define EFFECT_DIAG_SLIDE_2
        #define EFFECT_WAVES

        #ifdef EFFECT_DIAG_SLIDE_1
            // Diagonal slide in effect

            // Scroll in the Y direction by -1 every 1 scanlines
            // after [effect_y_line] is reached.
            // This makes the scanline at [effect_y_line]
            // stretch-repeat down to the bottom of the screen

            // Then XOR the updated value in SCY to create diagonal
            // waves that vary based on the Y scanline and move
            // farther left toward the bottom of the screen

            // This doesn't always make timing, which is
            // ok since that adds some useful visual artifacts
            // SCY_REG = effect_y_line - LY_REG; // <- Compiler generates error
            SCY_REG = 255U - (LY_REG - effect_y_line); // Workaround for compiler error
            SCX_REG = SCY_REG ^ SCX_INVERT_WAVE_BITS;
        #endif

        #ifdef EFFECT_DIAG_SLIDE_2
            // Slide in from left and right sides

            scroll_x_amount++;
            SCY_REG = 255U - scroll_x_amount;
            if (LY_REG & 1)
                SCX_REG = SCY_REG ^ SCX_INVERT_WAVE_BITS;
            else
                SCX_REG = 255U - (SCY_REG ^ SCX_INVERT_WAVE_BITS);
        #endif

        #ifdef EFFECT_WAVES
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
        #endif
    }
}


void intro_splash(void) {

    // Scroll so graphics are off-screen to start
    SCY_REG = 100;

    fade_set_pal(BG_PAL_0, 1, intro_cat_palette, FADE_PAL_BKG);

    set_bkg_data(TILES_SPLASH_START, TILE_COUNT_SPLASH, intro_cat_tiles);

    // Load BG tile attribute map
    VBK_REG = 1;
    set_bkg_tiles(6, 5, intro_cat_mapWidth, intro_cat_mapHeight, intro_cat_mapPLN1);

    // Load BG tile map
    VBK_REG = 0;
    set_bkg_tiles(6, 5, intro_cat_mapWidth, intro_cat_mapHeight, intro_cat_mapPLN0);

    SHOW_BKG;

    fade_start(FADE_IN);


    // ========== START EFFECT ==========

    effect_y_line = 0U + EFFECT_START_Y;

    // Add the hblank ISR and enable it
    disable_interrupts();
    STAT_REG = 0x18;
    add_LCD(hblank_effect_isr);
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
    enable_interrupts();

    // Repeat until effect_y_line reaches the bottom of the screen
    while (effect_y_line <= EFFECT_Y_LINE_MAX) {
        wait_vbl_done();

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
    set_interrupts(VBL_IFLAG);

    // ========== START EFFECT ==========

    delay(500);
    fade_start(FADE_OUT);
}
