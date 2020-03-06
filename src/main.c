#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "../res/bg_map.h"
#include "../res/bg_tiles.h"


// These palettes replace the default palette with the appropriate colors. See cgb.h for more defined colors.

UWORD bgPalette[] = {
    bg_tilesCGBPal0c0, bg_tilesCGBPal0c1, bg_tilesCGBPal0c2, bg_tilesCGBPal0c3,
    bg_tilesCGBPal1c0, bg_tilesCGBPal1c1, bg_tilesCGBPal1c2, bg_tilesCGBPal1c3,
    bg_tilesCGBPal2c0, bg_tilesCGBPal2c1, bg_tilesCGBPal2c2, bg_tilesCGBPal2c3,
    bg_tilesCGBPal3c0, bg_tilesCGBPal3c1, bg_tilesCGBPal3c2, bg_tilesCGBPal3c3,
    bg_tilesCGBPal4c0, bg_tilesCGBPal4c1, bg_tilesCGBPal4c2, bg_tilesCGBPal4c3,
    bg_tilesCGBPal5c0, bg_tilesCGBPal5c1, bg_tilesCGBPal5c2, bg_tilesCGBPal5c3,
    bg_tilesCGBPal6c0, bg_tilesCGBPal6c1, bg_tilesCGBPal6c2, bg_tilesCGBPal6c3,
    bg_tilesCGBPal7c0, bg_tilesCGBPal7c1, bg_tilesCGBPal7c2, bg_tilesCGBPal7c3,
};

void main(void){

    // Check for CGB
    // REG_A != 0x11
//    if (HARDWARE_TYPE != 0x80U) {

        // Handle Non-CGB
//    }
    //else
    {
        // Switch CGB fast mode
        cpu_fast();
        SPRITES_8x8;

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        set_bkg_data(0, 63, bg_tiles);

//        // Load BG tile attribute map
//        VBK_REG = 1;
//        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN1);
//
//        // Load BG tile map
//        VBK_REG = 0;
//        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN0);
//
//        SHOW_BKG;

        // Load BG tile attribute map
        VBK_REG = 1;
        set_win_tiles(0, 0, 20, 18, bg_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_win_tiles(0, 0, 20, 18, bg_mapPLN0);

        WX_REG = 7;
        WY_REG = 0;
        SHOW_WIN;

        DISPLAY_ON;
    }
}