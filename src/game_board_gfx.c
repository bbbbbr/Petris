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

#include "../res/intro_screen_tiles.h"
#include "../res/game_board_map.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


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

        SHOW_BKG;
}

#define CGB_TILE_SIZE 16
#define ANIM_TAIL_BATCH_SIZE 2

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
                                          // 0 * CGB_TILE_SIZE, // Regular Tail
                                          // 2 * CGB_TILE_SIZE,
                                          // 4 * CGB_TILE_SIZE,
                                          // 6 * CGB_TILE_SIZE,

                                          // 48 * CGB_TILE_SIZE, //
                                          // 50 * CGB_TILE_SIZE,
                                          // 52 * CGB_TILE_SIZE,
                                          // 54 * CGB_TILE_SIZE
                                         };


#define ANIM_FRAME_ALTERNATE_BITS 0x40 // (0x08 << 3)
#define ANIM_FRAME_MATCH          0x20 // (0x08 << 2)
#define ANIM_FRAME_MASK           0x3C // (0x0F << 2)
#define ANIM_COUNT_ALT_BITS       0x04 // (0x04)
#define ANIM_COUNT_MASK           0x03 // (0x03)

// #define ANIM_FRAME_ALTERNATE_BITS 0x20 // (0x04 << 3)
// #define ANIM_FRAME_MATCH          0x10 // (0x04 << 2)
// #define ANIM_FRAME_MASK           0x1C // (0x07 << 2)
// #define ANIM_COUNT_ALT_BITS       0x04 // (0x04)
// #define ANIM_COUNT_MASK           0x03 // (0x03)

// Animate tail wags every N frames
//
// A run of (0 -> 3) Once every 64 frames
//
// Alternating between .0 -> .1 and .1 -> .2 every *other* 32 frames using .6)
//
// 8 7|6 5 4 3|2 1
//    |- - - -|
//   o *      |* *| Tail Normal / Even
//   x *    |* *|   Tail Wagged / Off
//
void board_gfx_tail_animate(UINT8 frame_count) {

    UINT8 anim;

    if ((frame_count & ANIM_FRAME_MASK) == ANIM_FRAME_MATCH) {

        anim = frame_count & ANIM_COUNT_MASK;

        // alternate high/low every other batch
        if (frame_count & ANIM_FRAME_ALTERNATE_BITS)
            anim |= ANIM_COUNT_ALT_BITS;

        set_bkg_data(pet_tail_anim_tilenum[anim],
                     ANIM_TAIL_BATCH_SIZE,
                     pet_tiles + pet_tail_anim_srcoffset[anim]);
    }
    // if ((frame_count & 0x3C) == 0x20) {

    //     anim = frame_count & 0x07;

    //     if (anim <= 0x03) {

    //         // alternate high/low every other batch
    //         if (frame_count & 0x40)
    //             anim = anim |= 0x04;

    //         set_bkg_data(pet_tail_anim_tilenum[anim], 2,
    //                      pet_tiles + pet_tail_anim_srcoffset[anim]);
    //     }
    // }
}
