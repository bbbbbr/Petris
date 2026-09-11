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

#include "intro_clouds_dark_out.h"
#include "intro_clouds_light_out.h"


// #include "../res/pet_tiles.h"
// #include "../res/font_tiles.h"


#define LOGO_TILE_Y_START 5u

#define STR_INTRO_TEXT_START "PRESS START"
#define OAM_TEXT_INTRO_START 0u
#define OAM_TEXT_START_X  80u
#define OAM_TEXT_START_Y  136u

static uint16_t oam_text_intro_end = 0;


static uint16_t sprite_counter = 0;
static uint16_t cloud_counter = 0;


static void intro_background_and_logo_init(void);
static void intro_text_init(void);
static void intro_clouds_init(void);

static void intro_text_update(void);
static void intro_clouds_update(void);

static void intro_background_and_logo_cleanup(void);
static void intro_clouds_cleanup(void);
static void intro_text_cleanup(void);


static void intro_background_and_logo_init(void) {
    
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

    // Scroll logo down the screen a bit to intended position
    VDP.BG_SCROLL[BG0_SCROLL_Y] = (uint16_t)-8;  // TODO: Animated scroll in of Petris logo?    
}


// Translucent blended clouds overlay for intro screen
static void intro_clouds_init(void) {

    // Bitmap BM1/BM2 cloud layers on Screen B, Math ADD + BLEND
    SHOW_SCREEN_B;
    cloud_counter = 0;

    // Set up shared palette (from one of the images)
    set_bkg_4bpp_palette(PAL_12, 1, intro_clouds_light_out_palette);  // 1 Palette (actually 3 colors at present, but allow it to overrun to 16)

    // Load both sets of cloud bitmaps to the shared VRAM, one at top and other lower
    // Make the lighter clouds on BM2 so that they scroll over the darker ones
    VDP.BM_SCROLLY[BM2_SCROLL_Y] = (uint16_t)-16u;
    load_bitmap_4bpp(0, 0u, intro_clouds_light_out_PX_WIDTH, intro_clouds_light_out_PX_HEIGHT, intro_clouds_light_out);

    VDP.BM_SCROLLY[BM3_SCROLL_Y] = (uint16_t)(-16u + DEVICE_SCREEN_PX_HEIGHT);
    load_bitmap_4bpp(0, DEVICE_SCREEN_PX_HEIGHT, intro_clouds_dark_out_PX_WIDTH, intro_clouds_dark_out_PX_HEIGHT, intro_clouds_dark_out);
}


// Animated "Press Start" message using Sprites
static void intro_text_init(void) {

    sprite_counter = 0;

    // Load the 8x16 font and print it to the sprites
    #define TILE_LOAD_OFFSET_FONT (OBJ_TILEGROUP_BASE_512)
    load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT);
    load_8x16_font_sprite_palettes();

    PRINT_POS(OAM_TEXT_START_X, OAM_TEXT_START_Y);
    oam_text_intro_end = print_to_sprites(OAM_TEXT_INTRO_START, PRINT_PAL_OAM_GREY, STR_INTRO_TEXT_START);
    intro_text_update();
}


void intro_clouds_update(void) {
    cloud_counter += 3;
    VDP.BM_SCROLLX[BM2_SCROLL_X] = cloud_counter >> 2;  // Lighter Clouds
    VDP.BM_SCROLLX[BM3_SCROLL_X] = cloud_counter >> 3;  // Darker Clouds

}


// Generated using Dr LUT - Free Lookup Table Generator
// https://github.com/ppelikan/drlut
//
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

static void intro_text_update(void) {

    sprite_counter++;

    uint16_t lut_index = sprite_counter >> 1;
    uint16_t c = OAM_TEXT_INTRO_START;
    while (c < oam_text_intro_end) {
        shadow_OAM[c++].y = OAM_TEXT_START_Y + sine[lut_index & (ARRAY_LEN(sine) - 1)]; // Top of char sprite
        shadow_OAM[c++].y = (OAM_TEXT_START_Y + SPR_8x8_HEIGHT) + sine[lut_index & (ARRAY_LEN(sine) - 1)]; // Top of char sprite
        lut_index += 2;
    }
}


static void intro_background_and_logo_cleanup(void) {

    // TODO: could do a bounce out of the logo

    // Logo background on BG0 layer so it can scroll
    // BG0 is set to use pals 0,1,2,3
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, TILE_LOAD_OFFSET_LOGO);
    VDP.BG_SCROLL[BG0_SCROLL_Y] = 0;
}


static void intro_clouds_cleanup(void) {
    // Hide the clouds
    HIDE_SCREEN_B;
}


static void intro_text_cleanup(void) {
    // Hide sprites used for text
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);
}



void intro_screen_init(void) {

    intro_background_and_logo_init();
    intro_clouds_init();
    intro_text_init();

    // fade_set_pal(BG_PAL_0, 3, intro_screen_logo_palette, FADE_PAL_BKG);
    // fade_set_pal(BG_PAL_4, 3, intro_screen_palette, FADE_PAL_BKG);

    // SHOW_BKG;  // TODO: ?? BG1 as BKG and BG0 as WIN?
    SHOW_SPRITES;

    // fade_start(FADE_IN);  // TODO
}


void intro_screen_handle(void) {  // TODO: Some kind of animated roll in of logo?

    // Wait for the player to press start
    intro_text_update();
    intro_clouds_update();
    magic_code_update();
}


void intro_screen_cleanup(void) {
    // fade_start(FADE_OUT);    // TODO
    intro_background_and_logo_cleanup();
    intro_text_cleanup();
    intro_clouds_cleanup();
}