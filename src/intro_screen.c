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

#include "gfx.h"
#include "gfx_print.h"
#include "common.h"
#include "sound.h"
#include "input.h"
// #include "fade.h"

#include "magic_code.h"

// #include "player_gfx.h"

#include "intro_screen_out.h"
#include "intro_logo_out.h"
// #include "../res/pet_tiles.h"
// #include "../res/font_tiles.h"


#define LOGO_TILE_Y_START 5u

#define STR_INTRO_TEXT_START "PRESS START"
#define OAM_TEXT_INTRO_START 0u
#define OAM_TEXT_START_X  72u
#define OAM_TEXT_START_Y  136u

static uint16_t oam_text_intro_end = 0;


static uint16_t intro_counter = 0;

// TODO: Translucent blended clouds overlay for intro screen, using Blend + Math ADD
void intro_clouds_init(void) { // TODO
    intro_counter = 0;
}

void intro_clouds_update(void) { // TODO
}


void intro_clouds_cleanup(void) { // TODO
}


static void intro_text_sprites_update(void);


void intro_screen_init(void) {

    intro_clouds_init();

    bios_vsync();
    // Main background on BG1 layer
    // BG1 is set to use pals 4,5,6,7 (so no need to offset palette)
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);

    set_bkg_4bpp_palette(PAL_4, intro_screen_out_PALETTE_COUNT, intro_screen_out_palettes);
    set_bkg_4bpp_data(TILE_NUM_0, intro_screen_out_TILE_COUNT, intro_screen_out_tiles);
    set_bkg_tilemap_base_address(BG1_MAP_START());
    set_bkg_tiles((DEVICE_SCREEN_WIDTH - intro_screen_out_TILES_WIDTH)/2,  // Tile centered X
                  (DEVICE_SCREEN_HEIGHT - intro_screen_out_TILES_HEIGHT)/2,  // Tile centered Y
                  intro_screen_out_TILES_WIDTH, intro_screen_out_TILES_HEIGHT, intro_screen_out_map);


    // Petris Logo is now on BG0 so it can be scrolled over background in BG1

    // Logo background on BG0 layer so it can scroll
    // BG0 is set to use pals 0,1,2,3
    #define TILE_LOAD_OFFSET_LOGO (intro_screen_out_TILE_COUNT)
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);

    set_bkg_4bpp_palette(PAL_0, intro_logo_out_PALETTE_COUNT, intro_logo_out_palettes);
    set_bkg_4bpp_data(TILE_LOAD_OFFSET_LOGO, intro_logo_out_TILE_COUNT, intro_logo_out_tiles);
    // First fill entire BG1 tilemap with clear tiles to ensure
    // BG0 to shows through in transparent parts
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, TILE_LOAD_OFFSET_LOGO);
    set_bkg_based_tiles((DEVICE_SCREEN_WIDTH - intro_logo_out_TILES_WIDTH)/2,  // Tile centered X
                        LOGO_TILE_Y_START,
                        intro_logo_out_TILES_WIDTH, intro_logo_out_TILES_HEIGHT, intro_logo_out_map, TILE_LOAD_OFFSET_LOGO);


    // Load the 8x16 font and print it to the sprites
    #define TILE_LOAD_OFFSET_FONT (OBJ_TILEGROUP_BASE_512)
    load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT);

    oam_text_intro_end = print_to_sprites(OAM_TEXT_INTRO_START, OAM_TEXT_START_X, OAM_TEXT_START_Y, S_PAL2, STR_INTRO_TEXT_START);
    intro_text_sprites_update();


    // uint16_t oam_next = print_to_sprites(oam_text_intro_end, DEVICE_SCREEN_PX_WIDTH - (13 * SPR_PRINT_SPACING_X), DEVICE_SCREEN_PX_HEIGHT - 16u,
    //                                      S_PAL2, "bbbbbr 2026");

    // fade_set_pal(BG_PAL_0, 3, intro_screen_logo_palette, FADE_PAL_BKG);
    // fade_set_pal(BG_PAL_4, 3, intro_screen_palette, FADE_PAL_BKG);

    // SHOW_BKG;  // TODO: ?? BG1 as BKG and BG0 as WIN?
    SHOW_SPRITES;

    // // To avoid flicker from initial print changing the attrib tiles
    // // print the tiles before fade-in
    // PRINT(4,10, INTRO_TEXT_BLANK, 0);

    // fade_start(FADE_IN);  // TODO

}


/** Generated using Dr LUT - Free Lookup Table Generator
  * https://github.com/ppelikan/drlut
  **/
// Formula: sin(2*pi*t/T) 
// Amp=4, Offs=0, Sz=16 (size should be an even power of 2 for range masking to work)
const int16_t sine[32] = {
    //  0,     1,     2,     2,     3,     3,     4,
    //  4,     4,     4,     4,     3,     3,     2,
    //  2,     1,     0,    -1,    -2,    -2,    -3,
    // -3,    -4,    -4,    -4,    -4,    -4,    -3,
    // -3,    -2,    -2,    -1 };
     0,     1,     1,     2,     2,     2,     3,
     3,     3,     3,     3,     2,     2,     2,
     1,     1,     0,    -1,    -1,    -2,    -2,
    -2,    -3,    -3,    -3,    -3,    -3,    -2,
    -2,    -2,    -1,    -1 };

static void intro_text_sprites_update(void) {

    uint16_t lut_index = intro_counter >> 2;
    // for (uint16_t c = OAM_TEXT_INTRO_START; c <= oam_text_intro_end;) {
    uint16_t c = OAM_TEXT_INTRO_START;
    while (c < oam_text_intro_end) {
        shadow_OAM[c++].y = OAM_TEXT_START_Y + sine[lut_index & (ARRAY_LEN(sine) - 1)]; // Top of char sprite
        shadow_OAM[c++].y = (OAM_TEXT_START_Y + SPR_8x8_HEIGHT) + sine[lut_index & (ARRAY_LEN(sine) - 1)]; // Top of char sprite
        lut_index += 2;
    }
}




void intro_screen_handle(void) {  // TODO: Some kind of animated roll in of logo?

    intro_text_sprites_update();
    intro_clouds_update();
    magic_code_update();

    // Wait for the player to press start
    intro_counter += 2;
}


void intro_screen_cleanup(void) {
    // fade_start(FADE_OUT);    // TODO
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);
    intro_clouds_cleanup();
}