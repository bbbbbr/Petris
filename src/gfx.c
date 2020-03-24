// gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "gfx.h"

#include "../res/bg_tiles.h" // For the palette

const UWORD bgPalette[] = {
    bg_tilesCGBPal0c0, bg_tilesCGBPal0c1, bg_tilesCGBPal0c2, bg_tilesCGBPal0c3, // Pet 0
    bg_tilesCGBPal1c0, bg_tilesCGBPal1c1, bg_tilesCGBPal1c2, bg_tilesCGBPal1c3, // Pet 1
    bg_tilesCGBPal2c0, bg_tilesCGBPal2c1, bg_tilesCGBPal2c2, bg_tilesCGBPal2c3, // Pet 2
    bg_tilesCGBPal3c0, bg_tilesCGBPal3c1, bg_tilesCGBPal3c2, bg_tilesCGBPal3c3, // Pet 3
    bg_tilesCGBPal4c0, bg_tilesCGBPal4c1, bg_tilesCGBPal4c2, bg_tilesCGBPal4c3, // Pet Blank
    bg_tilesCGBPal5c0, bg_tilesCGBPal5c1, bg_tilesCGBPal5c2, bg_tilesCGBPal5c3, // BG  0
    bg_tilesCGBPal6c0, bg_tilesCGBPal6c1, bg_tilesCGBPal6c2, bg_tilesCGBPal6c3,
    bg_tilesCGBPal7c0, bg_tilesCGBPal7c1, bg_tilesCGBPal7c2, bg_tilesCGBPal7c3,
};
