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

// intro_screen.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "input.h"
#include "fade.h"

#include "player_gfx.h"

#include "../res/intro_screen_tiles.h"
#include "../res/intro_screen_map.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"



#define SPR_PAL_CLOUDS         BG_PAL_4
#define SPR_TILE_COUNT_CLOUDS  TILE_COUNT_CLOUDS
#define SPR_CLOUDS_START       0

#define CLOUD_LEN_1 5
#define CLOUD_LEN_2 3
#define CLOUD_LEN_3 4
#define CLOUD_LEN_4 5
#define CLOUD_LEN_5 5

const UINT8 SPR_CLOUDS_LEN[]   = {CLOUD_LEN_1,
                                  CLOUD_LEN_2,
                                  CLOUD_LEN_3,
                                  CLOUD_LEN_4,
                                  CLOUD_LEN_5}; // Must match SPR_CLOUDS_TILES entries
#define SPR_CLOUD_COUNT_ALL   (CLOUD_LEN_1 + CLOUD_LEN_2 + CLOUD_LEN_3 + CLOUD_LEN_4 + CLOUD_LEN_5)
#define SPR_CLOUD_MAX         (SPR_CLOUDS_START + SPR_CLOUD_COUNT_ALL - 1)



// Convert to 2D arrays if there is enough processor time left
#define SPR_CLOUD_START 0
#define SPR_CLOUD_COUNT 3
      UINT8 spr_clouds_x[]     = {20,28,36,44,52, // Start x is first in row, + 8 for each tile
                                  80,88,96,
                                  120,128,136,144,
                                  90,98,106,114,122,
                                  50,58,66,74,82};

const UINT8 SPR_CLOUDS_Y[]     = {30,30,30,30,30,
                                  42,42,42,
                                  37,37,37,37,
                                  71,71,71,71,71,
                                  67,67,67,67,67};

const UINT8 SPR_CLOUDS_VEL[]   = {3,3,3,3,3,
                                  1,1,1,
                                  2,2,2,2,
                                  5,5,5,5,5,
                                  2,2,2,2,2};

const UINT8 SPR_CLOUDS_TILES[] = {0,1,2,3,4, // Cloud 1
                                  5,7,8,     // Cloud 2
                                  5,6,7,8,   // Cloud 3
                                  5,6,6,7,8, // Cloud 4
                                  0,1,3,1,4};//0,1,2,3,4};  // Cloud 5

void intro_clouds_init(void) {

    UINT8 c;

    // Sprites
    SPRITES_8x8;
    fade_set_pal(BG_PAL_4, 1, clouds_sprite_palette, FADE_PAL_SPRITES);
    set_sprite_data(SPR_CLOUDS_START, SPR_TILE_COUNT_CLOUDS, intro_screen_tiles + TILE_OFFSET_CLOUDS);

    // Loop through all cloud sprites
    for(c = SPR_CLOUD_START; c <= SPR_CLOUD_MAX; c++) {

        set_sprite_tile(c, SPR_CLOUDS_TILES[c]);
        set_sprite_prop(c, SPR_PAL_CLOUDS);

        // Ensure sprite is off-screen to start
        move_sprite(c, 0,0);
    }

}

void intro_clouds_update(void) {

    UINT8 c;

    // // Flicker clouds off/on every other frame
    // if (sys_time & 0x01)
    //     HIDE_SPRITES;
    // else
    //     SHOW_SPRITES;

    // Loop through all cloud sprites
    for(c = 0; c <= SPR_CLOUD_MAX; c++) {

        // Move each cloud left every X out of Y frames
        if ((SPR_CLOUDS_VEL[c] == 1) || (sys_time & SPR_CLOUDS_VEL[c])) {

            spr_clouds_x[c]--;

            // Wrap cloud sprites around if the reach the left edge
            if (spr_clouds_x[c] == 0)
                spr_clouds_x[c]+= 168;

            move_sprite(c, spr_clouds_x[c], SPR_CLOUDS_Y[c]);
        }
    }
}


void intro_clouds_cleanup(void) {

    UINT8 c;

    // Loop through all cloud sprites
    for(c = 0; c <= SPR_CLOUD_MAX; c++) {

        move_sprite(c, 0,0);
    }

}


void intro_screen_init(void) {

    intro_clouds_init();

    // set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    fade_set_pal(BG_PAL_0, 3, intro_screen_logo_palette, FADE_PAL_BKG);
    fade_set_pal(BG_PAL_4, 3, intro_screen_palette, FADE_PAL_BKG);

    set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
    set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
    set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

    // TODO: OPTIMIZE: Might be possible to save a lot of space here
    //       by rendering the arrtib in code instead of from consts
    // Load BG tile attribute map
    VBK_REG = 1;
    set_bkg_tiles(0, 0, 20, 18, intro_screen_mapPLN1);

    // Load BG tile map
    VBK_REG = 0;
    set_bkg_tiles(0, 0, 20, 18, intro_screen_mapPLN0);

    SHOW_BKG;
    SHOW_SPRITES;

    fade_start(FADE_IN);

}

UINT8 counter; // TODO: share main -> frame_counter



void intro_screen_handle(void) {

    intro_clouds_update();

    // TODO: convert to global frame counter or sys_time
    // Wait for the player to press start
    counter++;

    if (counter == 1) {
        PRINT(4,10, "PRESS  START", 0);
    }
    else if (counter == 125) {
        PRINT(4,10, "            ", 0);
    }
    else if (counter == 200) {
        counter = 0;
    }
}