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

#include <gbdk/platform.h>

#include "common.h"
#include "platform_compat.h"
#include "gfx.h"
#include "options.h"

#include "../res/pet_tiles.h"
#include "../res/pet_tiles_hicontrast.h"
#include "../res/pet_tiles_hicontrast2.h"
#include "../res/pet_tiles_medcontrast.h"
#include "../res/special_tiles.h"
#include "../res/intro_screen_tiles.h"
#include "../res/intro_cat_tiles.h"



// On Game Board Palettes 0 - 3 are used for the Pet Tile Game Board Pieces
// Palettes 0..3
const UWORD board_pets_pal_high_contrast[] = {
    CGB2PAL(pet_tiles_hicontrastCGBPal0c0), CGB2PAL(pet_tiles_hicontrastCGBPal0c1), CGB2PAL(pet_tiles_hicontrastCGBPal0c2), CGB2PAL(pet_tiles_hicontrastCGBPal0c3), // Pet 0
    CGB2PAL(pet_tiles_hicontrastCGBPal1c0), CGB2PAL(pet_tiles_hicontrastCGBPal1c1), CGB2PAL(pet_tiles_hicontrastCGBPal1c2), CGB2PAL(pet_tiles_hicontrastCGBPal1c3), // Pet 1
    CGB2PAL(pet_tiles_hicontrastCGBPal2c0), CGB2PAL(pet_tiles_hicontrastCGBPal2c1), CGB2PAL(pet_tiles_hicontrastCGBPal2c2), CGB2PAL(pet_tiles_hicontrastCGBPal2c3), // Pet 2
    CGB2PAL(pet_tiles_hicontrastCGBPal3c0), CGB2PAL(pet_tiles_hicontrastCGBPal3c1), CGB2PAL(pet_tiles_hicontrastCGBPal3c2), CGB2PAL(pet_tiles_hicontrastCGBPal3c3), // Pet 3

    CGB2PAL(pet_tiles_hicontrastCGBPal4c0), CGB2PAL(pet_tiles_hicontrastCGBPal4c1), CGB2PAL(pet_tiles_hicontrastCGBPal4c2), CGB2PAL(pet_tiles_hicontrastCGBPal4c3), // Specials
};

const UWORD board_pets_pal_high_contrast_2[] = {
    CGB2PAL(pet_tiles_hicontras2CGBPal0c0), CGB2PAL(pet_tiles_hicontras2CGBPal0c1), CGB2PAL(pet_tiles_hicontras2CGBPal0c2), CGB2PAL(pet_tiles_hicontras2CGBPal0c3), // Pet 0
    CGB2PAL(pet_tiles_hicontras2CGBPal1c0), CGB2PAL(pet_tiles_hicontras2CGBPal1c1), CGB2PAL(pet_tiles_hicontras2CGBPal1c2), CGB2PAL(pet_tiles_hicontras2CGBPal1c3), // Pet 1
    CGB2PAL(pet_tiles_hicontras2CGBPal2c0), CGB2PAL(pet_tiles_hicontras2CGBPal2c1), CGB2PAL(pet_tiles_hicontras2CGBPal2c2), CGB2PAL(pet_tiles_hicontras2CGBPal2c3), // Pet 2
    CGB2PAL(pet_tiles_hicontras2CGBPal3c0), CGB2PAL(pet_tiles_hicontras2CGBPal3c1), CGB2PAL(pet_tiles_hicontras2CGBPal3c2), CGB2PAL(pet_tiles_hicontras2CGBPal3c3), // Pet 3

    CGB2PAL(pet_tiles_hicontrastCGBPal4c0), CGB2PAL(pet_tiles_hicontrastCGBPal4c1), CGB2PAL(pet_tiles_hicontrastCGBPal4c2), CGB2PAL(pet_tiles_hicontrastCGBPal4c3), // Specials
};


// On Game Board Palettes 0 - 3 are used for the Pet Tile Game Board Pieces
// Palettes 0..3
const UWORD board_pets_pal_med_contrast[] = {
    CGB2PAL(pet_tiles_mdcontrastCGBPal0c0), CGB2PAL(pet_tiles_mdcontrastCGBPal0c1), CGB2PAL(pet_tiles_mdcontrastCGBPal0c2), CGB2PAL(pet_tiles_mdcontrastCGBPal0c3), // Pet 0
    CGB2PAL(pet_tiles_mdcontrastCGBPal1c0), CGB2PAL(pet_tiles_mdcontrastCGBPal1c1), CGB2PAL(pet_tiles_mdcontrastCGBPal1c2), CGB2PAL(pet_tiles_mdcontrastCGBPal1c3), // Pet 1
    CGB2PAL(pet_tiles_mdcontrastCGBPal2c0), CGB2PAL(pet_tiles_mdcontrastCGBPal2c1), CGB2PAL(pet_tiles_mdcontrastCGBPal2c2), CGB2PAL(pet_tiles_mdcontrastCGBPal2c3), // Pet 2
    CGB2PAL(pet_tiles_mdcontrastCGBPal3c0), CGB2PAL(pet_tiles_mdcontrastCGBPal3c1), CGB2PAL(pet_tiles_mdcontrastCGBPal3c2), CGB2PAL(pet_tiles_mdcontrastCGBPal3c3), // Pet 3

    CGB2PAL(pet_tiles_mdcontrastCGBPal4c0), CGB2PAL(pet_tiles_mdcontrastCGBPal4c1), CGB2PAL(pet_tiles_mdcontrastCGBPal4c2), CGB2PAL(pet_tiles_mdcontrastCGBPal4c3), // Specials
};

const UWORD board_pets_palette[] = {
    CGB2PAL(pet_tilesCGBPal4c0), CGB2PAL(pet_tilesCGBPal4c1), CGB2PAL(pet_tilesCGBPal4c2), CGB2PAL(pet_tilesCGBPal4c3), // Pet 0
    CGB2PAL(pet_tilesCGBPal5c0), CGB2PAL(pet_tilesCGBPal5c1), CGB2PAL(pet_tilesCGBPal5c2), CGB2PAL(pet_tilesCGBPal5c3), // Pet 1
    CGB2PAL(pet_tilesCGBPal6c0), CGB2PAL(pet_tilesCGBPal6c1), CGB2PAL(pet_tilesCGBPal6c2), CGB2PAL(pet_tilesCGBPal6c3), // Pet 2
    CGB2PAL(pet_tilesCGBPal7c0), CGB2PAL(pet_tilesCGBPal7c1), CGB2PAL(pet_tilesCGBPal7c2), CGB2PAL(pet_tilesCGBPal7c3), // Pet 3

    // CGB2PAL(pet_tilesCGBPal4c0), CGB2PAL(pet_tilesCGBPal4c1), CGB2PAL(pet_tilesCGBPal4c2), CGB2PAL(pet_tilesCGBPal4c3), // Specials
};

const UWORD clouds_sprite_palette[] = {
    CGB2PAL(intro_screen_tilesCGBPal7c0), CGB2PAL(intro_screen_tilesCGBPal7c1), CGB2PAL(intro_screen_tilesCGBPal7c2), intro_screen_tilesCGBPal7c3,
};

// Mostly the same as default text pal (intro screen .4) except lighter final color
const UWORD option_title_palette[] = {
    CGB2PAL(intro_screen_tilesCGBPal4c0), CGB2PAL(intro_screen_tilesCGBPal4c1), CGB2PAL(intro_screen_tilesCGBPal4c2), RGB8(90, 90, 90)  // Lighter text color (CGB2PAL(B), CGB2PAL(G), R)
    // ((90 >> 3) <<10) | ((90 >> 3) << 5) | (90 >> 3) // Lighter text color (CGB2PAL(B), CGB2PAL(G), R)
};

// Loaded as Palettes 4..7 BG / SPRITES
const UWORD intro_screen_logo_palette[] = {
    CGB2PAL(intro_screen_tilesCGBPal4c0), CGB2PAL(intro_screen_tilesCGBPal4c1), CGB2PAL(intro_screen_tilesCGBPal4c2), CGB2PAL(intro_screen_tilesCGBPal4c3), // Logo Top
    CGB2PAL(intro_screen_tilesCGBPal5c0), CGB2PAL(intro_screen_tilesCGBPal5c1), CGB2PAL(intro_screen_tilesCGBPal5c2), CGB2PAL(intro_screen_tilesCGBPal5c3), // Logo Bottom
    CGB2PAL(intro_screen_tilesCGBPal6c0), CGB2PAL(intro_screen_tilesCGBPal6c1), CGB2PAL(intro_screen_tilesCGBPal6c2), CGB2PAL(intro_screen_tilesCGBPal6c3), // Logo Bottom
// For clouds:
    CGB2PAL(intro_screen_tilesCGBPal3c0), CGB2PAL(intro_screen_tilesCGBPal3c1), CGB2PAL(intro_screen_tilesCGBPal3c2), CGB2PAL(intro_screen_tilesCGBPal3c3)
};

// Loaded as Palettes 0..3
const UWORD intro_screen_palette[] = {
    CGB2PAL(intro_screen_tilesCGBPal0c0), CGB2PAL(intro_screen_tilesCGBPal0c1), CGB2PAL(intro_screen_tilesCGBPal0c2), CGB2PAL(intro_screen_tilesCGBPal0c3), // Sky
    CGB2PAL(intro_screen_tilesCGBPal1c0), CGB2PAL(intro_screen_tilesCGBPal1c1), CGB2PAL(intro_screen_tilesCGBPal1c2), CGB2PAL(intro_screen_tilesCGBPal1c3), // Ground Top
    CGB2PAL(intro_screen_tilesCGBPal2c0), CGB2PAL(intro_screen_tilesCGBPal2c1), CGB2PAL(intro_screen_tilesCGBPal2c2), CGB2PAL(intro_screen_tilesCGBPal2c3), // Ground Bottom
    CGB2PAL(intro_screen_tilesCGBPal3c0), CGB2PAL(intro_screen_tilesCGBPal3c1), CGB2PAL(intro_screen_tilesCGBPal3c2), CGB2PAL(intro_screen_tilesCGBPal3c3)
};

// Intro Splash screen only uses Palette 1
const UWORD intro_cat_palette[] = {
    CGB2PAL(intro_cat_tilesCGBPal0c0), CGB2PAL(intro_cat_tilesCGBPal0c1), CGB2PAL(intro_cat_tilesCGBPal0c2), intro_cat_tilesCGBPal0c3
};



// Pointer used to load pet tiles/palettes,
// allows tile sets/palettes to be easily swapped out
const UINT8 * p_pet_tiles;
UWORD * p_pet_palette;

// Storage for expanded hi contrast pet tiles
// This isn't the most efficient way to do this, but this RAM is otherwise unused
UINT8 pet_tiles_hicontrast_ram[ TILE_SIZE_BYTES * (TILE_COUNT_PETTOTAL + TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__) ];
UINT8 pet_tiles_hicontrast_pals_ram[ (TILE_COUNT_PETTOTAL + TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__) ];

#define TILE_COPY_COUNT 4                // Duplicate each tile 4 times
#define TILE_SIZE_EXPANDED_BYTES         (TILE_SIZE_BYTES * TILE_COPY_COUNT)
#define TILE_INDEX_PETS_HICONTRAST_BLANK (TILE_COUNT_PETS / TILE_COPY_COUNT) // Blank tile is right after pet tiles
// Blank tile doesn't get CGB2PAL(repeated), all the rest do by a factor of TILE_COPY_COUNT
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
void pet_tiles_prepare() {

    const UINT8 * p_src_tile;
    UINT8 * p_dest;
    UINT8 src_tile;
    UINT8 c;
    UINT8 duplicate, repeat_num;


    if (option_game_high_contrast == OPTION_HIGH_CONTRAST_OFF) {
        p_pet_tiles = pet_tiles;
        p_pet_palette = (UWORD *)board_pets_palette;
    }
    else { // Implied: OPTION_HIGH_CONTRAST_MED or OPTION_HIGH_CONTRAST_HI/2

        p_pet_tiles = pet_tiles_hicontrast_ram;


        // Non-expanded source CODE tile data
        if (option_game_high_contrast == OPTION_HIGH_CONTRAST_HI) {
            p_src_tile = pet_tiles_hicontrast;
            p_pet_palette = (UWORD *) board_pets_pal_high_contrast;
        } else if (option_game_high_contrast == OPTION_HIGH_CONTRAST_HI_2) {
            p_src_tile = pet_tiles_hicontrast;
            p_pet_palette = (UWORD *) board_pets_pal_high_contrast_2;
        }
        else { // implied (option_game_high_contrast == OPTION_HIGH_CONTRAST_MED)
            p_src_tile = pet_tiles_mdcontrast;
            p_pet_palette = (UWORD *) board_pets_pal_med_contrast;
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

                // Copy the tile one byte at a time
                // Could optimize this with DMA / etc
                // but it seems fast enough as is
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

}

