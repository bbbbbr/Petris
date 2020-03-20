// intro_screen.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "input.h"

#include "../res/intro_screen_tiles.h"
#include "../res/intro_screen_map.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


extern const UWORD bgPalette[];

void intro_screen_gfx_init_background (void) {

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

        set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
        set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
        set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

        // Load BG tile attribute map
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 20, 18, intro_screen_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 20, 18, intro_screen_mapPLN0);

        SHOW_BKG;

        DISPLAY_ON;

}


void intro_screen_handle(void) {

    UINT8 counter = 0;

    intro_screen_gfx_init_background();

    // Wait for the player to press start
    waitpadup();
    while (! KEY_TICKED(J_START)) {
        UPDATE_KEYS();
        delay(20);

        // 8 bit rollover is expected, don't check upper bound
        counter++;
        if (counter == 1) {
            PRINT(5,12, "PRESS START", 0);
        }
        else if (counter == 128) {
            PRINT(5,12, "           ", 0);
        }
    }

    waitpadup();
}