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

// intro_screen.c

#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

// #include "gfx.h"
// #include "gfx_print.h"
#include "common.h"
#include "sound.h"
#include "input.h"
// #include "fade.h"

#include "magic_code.h"

// #include "player_gfx.h"

#include "intro_screen_out.h"
// #include "../res/pet_tiles.h"
// #include "../res/font_tiles.h"



// #define INTRO_TEXT_BLANK "            "
// #define INTRO_TEXT_START "PRESS  START"


// TODO: Translucent blended clouds overlay for intro screen, using Blend + Math ADD

uint16_t intro_counter = 0;

void intro_clouds_init(void) { // TODO

    intro_counter = 0;
}

void intro_clouds_update(void) { // TODO


}


void intro_clouds_cleanup(void) { // TODO
}


void intro_screen_init(void) {

    intro_clouds_init();

    bios_vsync();
    set_bkg_4bpp_palette(PAL_0, intro_screen_out_PALETTE_COUNT, intro_screen_out_palettes);
    set_bkg_4bpp_data(TILE_NUM_0, intro_screen_out_TILE_COUNT, intro_screen_out_tiles);
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_tiles((DEVICE_SCREEN_WIDTH - intro_screen_out_TILES_WIDTH)/2,  // Tile centered X
                  (DEVICE_SCREEN_HEIGHT - intro_screen_out_TILES_HEIGHT)/2,  // Tile centered Y
                  intro_screen_out_TILES_WIDTH, intro_screen_out_TILES_HEIGHT, intro_screen_out_map);

    // fade_set_pal(BG_PAL_0, 3, intro_screen_logo_palette, FADE_PAL_BKG);
    // fade_set_pal(BG_PAL_4, 3, intro_screen_palette, FADE_PAL_BKG);

    // SHOW_BKG;
    // SHOW_SPRITES;

    // // To avoid flicker from initial print changing the attrib tiles
    // // print the tiles before fade-in
    // PRINT(4,10, INTRO_TEXT_BLANK, 0);

    // fade_start(FADE_IN);  // TODO

}



void intro_screen_handle(void) {

    intro_clouds_update();

    magic_code_update();

    // Wait for the player to press start
    intro_counter++;

    // TODO: Some kind of "Press Start" indicator on intro screen, maybe less bare bones than original
    //
    // if (intro_counter == 1) {
    //     PRINT(4,10, INTRO_TEXT_START, 0);
    // }
    // else if (intro_counter == 125) {
    //     PRINT(4,10, INTRO_TEXT_BLANK, 0);
    // }
    // else if (intro_counter == 200) {
    //     intro_counter = 0;
    // }
}