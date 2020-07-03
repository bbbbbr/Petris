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

#include "../res/pet_tiles.h"
#include "../res/pet_tiles_hicontrast.h"
#include "../res/special_tiles.h"
#include "../res/intro_screen_tiles.h"

// Palettes 0..3
const UWORD board_pets_palette_high_contrast[] = {
    pet_tiles_hicontrastCGBPal0c0, pet_tiles_hicontrastCGBPal0c1, pet_tiles_hicontrastCGBPal0c2, pet_tiles_hicontrastCGBPal0c3, // Pet 0
    pet_tiles_hicontrastCGBPal1c0, pet_tiles_hicontrastCGBPal1c1, pet_tiles_hicontrastCGBPal1c2, pet_tiles_hicontrastCGBPal1c3, // Pet 1
    pet_tiles_hicontrastCGBPal2c0, pet_tiles_hicontrastCGBPal2c1, pet_tiles_hicontrastCGBPal2c2, pet_tiles_hicontrastCGBPal2c3, // Pet 2
    pet_tiles_hicontrastCGBPal3c0, pet_tiles_hicontrastCGBPal3c1, pet_tiles_hicontrastCGBPal3c2, pet_tiles_hicontrastCGBPal3c3, // Pet 3
};

const UWORD board_pets_palette[] = {
    pet_tilesCGBPal0c0, pet_tilesCGBPal0c1, pet_tilesCGBPal0c2, pet_tilesCGBPal0c3, // Pet 0
    pet_tilesCGBPal1c0, pet_tilesCGBPal1c1, pet_tilesCGBPal1c2, pet_tilesCGBPal1c3, // Pet 1
    pet_tilesCGBPal2c0, pet_tilesCGBPal2c1, pet_tilesCGBPal2c2, pet_tilesCGBPal2c3, // Pet 2
    pet_tilesCGBPal3c0, pet_tilesCGBPal3c1, pet_tilesCGBPal3c2, pet_tilesCGBPal3c3, // Pet 3
};

const UWORD board_specials_palette[] = {
    pet_tilesCGBPal4c0, pet_tilesCGBPal4c1, pet_tilesCGBPal4c2, pet_tilesCGBPal4c3, // Specials
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

UINT8 * p_pet_tiles;