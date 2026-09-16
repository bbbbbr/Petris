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

// board_gfx.c

#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>


#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"
#include "gameover_message.h"

#include "common.h"

// #include "fade.h"
#include "gfx.h"
#include "gfx_print.h"

#include "player_gfx.h"
#include "player_info.h"
#include "options.h"


#include "intro_screen_out.h"  // For determining where to start loading font tiles
#include "pet_and_special_tiles_out.h"
#include "font_8x8_nums_pet_colored_out.h"

#define CGB_TILE_SIZE 16
#define ANIM_TAIL_BATCH_SIZE 2
#define TAIL_ANIM_COUNT_LOOP_SIZE 60 // One run of updates every N frames
#define TAIL_ANIM_ALTERNATE_BITS  0x04
#define TAIL_ANIM_FRAMES_PER_SET  4

const uint8_t NEXT_PIECE_BG_TILE = TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG;

uint8_t tail_anim_count = 0;
bool tail_anim_alternate = false;


void board_gfx_init(void) {

    pet_tiles_prepare();
    board_gfx_init_pettiles();

    board_gfx_init_background();
    board_gfx_init_sprites();

    gameover_message_reset();

    // fade_start(FADE_IN);
}


void board_gfx_change_pettiles(void) {

    // Change to next option setting and handle wraparound
    option_game_high_contrast++;
    if (option_game_high_contrast == OPTION_HIGH_CONTRAST_END)
        option_game_high_contrast = OPTION_HIGH_CONTRAST_MIN;

    // Update pet tiles to match new setting
    pet_tiles_prepare();
    board_gfx_init_pettiles();
}

void board_gfx_init_pettiles(void) {

    p_pet_tiles              = pet_and_special_tiles_out_tiles;
    p_special_tiles          = pet_and_special_tiles_out_tiles + TILES_SPECIAL_START;
    p_pet_wag_tiles          = pet_and_special_tiles_out_tiles + TILE_PET_TAIL_WAG_START;
    p_font_pet_colored_tiles = font_8x8_nums_pet_colored_out_tiles;

    // == Background data ==
    // Background Tiles
    #define TILE_LOAD_OFFSET_FONT (intro_screen_out_TILE_COUNT + SHARED_4BPP_TRANSP_TILE_ID_START)
    uint16_t next_free_tile = load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT);

    // Sprite and BG0 palettes for: pet tiles, shared with special pieces and pet font 8x8
    // Set palettes for pet tiles on BG0
    set_bkg_4bpp_palette(PAL_ASSIGN_BG0_3, pet_and_special_tiles_out_PALETTE_COUNT, pet_and_special_tiles_out_palettes);
    // Set palettes for Sprites
    set_bkg_4bpp_palette(PAL_ASSIGN_OBJ_0, pet_and_special_tiles_out_PALETTE_COUNT, pet_and_special_tiles_out_palettes);

    // Shared Sprite and background data:
    // Set at OBJ_TILEGROUP_BASE_512
    // BG:  TILES_PET_START_VRAM_ABSOLUTE
    // OAM: TILES_PET_START_OAM_RELATIVE
    //
    uint16_t tile_id = TILES_PET_START_VRAM_ABSOLUTE;

    set_bkg_4bpp_data(tile_id, pet_and_special_tiles_out_TILE_COUNT, pet_and_special_tiles_out_tiles);
    tile_id  += pet_and_special_tiles_out_TILE_COUNT;

    set_bkg_4bpp_data(tile_id, font_8x8_nums_pet_colored_out_TILE_COUNT, font_8x8_nums_pet_colored_out_tiles);
    tile_id  = font_8x8_nums_pet_colored_out_TILE_COUNT;

        // // Uses High Contrast tile set if option is enabled  // TODO: OPTIONAL: High Contrast init
        // if (game_state == GAME_READY_TO_START)
        //     // fade_set_pal(BG_PAL_0, 4, p_pet_palette,   FADE_PAL_BKG);
        // else
        //     set_bkg_palette(BG_PAL_0, 4, p_pet_palette);


    // See Background loading for sprite tile pattern data loading

        // // Uses High Contrast tile set if option is enabled
        //  if (game_state == GAME_READY_TO_START)
        //      // fade_set_pal(BG_PAL_0, 5, p_pet_palette, FADE_PAL_SPRITES); // Includes Specials palette as #5
        //  else
        //      set_sprite_palette(BG_PAL_0, 5, p_pet_palette); // Includes Specials palette as #5
}


void board_gfx_init_sprites(void) {

    // This is now handled in the common graphics loading since BG and sprites share tiles

    // Just load first of 4 pals from this -> for printing font to sprites
    // fade_set_pal(BG_PAL_5, 1, intro_screen_palette, FADE_PAL_SPRITES);

    // Load overlay font data after sprite data
    // set_sprite_data(SPRITE_TILE_FONT_DIGITS_START, TILE_COUNT_FONT_NUMS,
                    // font_tiles + (TILES_FONT_CHARS_LEN * CGB_TILE_SIZE));
}


void board_gfx_init_background(void) {


    // TODO improve the background for the game play screen
    // - Fill rect some things
    // - Put clouds BEHIND game board if possible

    // Set up text areas
    PRINTXY(DISPLAY_NEXT_PIECE_TEXT_X,    DISPLAY_NEXT_PIECE_TEXT_Y_LABEL,    "NEXT:", 0);

    PRINTXY(DISPLAY_LEVEL_X,    DISPLAY_LEVEL_Y_LABEL,    "LEVEL", 0);
     // On same line as level readout
    PRINTXY(DISPLAY_DIFF_X,     DISPLAY_DIFF_Y,           options_difficulty_abbrev_text_get(), 0);

    PRINTXY(DISPLAY_SCORE_X,    DISPLAY_SCORE_Y_LABEL,    "SCORE", 0);
    // Display static trailing zero for score (inflates score apparent value)
    PRINTXY(DISPLAY_SCORE_X + 4, DISPLAY_SCORE_Y,      "0", 0);

    if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
        PRINTXY(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y_LABEL_LINE2,  "TAILS", 0);
    } else if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
        PRINTXY(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y_LABEL_LINE1,  "PET\nSIZE", 0);
    } else {
        PRINTXY(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y_LABEL_LINE2,  "PETS", 0);
    }

    // SHOW_BKG;
}


// Animate tail wags every N frames
// (Spread the update load across a couple frames)
//
// A run of 4 updates Once/at the start of every 64 frames
// * Each run alternates between different sets of tile data to use
// * One update per frame
// * Each update loads the next batch of ANIM_TAIL_BATCH_SIZE tiles
void board_gfx_tail_animate(void) {

    tail_anim_count++;

    if (tail_anim_count > TAIL_ANIM_COUNT_LOOP_SIZE) {
        tail_anim_count = 0;
        
        // Switch to alternate tail animation tiles
        tail_anim_alternate = !tail_anim_alternate;        
        const uint16_t * p_src = (tail_anim_alternate) ? p_pet_wag_tiles : p_pet_tiles;

        set_bkg_4bpp_data(TILES_PET_START_VRAM_ABSOLUTE, TILE_COUNT_PET_TAIL_WAGS, p_src);
    }
}
