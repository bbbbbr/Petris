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

// intro_screen.c

#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

#include "gfx.h"
#include "gfx_print.h"
#include "common.h"
#include "sound.h"
#include "input.h"
#include "fade.h"

#include "magic_code.h"

// #include "player_gfx.h"

#include "intro_screen.h"

#include "intro_screen_out.h"
#include "intro_logo_out.h"

#include "intro_clouds_dark_out.h"
#include "intro_clouds_light_out.h"


// #include "../res/pet_tiles.h"
// #include "../res/font_tiles.h"


#define LOGO_TILE_Y_START 4u

#define STR_INTRO_TEXT_START      "START GAME"
#define STR_INTRO_TEXT_HIGHSCORES "HIGHSCORES"
#define STR_INTRO_TEXT_CREDITS    " CREDITS"

#define OAM_TEXT_SPR_IDS_START 0u
#define OAM_TEXT_START_X       88u
#define OAM_TEXT_START_Y       116u
#define OAM_TEXT_GAP_Y         ((FONT_8x16_TILE_HEIGHT * SPR_8x8_HEIGHT) + 4u)
#define OAM_TEXT_HIGHSCORES_Y  (OAM_TEXT_START_Y + OAM_TEXT_GAP_Y)
#define OAM_TEXT_CREDITS_Y     (OAM_TEXT_HIGHSCORES_Y + OAM_TEXT_GAP_Y)

#define MENU_PAL_HIGHLIGHT     (PRINT_PAL_OAM_YELLOW)
#define MENU_PAL_NORMAL        (PRINT_PAL_OAM_GREY)

static uint8_t        intro_menu_id_active;
static uint16_t       menu_text_spr_ids[INTRO_MENU_ID_COUNT + 1u];  // Trailing entryu
static const uint8_t  menu_text_spr_y[INTRO_MENU_ID_COUNT]      = {OAM_TEXT_START_Y,     OAM_TEXT_HIGHSCORES_Y,     OAM_TEXT_CREDITS_Y};
static const char *   menu_items_text[INTRO_MENU_ID_COUNT]      = {STR_INTRO_TEXT_START, STR_INTRO_TEXT_HIGHSCORES, STR_INTRO_TEXT_CREDITS};
static const uint16_t menu_next_game_state[INTRO_MENU_ID_COUNT] = {GAME_OPTIONS_INIT,    GAME_HIGHSCORES_INIT,      /* Credits entry is nerfed until implemented */ GAME_INTRO };

static uint16_t sprite_counter = 0;
static uint16_t cloud_counter = 0;


static void intro_background_and_logo_init(void);
static void intro_text_init(void);
static void intro_clouds_init(void);

static void intro_text_reset(uint8_t menu_index);
static void intro_text_update(void);
static void intro_clouds_update(void);

static void intro_background_and_logo_cleanup(void);
static void intro_clouds_cleanup(void);
static void intro_text_cleanup(void);


static void intro_background_and_logo_init(void) {

    uint16_t base_tile_id = SHARED_4BPP_TRANSP_TILE_ID_START;   // TODO: Maybe a global shared next_free_bg_tile var with add_tiles and remove_tiles
    // Load the globally shared (for convenience) transparent tile
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_4bpp_data(base_tile_id, SHARED_4BPP_TRANSP_TILE_COUNT, shared_4bpp_transparent_tile);
    base_tile_id += SHARED_4BPP_TRANSP_TILE_COUNT;

    // Main background on BG1 layer
    // BG1 is set to use pals 4,5,6,7 (so no need to offset palette)
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);

    set_fade_target_4bpp_palette(PAL_ASSIGN_BG1_0, intro_screen_out_PALETTE_COUNT, intro_screen_out_palettes);
    set_bkg_4bpp_data(base_tile_id, intro_screen_out_TILE_COUNT, intro_screen_out_tiles);
    set_bkg_tilemap_base_address(BG1_MAP_START());
    set_bkg_based_tiles((DEVICE_SCREEN_WIDTH - intro_screen_out_TILES_WIDTH)/2,  // Tile centered X
                        (DEVICE_SCREEN_HEIGHT - intro_screen_out_TILES_HEIGHT)/2,  // Tile centered Y
                        intro_screen_out_TILES_WIDTH, intro_screen_out_TILES_HEIGHT, intro_screen_out_map, base_tile_id);
    base_tile_id += intro_screen_out_TILE_COUNT;


    // Petris Logo is now on BG0 so it can be scrolled over background in BG1

    // Logo background on BG0 layer so it can scroll
    // BG0 is set to use pals 0,1,2,3
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);

    set_fade_target_4bpp_palette(PAL_ASSIGN_BG0_0, intro_logo_out_PALETTE_COUNT, intro_logo_out_palettes);
    set_bkg_4bpp_data(base_tile_id, intro_logo_out_TILE_COUNT, intro_logo_out_tiles);
    // First fill entire BG1 tilemap with clear tiles to ensure
    // BG0 to shows through in transparent parts
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, SHARED_4BPP_TRANSP_TILE_ID_START);
    set_bkg_based_tiles((DEVICE_SCREEN_WIDTH - intro_logo_out_TILES_WIDTH)/2,  // Tile centered X
                        LOGO_TILE_Y_START,
                        intro_logo_out_TILES_WIDTH, intro_logo_out_TILES_HEIGHT, intro_logo_out_map, base_tile_id);
    base_tile_id += intro_logo_out_TILE_COUNT;

    // Scroll logo down the screen a bit to intended position
    VDP.BG_SCROLL[BG0_SCROLL_Y] = (uint16_t)-8;  // TODO: Animated scroll in of Petris logo?
}


// Translucent blended clouds overlay for intro screen
static void intro_clouds_init(void) {

    // Bitmap BM1/BM2 cloud layers on Screen B, Math ADD + BLEND
    SHOW_SCREEN_B;
    cloud_counter = 0;

    // Set up shared palette (from one of the images)
    set_fade_target_4bpp_palette(PAL_ASSIGN_BM_0, 1, intro_clouds_light_out_palette);  // 1 Palette (actually 3 colors at present, but allow it to overrun to 16)

    // Load both sets of cloud bitmaps to the shared VRAM, one at top and other lower
    // Make the lighter clouds on BM2 so that they scroll over the darker ones
    VDP.BM_SCROLLY[BM2_SCROLL_Y] = (uint16_t)-16u;
    load_bitmap_4bpp(0, 0u, intro_clouds_light_out_PX_WIDTH, intro_clouds_light_out_PX_HEIGHT, intro_clouds_light_out);

    VDP.BM_SCROLLY[BM3_SCROLL_Y] = (uint16_t)(-16u + DEVICE_SCREEN_PX_HEIGHT);
    load_bitmap_4bpp(0, DEVICE_SCREEN_PX_HEIGHT, intro_clouds_dark_out_PX_WIDTH, intro_clouds_dark_out_PX_HEIGHT, intro_clouds_dark_out);
}


// Animated "Press Start" message using Sprites
static void intro_text_init(void) {

    intro_menu_id_active = INTRO_MENU_ID_DEFAULT;
    sprite_counter = 0;

    // Load the 8x16 font and print it to the sprites
    #define TILE_LOAD_OFFSET_FONT_VRAM_ABS     (OBJ_TILEGROUP_BASE_512 + 0u)
    #define TILE_LOAD_OFFSET_FONT_VRAM_OAM_REL (0u)
    load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT_VRAM_ABS, TILE_LOAD_OFFSET_FONT_VRAM_OAM_REL);

    load_8x16_font_sprite_palettes();

    // Draw the menu
    menu_text_spr_ids[INTRO_MENU_ID_MIN] = OAM_TEXT_SPR_IDS_START;
    for (uint8_t idx = INTRO_MENU_ID_MIN; idx <= INTRO_MENU_ID_MAX; idx++) {
        PRINT_POS(OAM_TEXT_START_X, menu_text_spr_y[idx]);
        // Next oam index after printed menu item is saved as start for the next one
        // These values are later used to select the oam indexes to animate per menu item
        menu_text_spr_ids[idx + 1u] = print_to_sprites(menu_text_spr_ids[idx], MENU_PAL_NORMAL, menu_items_text[idx]);
    }

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
// Amp=3, Offs=0, Sz=16 (size should be an even power of 2 for range masking to work)
const int16_t sine[32] = {
     0,     1,     1,     2,     2,     2,     3,
     3,     3,     3,     3,     2,     2,     2,
     1,     1,     0,    -1,    -1,    -2,    -2,
    -2,    -3,    -3,    -3,    -3,    -3,    -2,
    -2,    -2,    -1,    -1 };


// Resets text to static position with no sine wave
static void intro_text_gradual_remove_sine_wiggle(uint8_t menu_index) {

    // Update menu text sprites every 8 frames so they gradually
    // settle back into position instead of snapping back immediately
    if ((sprite_counter & 0x07u) == 0u) {
        uint16_t oam_id     = menu_text_spr_ids[menu_index];
        uint16_t oam_id_end = menu_text_spr_ids[menu_index + 1u];
        // Set the sprite colors back to the normal menu color
        uint8_t pal = MENU_PAL_NORMAL;

        while (oam_id < oam_id_end) {
            // If Y position is above or below default position for the menu entry
            // then increment it by 1 in the relevant direction (same for top and bottom)
            uint8_t spr_y = menu_text_spr_y[menu_index];

            // Top 8x8 sprite of letter
            int8_t mv_y = (shadow_OAM[oam_id].y > spr_y) ? -1 : ((shadow_OAM[oam_id].y < spr_y) ? 1 : 0);
            set_sprite_prop(oam_id, S_8x8 | pal);
            scroll_sprite(oam_id++, 0, mv_y);
            // Bottom 8x8 sprite of letter
            set_sprite_prop(oam_id, S_8x8 | pal);
            scroll_sprite(oam_id++, 0, mv_y);
        }
    }
}


// Updates current menu entry text with slow sine wave
static void intro_text_sine_wiggle(uint8_t menu_index) {

    uint16_t lut_index = sprite_counter >> 1;
    uint16_t oam_id     = menu_text_spr_ids[menu_index];
    uint16_t oam_id_end = menu_text_spr_ids[menu_index + 1u];
    // Set the sprite colors to the highlight menu color
    uint8_t pal = MENU_PAL_HIGHLIGHT;

    while (oam_id < oam_id_end) {
        uint8_t spr_y = menu_text_spr_y[menu_index] + sine[lut_index & (ARRAY_LEN(sine) - 1)];
        // Top 8x8 sprite of letter        
        set_sprite_prop(oam_id, S_8x8 | pal);
        move_sprite_y(oam_id++, spr_y);
        // Bottom 8x8 sprite of letter
        set_sprite_prop(oam_id, S_8x8 | pal);
        move_sprite_y(oam_id++, spr_y + SPR_8x8_HEIGHT);
        lut_index += 2;
    }
}


// Updates animation for all menu entries
static void intro_text_update(void) {

    sprite_counter++;
    for (uint8_t c = INTRO_MENU_ID_MIN; c <= INTRO_MENU_ID_MAX; c++) {
        if (c == intro_menu_id_active) intro_text_sine_wiggle(c);
        else                           intro_text_gradual_remove_sine_wiggle(c);
    }
}


static void intro_background_and_logo_cleanup(void) {

    // TODO: could do a bounce out of the logo

    // Logo background on BG0 layer so it can scroll
    // BG0 is set to use pals 0,1,2,3
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, SHARED_4BPP_TRANSP_TILE_ID_START);
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

    // SHOW_BKG;  // TODO: ?? BG1 as BKG and BG0 as WIN?
    SHOW_SPRITES;

    palette_fade_start(FADE_TO_TARGET);
}


void intro_screen_handle(void) {  // TODO: Some kind of animated roll in of logo?

    // Wait for the player to press start
    intro_text_update();
    intro_clouds_update();
    magic_code_update();

    // Check for switching active menu item (and reset previous if needed)
    if (KEY_TICKED(J_UP)) {
        if (intro_menu_id_active > INTRO_MENU_ID_MIN) intro_menu_id_active--;
    }
    else if (KEY_TICKED(J_DOWN)) {
        if (intro_menu_id_active < INTRO_MENU_ID_MAX) intro_menu_id_active++;
    }
    else if (KEY_TICKED(J_START | J_A)) {
        // If a menu item was selected, change to the associated game state
        game_state = menu_next_game_state[intro_menu_id_active];
    }
}


void intro_screen_cleanup(void) {

    palette_fade_start(FADE_TO_BLACK);
    // Keep animations running until fade-out completes
    while (palette_fade_is_running()) {
        intro_screen_handle();
        vsync();
    }

    intro_background_and_logo_cleanup();
    intro_text_cleanup();
    intro_clouds_cleanup();
}