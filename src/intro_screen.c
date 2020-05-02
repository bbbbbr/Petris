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


void intro_screen_init(void) {

//    move_sprite(SPR_PLAYER, 0,0); // Hide main sprite // TODO: should be ok to remove this if other screens do proper cleanup on exit

//    set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

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

SetFadePalBkg(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);
FadeColor(FADE_IN);

}

UINT8 counter; // TODO: share main -> frame_counter



void intro_screen_handle(void) {

// TODO: clean this up

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