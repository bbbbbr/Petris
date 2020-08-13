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

// gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "common.h"
#include "gfx.h"
#include "options.h"

#include "../res/pet_tiles.h"
#include "../res/pet_tiles_hicontrast.h"
#include "../res/pet_tiles_medcontrast.h"
#include "../res/special_tiles.h"
#include "../res/intro_screen_tiles.h"
#include "../res/intro_cat_tiles.h"

// On Game Board Palettes 0 - 3 are used for the Pet Tile Game Board Pieces
// Palettes 0..3
const UWORD board_pets_palette_high_contrast[] = {
    pet_tiles_hicontrastCGBPal0c0, pet_tiles_hicontrastCGBPal0c1, pet_tiles_hicontrastCGBPal0c2, pet_tiles_hicontrastCGBPal0c3, // Pet 0
    pet_tiles_hicontrastCGBPal1c0, pet_tiles_hicontrastCGBPal1c1, pet_tiles_hicontrastCGBPal1c2, pet_tiles_hicontrastCGBPal1c3, // Pet 1
    pet_tiles_hicontrastCGBPal2c0, pet_tiles_hicontrastCGBPal2c1, pet_tiles_hicontrastCGBPal2c2, pet_tiles_hicontrastCGBPal2c3, // Pet 2
    pet_tiles_hicontrastCGBPal3c0, pet_tiles_hicontrastCGBPal3c1, pet_tiles_hicontrastCGBPal3c2, pet_tiles_hicontrastCGBPal3c3, // Pet 3

    pet_tiles_hicontrastCGBPal4c0, pet_tiles_hicontrastCGBPal4c1, pet_tiles_hicontrastCGBPal4c2, pet_tiles_hicontrastCGBPal4c3, // Specials
};

// On Game Board Palettes 0 - 3 are used for the Pet Tile Game Board Pieces
// Palettes 0..3
const UWORD board_pets_palette_med_contrast[] = {
    pet_tiles_mdcontrastCGBPal0c0, pet_tiles_mdcontrastCGBPal0c1, pet_tiles_mdcontrastCGBPal0c2, pet_tiles_mdcontrastCGBPal0c3, // Pet 0
    pet_tiles_mdcontrastCGBPal1c0, pet_tiles_mdcontrastCGBPal1c1, pet_tiles_mdcontrastCGBPal1c2, pet_tiles_mdcontrastCGBPal1c3, // Pet 1
    pet_tiles_mdcontrastCGBPal2c0, pet_tiles_mdcontrastCGBPal2c1, pet_tiles_mdcontrastCGBPal2c2, pet_tiles_mdcontrastCGBPal2c3, // Pet 2
    pet_tiles_mdcontrastCGBPal3c0, pet_tiles_mdcontrastCGBPal3c1, pet_tiles_mdcontrastCGBPal3c2, pet_tiles_mdcontrastCGBPal3c3, // Pet 3

    pet_tiles_mdcontrastCGBPal4c0, pet_tiles_mdcontrastCGBPal4c1, pet_tiles_mdcontrastCGBPal4c2, pet_tiles_mdcontrastCGBPal4c3, // Specials
};

const UWORD board_pets_palette[] = {
    pet_tilesCGBPal0c0, pet_tilesCGBPal0c1, pet_tilesCGBPal0c2, pet_tilesCGBPal0c3, // Pet 0
    pet_tilesCGBPal1c0, pet_tilesCGBPal1c1, pet_tilesCGBPal1c2, pet_tilesCGBPal1c3, // Pet 1
    pet_tilesCGBPal2c0, pet_tilesCGBPal2c1, pet_tilesCGBPal2c2, pet_tilesCGBPal2c3, // Pet 2
    pet_tilesCGBPal3c0, pet_tilesCGBPal3c1, pet_tilesCGBPal3c2, pet_tilesCGBPal3c3, // Pet 3

    pet_tilesCGBPal4c0, pet_tilesCGBPal4c1, pet_tilesCGBPal4c2, pet_tilesCGBPal4c3, // Specials
};

const UWORD clouds_sprite_palette[] = {
    intro_screen_tilesCGBPal7c0, intro_screen_tilesCGBPal7c1, intro_screen_tilesCGBPal7c2, intro_screen_tilesCGBPal7c3,
};

// Mostly the same as default text pal (intro screen .4) except lighter final color
const UWORD option_title_palette[] = {
    intro_screen_tilesCGBPal4c0,
    intro_screen_tilesCGBPal4c1,
    intro_screen_tilesCGBPal4c2,
    ((90 >> 3) <<10) | ((90 >> 3) << 5) | (90 >> 3) // Lighter text color (B, G, R)
};

// Palettes 0..1
const UWORD intro_screen_logo_palette[] = {
    intro_screen_tilesCGBPal0c0, intro_screen_tilesCGBPal0c1, intro_screen_tilesCGBPal0c2, intro_screen_tilesCGBPal0c3, // Logo Top
    intro_screen_tilesCGBPal1c0, intro_screen_tilesCGBPal1c1, intro_screen_tilesCGBPal1c2, intro_screen_tilesCGBPal1c3, // Logo Bottom
    intro_screen_tilesCGBPal2c0, intro_screen_tilesCGBPal2c1, intro_screen_tilesCGBPal2c2, intro_screen_tilesCGBPal2c3, // Logo Bottom
};

// Palettes 4..6
const UWORD intro_screen_palette[] = {
    intro_screen_tilesCGBPal4c0, intro_screen_tilesCGBPal4c1, intro_screen_tilesCGBPal4c2, intro_screen_tilesCGBPal4c3, // Sky
    intro_screen_tilesCGBPal5c0, intro_screen_tilesCGBPal5c1, intro_screen_tilesCGBPal5c2, intro_screen_tilesCGBPal5c3, // Ground Top
    intro_screen_tilesCGBPal6c0, intro_screen_tilesCGBPal6c1, intro_screen_tilesCGBPal6c2, intro_screen_tilesCGBPal6c3, // Ground Bottom
    intro_screen_tilesCGBPal7c0, intro_screen_tilesCGBPal7c1, intro_screen_tilesCGBPal7c2, intro_screen_tilesCGBPal7c3,
};

// Intro Splash screen only uses Palette 1
const UWORD intro_cat_palette[] = {
    intro_cat_tilesCGBPal0c0, intro_cat_tilesCGBPal0c1, intro_cat_tilesCGBPal0c2, intro_cat_tilesCGBPal0c3
};


// Pointer used to load pet tiles/palettes,
// allows tile sets/palettes to be easily swapped out
UINT8 * p_pet_tiles;
UWORD * p_pet_palette;

// Storage for expanded hi contrast pet tiles
// This isn't the most efficient way to do this, but this RAM is otherwise unused
UINT8 pet_tiles_hicontrast_ram[ TILE_SIZE_BYTES * (TILE_COUNT_PETTOTAL + TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__)];

#define TILE_COPY_COUNT 4         // Duplicate each tile 4 times
#define TILE_SIZE_EXPANDED_BYTES  (TILE_SIZE_BYTES * TILE_COPY_COUNT)
#define TILE_INDEX_PETS_HICONTRAST_BLANK (TILE_COUNT_PETS / TILE_COPY_COUNT) // Blank tile is right after pet tiles
// Blank tile doesn't get repeated, all the rest do by a factor of TILE_COPY_COUNT
#define TILE_COUNT_PETS_HICONTRAST_SRC   (TILE_COUNT_PETS / TILE_COPY_COUNT) + \
                                          TILE_COUNT_PETBLANK + \
                                         (TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__ / TILE_COPY_COUNT)

// High contrast pet tiles can be deduplicated
// because all the different pets have the same
// body segment shape, and differ only in color.
// (Unlike the standard full color and shaped pets)
//
// Expand deduplicated high contrast
//   pet tiles from CODE into RAM
// And select matching color palette from CODE
void pet_tiles_hicontrast_prepare() {

    UINT8 * p_src_tile;
    UINT8 * p_dest;
    UINT8 src_tile;
    UINT8 c;
    UINT8 duplicate, repeat_num;

    // Non-expanded source CODE tile data
    if (option_game_high_contrast == OPTION_HIGH_CONTRAST_HI) {
        p_src_tile = pet_tiles_hicontrast;
        p_pet_palette = (UWORD *) board_pets_palette_high_contrast;
    }
    else { // implied (option_game_high_contrast == OPTION_HIGH_CONTRAST_MED)
        p_src_tile = pet_tiles_mdcontrast;
        p_pet_palette = (UWORD *) board_pets_palette_med_contrast;
    }


    // Start of expanded RAM tile array
    p_dest = pet_tiles_hicontrast_ram;

    src_tile = 0;

    // Loop through all deduplicated tiles
    while (src_tile < TILE_COUNT_PETS_HICONTRAST_SRC) {

        if (src_tile != TILE_INDEX_PETS_HICONTRAST_BLANK)
            repeat_num = 4;
        else
            repeat_num = 1; // Don't expand blank tile

        // Copy the tile from CODE to RAM N times
        for (duplicate = 0; duplicate < repeat_num; duplicate++) {

            // Copy the tile one bytes at a time
            // TODO: OPTIONAL: could optimize this with DMA / etc
            //       (seems fast enough as is though)
            for (c = 0; c < TILE_SIZE_BYTES; c++) {
                *p_dest++ = *p_src_tile++;
            }

            // Rewind to the start of the source tile
            p_src_tile -= TILE_SIZE_BYTES;
        }

        // Move to next source tile
        p_src_tile += TILE_SIZE_BYTES;
        src_tile++;
    }


}

