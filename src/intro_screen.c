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


#define SPR_PLAYER 0 // Player is sprite "0" // TODO: fix/centralize this

void intro_screen_init(void) {

    move_sprite(SPR_PLAYER, 0,0); // Hide main sprite

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

UINT8 counter; // TODO: share main -> frame_counter

void intro_screen_handle(void) {

// TODO: clean this up

    // Wait for the player to press start
    counter++;

    if (counter == 1) {
        PRINT(5,12, "PRESS START", 0);
    }
    else if (counter == 100) {
        PRINT(5,12, "           ", 0);
    }
    else if (counter == 200) {
        PRINT(5,12, "           ", 0);
        counter = 0;
    }
}