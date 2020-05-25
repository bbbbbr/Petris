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

#include "fade.h"
#include "gfx.h"
#include "gfx_print.h"

#include "player_info.h"
#include "options.h"

#include "../res/intro_screen_tiles.h"
#include "../res/game_board_map.h"
#include "../res/pet_tiles.h"
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

    board_gfx_init_background();
    board_gfx_init_sprites();
    fade_start(FADE_IN);
}


void board_gfx_init_sprites(void) {

    SPRITES_8x8;
    //set_sprite_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    //set_sprite_palette(BG_PAL_4, 1, board_specials_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    fade_set_pal(BG_PAL_0, 4, board_pets_palette,     FADE_PAL_SPRITES);
    fade_set_pal(BG_PAL_4, 1, board_specials_palette, FADE_PAL_SPRITES);
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);
    // Load overlay font data after sprite data
    set_sprite_data(SPRITE_TILE_FONT_DIGITS_START, TILE_COUNT_FONT_NUMS,
                    font_tiles + (TILES_FONT_CHARS_LEN * CGB_TILE_SIZE));
}


// TODO: rename to board_init_background(); move to board_gfx.c
void board_gfx_init_background(void) {

        //set_bkg_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        //set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        fade_set_pal(BG_PAL_0, 4, board_pets_palette,   FADE_PAL_BKG);
        fade_set_pal(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);

        set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
        // TODO: move fonts to a global shared gfx init: print_gfx_init() ?
        set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
        set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

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

        if (option_game_type == OPTION_GAME_TYPE_PET_CLEANUP) {
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 1,  "TAILS", 0);
        } else if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y - 2,  "PET\nSIZE", 0);
            PRINT(DISPLAY_NUMPETS_X,  DISPLAY_NUMPETS_Y + 1,"(  )", 0);
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
                     pet_tiles + pet_tail_anim_srcoffset[tail_anim_count | tail_anim_alternate]);
    }
}
