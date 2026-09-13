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

// gfx.c

#include <gbdk/platform.h>


#include "common.h"
#include "gfx.h"
#include "options.h"


// Pointer used to load pet tiles/palettes,
// allows tile sets/palettes to be easily swapped out
uint16_t * p_pet_tiles = NULL;
uint16_t * p_special_tiles = NULL;

// uint16_t * p_pet_palette;

void pet_tiles_prepare(void) {

    // p_pet_tiles = pet_tiles; // TODO
    // p_pet_palette = (uint16_t *)board_pets_palette;  // TODO
}
