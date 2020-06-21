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
#ifdef GFX_HIGH_CONTRAST
    #include "../res/pet_tiles_hc.h"
#endif

#include "../res/intro_screen_tiles.h"

// Palettes 0..3
#ifdef GFX_HIGH_CONTRAST
const UWORD board_pets_palette[] = {
    pet_tiles_hcCGBPal0c0, pet_tiles_hcCGBPal0c1, pet_tiles_hcCGBPal0c2, pet_tiles_hcCGBPal0c3, // Pet 0
    pet_tiles_hcCGBPal1c0, pet_tiles_hcCGBPal1c1, pet_tiles_hcCGBPal1c2, pet_tiles_hcCGBPal1c3, // Pet 1
    pet_tiles_hcCGBPal2c0, pet_tiles_hcCGBPal2c1, pet_tiles_hcCGBPal2c2, pet_tiles_hcCGBPal2c3, // Pet 2
    pet_tiles_hcCGBPal3c0, pet_tiles_hcCGBPal3c1, pet_tiles_hcCGBPal3c2, pet_tiles_hcCGBPal3c3, // Pet 3
};
#else
const UWORD board_pets_palette[] = {
    pet_tilesCGBPal0c0, pet_tilesCGBPal0c1, pet_tilesCGBPal0c2, pet_tilesCGBPal0c3, // Pet 0
    pet_tilesCGBPal1c0, pet_tilesCGBPal1c1, pet_tilesCGBPal1c2, pet_tilesCGBPal1c3, // Pet 1
    pet_tilesCGBPal2c0, pet_tilesCGBPal2c1, pet_tilesCGBPal2c2, pet_tilesCGBPal2c3, // Pet 2
    pet_tilesCGBPal3c0, pet_tilesCGBPal3c1, pet_tilesCGBPal3c2, pet_tilesCGBPal3c3, // Pet 3
};
#endif

const UWORD board_specials_palette[] = {
    pet_tilesCGBPal4c0, pet_tilesCGBPal4c1, pet_tilesCGBPal4c2, pet_tilesCGBPal4c3, // Specials
};



// Palettes 4..7
const UWORD intro_screen_palette[] = {
    intro_screen_tilesCGBPal4c0, intro_screen_tilesCGBPal4c1, intro_screen_tilesCGBPal4c2, intro_screen_tilesCGBPal4c3, // Pet Blank
    intro_screen_tilesCGBPal5c0, intro_screen_tilesCGBPal5c1, intro_screen_tilesCGBPal5c2, intro_screen_tilesCGBPal5c3, // BG  0
    intro_screen_tilesCGBPal6c0, intro_screen_tilesCGBPal6c1, intro_screen_tilesCGBPal6c2, intro_screen_tilesCGBPal6c3,
    intro_screen_tilesCGBPal7c0, intro_screen_tilesCGBPal7c1, intro_screen_tilesCGBPal7c2, intro_screen_tilesCGBPal7c3,
};
