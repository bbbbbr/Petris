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

// board_gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"

#include "common.h"

#include "fade.h"
#include "gfx.h"
#include "gfx_print.h"

#include "player_gfx.h"
#include "player_info.h"
#include "options.h"

#include "../res/intro_screen_tiles.h"
#include "../res/game_board_map.h"
#include "../res/pet_tiles.h"
#include "../res/special_tiles.h"
#include "../res/font_tiles.h"

#define CGB_TILE_SIZE 16
#define ANIM_TAIL_BATCH_SIZE 2
#define TAIL_ANIM_COUNT_LOOP_SIZE 60 // One run of updates every N frames
#define TAIL_ANIM_ALTERNATE_BITS  0x04
#define TAIL_ANIM_FRAMES_PER_SET  4

const UINT8 NEXT_PIECE_BG_TILE = TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG;
const UINT8 NEXT_PIECE_BG_PAL  = BG_PAL_BOARD_NEXT_PIECE_PREVIEW;


// TODO: this could be collapsed into a single set of 4
const UINT8 pet_tail_anim_tilenum[] = {TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 0), // Set 1
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 1),
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 2),
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 3),

                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 0), // Set 2
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 1),
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 2),
                                       TILES_PET_START + (ANIM_TAIL_BATCH_SIZE * 3)
                                   };

// TODO: const UINT16 * ??
const UINT16 pet_tail_anim_srcoffset[] = {
                                      // Regular Tail
                                      (TILE_OFFSET_PET_TAIL_REG + (ANIM_TAIL_BATCH_SIZE * 0)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_REG + (ANIM_TAIL_BATCH_SIZE * 1)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_REG + (ANIM_TAIL_BATCH_SIZE * 2)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_REG + (ANIM_TAIL_BATCH_SIZE * 3)) * CGB_TILE_SIZE,

                                      // Wag Tail
                                      (TILE_OFFSET_PET_TAIL_WAG + (ANIM_TAIL_BATCH_SIZE * 0)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_WAG + (ANIM_TAIL_BATCH_SIZE * 1)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_WAG + (ANIM_TAIL_BATCH_SIZE * 2)) * CGB_TILE_SIZE,
                                      (TILE_OFFSET_PET_TAIL_WAG + (ANIM_TAIL_BATCH_SIZE * 3)) * CGB_TILE_SIZE
                                    };
UINT8 tail_anim_count = 0;
UINT8 tail_anim_alternate = 0;


void board_gfx_init(void) {

    if (option_game_high_contrast == OPTION_HIGH_CONTRAST_OFF) {
        p_pet_tiles = pet_tiles;
        p_pet_palette = (UWORD *)board_pets_palette;
    }
    else { // Implied: OPTION_HIGH_CONTRAST_MED or OPTION_HIGH_CONTRAST_HI
        pet_tiles_hicontrast_prepare();
        p_pet_tiles = pet_tiles_hicontrast_ram;
    }


    board_gfx_init_background();
    board_gfx_init_sprites();

    board_gameover_animate_reset();

    fade_start(FADE_IN);
}


void board_gfx_init_sprites(void) {

    SPRITES_8x8;

    // Uses High Contrast tile set if option is enabled
    fade_set_pal(BG_PAL_0, 5, p_pet_palette, FADE_PAL_SPRITES); // Includes Specials palette as #5

    // Just load first of 4 pals from this -> for printing font to sprites
    fade_set_pal(BG_PAL_5, 1, intro_screen_palette, FADE_PAL_SPRITES);

    set_sprite_data(SPRITE_TILE_PET_START, TILE_COUNT_PETTOTAL, p_pet_tiles);

    // Load special sprite data after pet data
    set_sprite_data(SPRITE_TILE_SPECIAL_START, TILE_COUNT_SPECIALTOTAL, special_tiles);

    // Load overlay font data after sprite data
    set_sprite_data(SPRITE_TILE_FONT_DIGITS_START, TILE_COUNT_FONT_NUMS,
                    font_tiles + (TILES_FONT_CHARS_LEN * CGB_TILE_SIZE));
}


// TODO: rename to board_init_background(); move to board_gfx.c
void board_gfx_init_background(void) {

        //set_bkg_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        //set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

        // Uses High Contrast tile set if option is enabled
        fade_set_pal(BG_PAL_0, 4, p_pet_palette,   FADE_PAL_BKG);
        fade_set_pal(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);

        set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
        // TODO: move fonts to a global shared gfx init: print_gfx_init() ?
        set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);

        set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  p_pet_tiles);

        // Load special sprite data after pet data
        set_bkg_data(TILES_SPECIAL_START,   TILE_COUNT_SPECIALTOTAL, special_tiles);


        // Load BG tile attribute map
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 20, 18, game_board_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 20, 18, game_board_mapPLN0);


        // Set the preview tile area to a white background
        VBK_REG = 1; // Select BG tile attribute map
        set_bkg_tiles(GAME_PIECE_NEXT_PREVIEW_BG_X, GAME_PIECE_NEXT_PREVIEW_BG_Y,
                      1, 1, &NEXT_PIECE_BG_PAL);

        VBK_REG = 0; // Re-Select regular BG tile map
        set_bkg_tiles(GAME_PIECE_NEXT_PREVIEW_BG_X, GAME_PIECE_NEXT_PREVIEW_BG_Y,
                      1, 1, &NEXT_PIECE_BG_TILE);


        // Set up text areas
        PRINT(DISPLAY_NEXT_PIECE_TEXT_X,    DISPLAY_NEXT_PIECE_TEXT_Y - 1,    "NEXT:", 0);

        PRINT(DISPLAY_LEVEL_X,    DISPLAY_LEVEL_Y - 1,    "LEVEL", 0);
         // On same line as level readout
        PRINT(DISPLAY_DIFF_X,     DISPLAY_DIFF_Y,         options_difficulty_abbrev_text_get(), 0);

        PRINT(DISPLAY_SCORE_X,    DISPLAY_SCORE_Y - 1,    "SCORE", 0);
        // Display static trailing zero for score (inflates score apparent value)
        PRINT(DISPLAY_SCORE_X + 4, DISPLAY_SCORE_Y,      "0", 0);

        if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "TAILS", 0);
        } else if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 2,  "PET\nSIZE", 0);
        } else {
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "PETS", 0);
        }


        SHOW_BKG;
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
        tail_anim_alternate ^= TAIL_ANIM_ALTERNATE_BITS;

    }

    // Can't be an else-if since it would miss tail_anim_count = 0
    if (tail_anim_count < TAIL_ANIM_FRAMES_PER_SET) {

        // Note: the OR of tail_anim_alternate only works if it's 8 frames total and one batch
        // otherwise use +
        set_bkg_data(pet_tail_anim_tilenum[tail_anim_count | tail_anim_alternate],
                     ANIM_TAIL_BATCH_SIZE,
                     p_pet_tiles + pet_tail_anim_srcoffset[tail_anim_count | tail_anim_alternate]);
    }
}


// #define GAMEOVER_ANIMATE_RAINBOW_PALS
#define SPR_PAL_PRINT BG_PAL_5

const INT8 SPR_GAMEOVER_CHARS[] = {'G' - 'A' + TILES_FONT_CHARS_START,
                                    'A' - 'A' + TILES_FONT_CHARS_START,
                                    'M' - 'A' + TILES_FONT_CHARS_START,
                                    'E' - 'A' + TILES_FONT_CHARS_START,
                                    'O' - 'A' + TILES_FONT_CHARS_START,
                                    'V' - 'A' + TILES_FONT_CHARS_START,
                                    'E' - 'A' + TILES_FONT_CHARS_START,
                                    'R' - 'A' + TILES_FONT_CHARS_START};

#define SPR_GAMEOVER_MAX_Y ((BRD_ST_Y + 8) * 8)
#define SPR_GAMEOVER_START_X (((BRD_ST_X + 1) * 8) + 2) // 2 Pixels right of left board edge

const UINT8 SPR_GAMEOVER_LUT_X[] = {SPR_GAMEOVER_START_X,
                                    SPR_GAMEOVER_START_X + (8 * 1) + 1, // 1 pixel between each letter
                                    SPR_GAMEOVER_START_X + (8 * 2) + 2,
                                    SPR_GAMEOVER_START_X + (8 * 3) + 3,
                                    // Gap between GANE -- and -- OVER
                                    SPR_GAMEOVER_START_X + (8 * 4) + 9,
                                    SPR_GAMEOVER_START_X + (8 * 5) + 10,
                                    SPR_GAMEOVER_START_X + (8 * 6) + 11,
                                    SPR_GAMEOVER_START_X + (8 * 7) + 12};


// Pre-calculated bounce LUT
// For calculation ref commented out version of board_gameover_animate() below
const INT8 SPR_GAMEOVER_LUT_Y[] = {
    0x01, 0x03, 0x06, 0x0A, 0x0F, 0x15, 0x1C, 0x24,
    0x2D, 0x37, 0x38, 0x31, 0x2B, 0x26, 0x22, 0x1F,
    0x1D, 0x1C, 0x1C, 0x1D, 0x1F, 0x22, 0x26, 0x2B,
    0x31, 0x38, 0x40, 0x3B, 0x37, 0x34, 0x32, 0x31,
    0x31, 0x32, 0x34, 0x37, 0x3B, 0x40, 0x3E, 0x3D,
    0x3D, 0x3E, 0x40, 0x3F, 0x3F, 0x40, 0x40};

#define SPR_GAMEOVER_LUT_Y_MAX (ARRAY_LEN(SPR_GAMEOVER_LUT_Y) - 1)

#define GAMEOVER_UPDATE_MASK 0x03 // Only update cursor once every 8 frames
#define SPR_GAMEOVER_GRAVITY 1
#define SPR_GAMEOVER_LANDED  127

UINT8 spr_gameover_y_idx[SPR_GAMEOVER_COUNT];


// Drop "G A M E   O V E R" letters with a bounce, starting from left to right
void board_gameover_animate(void) {

    UINT8 c;
    UINT8 min_spr = 0; // Used to slowly exit the loop as pieces land
    UINT8 max_spr = 0; // Used for delay launch left to right

    // Loop exits when all sprites have landed
    while (min_spr != SPR_GAMEOVER_COUNT) {

        // Periodic Update
        if ((sys_time & GAMEOVER_UPDATE_MASK) == GAMEOVER_UPDATE_MASK) {

            wait_vbl_done();

            // Delay launch from left to right
            // by adding one more sprite during each animation pass
            if (max_spr < SPR_GAMEOVER_COUNT)
                max_spr++;

            for (c = min_spr; c < max_spr; c++) {

                // Move sprite to next Y LUT position
                spr_gameover_y_idx[c]++;

                // If it's at the end of the LUT then it's landing
                // is complete. Increment the starting sprite past it
                // so that on the next iteration it's excluded
                if (spr_gameover_y_idx[c] >= SPR_GAMEOVER_LUT_Y_MAX) {
                    min_spr++;
                }

                move_sprite(SPR_GAMEOVER_START + c,
                            SPR_GAMEOVER_LUT_X[c],
                            SPR_GAMEOVER_LUT_Y[ spr_gameover_y_idx[c] ]);

            } // for (c = min_spr; c < max_spr; c++) {
        } // if ((sys_time & 0x03) == 0x03)
    } // while (min_spr != SPR_GAMEOVER_COUNT)
}


void board_gameover_animate_reset(void) {

    UINT8 c;

    for (c = 0; c< SPR_GAMEOVER_COUNT; c++) {
        spr_gameover_y_idx[c] = 0; // Reset Y LUT position

        move_sprite(SPR_GAMEOVER_START + c, 0,0);
        set_sprite_tile(SPR_GAMEOVER_START + c, SPR_GAMEOVER_CHARS[c]);

        #ifdef GAMEOVER_ANIMATE_RAINBOW_PALS
            // Use rainbow of pet tile palettes for different letters
            set_sprite_prop(SPR_GAMEOVER_START + c, c & 0x03);
        #else
            // Use solid color palette for all letter
            set_sprite_prop(SPR_GAMEOVER_START + c, SPR_PAL_PRINT);
        #endif

    }
}



// Non-LUT version for reference
/*
// Drop "G A M E   O V E R" letters with a bounce, starting from left to right
void board_gameover_animate(void) {

    UINT8 c;
    UINT8 min_spr = 0; // Used to slowly exit the loop as pieces land
    UINT8 max_spr = 0; // Used for delay launch left to right

    // Loop exits when all sprites have landed
    while (min_spr != SPR_GAMEOVER_COUNT) {


        // Periodic Update
        if ((sys_time & GAMEOVER_UPDATE_MASK) == GAMEOVER_UPDATE_MASK) {

            wait_vbl_done();

            // Delay launch from left to right
            // by adding one more sprite during each animation pass
            if (max_spr < SPR_GAMEOVER_COUNT)
                max_spr++;

            for (c = min_spr; c < max_spr; c++) {

                // Apply gravity to velocity
                // and then velocity to position
                spr_gameover_vel[c] += SPR_GAMEOVER_GRAVITY;
                spr_gameover_y[c] += spr_gameover_vel[c];

                // Bounce back if ground was reached
                if (spr_gameover_y[c] >= SPR_GAMEOVER_MAX_Y) {

                    // Decrease max speed a little on first big bounce
                    if (spr_gameover_vel[c] > 4)
                        spr_gameover_vel[c] -= 2;

                    // Reverse direction
                    // spr_gameover_vel[c] *= -1;
                    spr_gameover_vel[c] = ~spr_gameover_vel[c] + 1; // i.e. *= -1

                    spr_gameover_y[c] = SPR_GAMEOVER_MAX_Y; // Cap max Y

                    // If sprite landed and stopped moving then de-activate it
                    if (spr_gameover_vel[c] == 0) {
                        // Sprites launch one after another from left to right
                        // and land in the same order. So the landed sprite is
                        // always the current left-most (min_spr).
                        // De-activate by excluding it from the loop start
                        min_spr++;
                    }
                }

                move_sprite(SPR_GAMEOVER_START + c,
                            SPR_GAMEOVER_LUT_X[c],
                            spr_gameover_y[c]);

            } // for (c = min_spr; c < max_spr; c++) {
        } // if ((sys_time & 0x03) == 0x03)
    } // while (min_spr != SPR_GAMEOVER_COUNT)
}
*/