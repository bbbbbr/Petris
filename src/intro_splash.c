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

#define TILES_SPLASH_START 0
#define TILE_COUNT_SPLASH  23


UINT8  effect_y_line;
#define EFFECT_Y_LINE_MAX  143
#define EFFECT_START_Y      16
#define SCX_INVERT_WAVE_BITS   0xFA

void hblank_effect_isr() {

    // Scroll in the Y direction by -1 every 1 scanlines
    // after [effect_y_line] is reached.
    // This makes the scanline at [effect_y_line]
    // stretch-repeat down to the bottom of the screen

    // Then XOR the updated value in SCY to create diagonal
    // waves that vary based on the Y scanline and move
    // farther left toward the bottom of the screen

    // This doesn't always make timing, which is
    // ok since that adds some useful visual artifacts

    // Don't start until line [effect_y_line]
    if (LY_REG > effect_y_line) {

        // Update the scroll Y register to scroll back
        // to and repeat the scanline at [effect_y_line]
        SCY_REG = (effect_y_line - LY_REG);
        SCX_REG = SCY_REG ^ SCX_INVERT_WAVE_BITS;

        // Alternate slide in from Right Edge
        // that's a little more subtle
        // if (SCY_REG > (256 - 16))
        //     SCX_REG = (SCY_REG << 3);
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
    //
    // Slide in diagonal effect

    effect_y_line           = 0 + EFFECT_START_Y;  // Start with effect at bottom of screen

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
        SCY_REG = 0;
        SCX_REG = 0;

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
