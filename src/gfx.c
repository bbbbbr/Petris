// gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "gfx.h"

#include "../res/pet_tiles.h"
#include "../res/intro_screen_tiles.h"

// Palettes 0..3
const UWORD board_pets_palette[] = {
    pet_tilesCGBPal0c0, pet_tilesCGBPal0c1, pet_tilesCGBPal0c2, pet_tilesCGBPal0c3, // Pet 0
    pet_tilesCGBPal1c0, pet_tilesCGBPal1c1, pet_tilesCGBPal1c2, pet_tilesCGBPal1c3, // Pet 1
    pet_tilesCGBPal2c0, pet_tilesCGBPal2c1, pet_tilesCGBPal2c2, pet_tilesCGBPal2c3, // Pet 2
    pet_tilesCGBPal3c0, pet_tilesCGBPal3c1, pet_tilesCGBPal3c2, pet_tilesCGBPal3c3, // Pet 3
};


// Palettes 4..7
const UWORD intro_screen_palette[] = {
    intro_screen_tilesCGBPal4c0, intro_screen_tilesCGBPal4c1, intro_screen_tilesCGBPal4c2, intro_screen_tilesCGBPal4c3, // Pet Blank
    intro_screen_tilesCGBPal5c0, intro_screen_tilesCGBPal5c1, intro_screen_tilesCGBPal5c2, intro_screen_tilesCGBPal5c3, // BG  0
    intro_screen_tilesCGBPal6c0, intro_screen_tilesCGBPal6c1, intro_screen_tilesCGBPal6c2, intro_screen_tilesCGBPal6c3,
    intro_screen_tilesCGBPal7c0, intro_screen_tilesCGBPal7c1, intro_screen_tilesCGBPal7c2, intro_screen_tilesCGBPal7c3,
};
