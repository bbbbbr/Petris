// gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_pieces.h"
#include "game_board.h"
#include "gfx.h"


#include "../res/bg_map.h"
#include "../res/bg_tiles.h"
#include "../res/pet_tiles.h"
//#include "../res/font_numeric_tiles.h"
#include "../res/font_tiles.h"

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


void gfx_init (void) {

    gfx_init_background();
    gfx_init_sprites();
}

void gfx_init_sprites (void) {

    SPRITES_8x8;
    set_sprite_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);
}

// TODO: rename to board_init_background(); move to board_gfx.c
void gfx_init_background (void) {

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

        set_bkg_data(TILES_BG_START,        TILE_COUNT_BG,        bg_tiles);
        // set_bkg_data(TILES_FONT_NUMS_START, TILE_COUNT_FONT_NUMS, font_numeric_tiles);
        set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
        set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

        // Load BG tile attribute map
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN0);

        SHOW_BKG;

        DISPLAY_ON;
}


void video_setflip(UINT8 bg_attrib) { // TODO: DEBUG: DELETE ME

    // int x,y;
    // UINT8 video_buff;

    // VBK_REG = 1; // Select BG tile attribute map

    //     for (x=0; x < 20; x++) {
    //         for (y=0; y < 18; y++) {

    //             get_bkg_tiles(x, y, 1, 1, &video_buff);
    //             video_buff ^= bg_attrib;
    //             set_bkg_tiles(x, y, 1, 1, &video_buff);
    //         }
    //     }

    // VBK_REG = 0; // Re-Select regular BG tile map


    #define VBUF_SIZE 18 * 20
    int x;
    UINT8 video_buff[ VBUF_SIZE ];

    VBK_REG = 1; // Select BG tile attribute map

        get_bkg_tiles(0, 0, 20, 18, &video_buff[0]);

        for (x=0; x < VBUF_SIZE; x++) {
            video_buff[x] ^= bg_attrib;
        }

        set_bkg_tiles(0, 0, 20, 18, &video_buff[0]);

    VBK_REG = 0; // Re-Select regular BG tile map

}
