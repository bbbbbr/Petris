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
#define TILE_COUNT_SPLASH  17

void intro_splash(void) {

    fade_set_pal(BG_PAL_0, 1, intro_cat_palette, FADE_PAL_BKG);

    set_bkg_data(TILES_SPLASH_START, TILE_COUNT_SPLASH, intro_cat_tiles);

    // Load BG tile attribute map
    VBK_REG = 1;
    set_bkg_tiles(6, 6, 8, 5, intro_cat_mapPLN1);

    // Load BG tile map
    VBK_REG = 0;
    set_bkg_tiles(6, 6, 8, 5, intro_cat_mapPLN0);

    SHOW_BKG;

    fade_start(FADE_IN);
    delay(1700);
    // waitpad_lowcpu(J_START, J_START);
    fade_start(FADE_OUT);


}
