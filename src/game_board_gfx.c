// board_gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "gfx.h"

#include "game_pieces.h"
#include "game_board.h"
#include "game_board_gfx.h"

#include "../res/bg_map.h"
#include "../res/bg_tiles.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


void board_gfx_init(void) {

    board_gfx_init_background();
    board_gfx_init_sprites();
}


void board_gfx_init_sprites(void) {

    SPRITES_8x8;
    set_sprite_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);
}


// TODO: rename to board_init_background(); move to board_gfx.c
void board_gfx_init_background(void) {

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

        set_bkg_data(TILES_BG_START,        TILE_COUNT_BG,        bg_tiles);
        // TODO: move fonts to a global shared gfx init: print_gfx_init() ?
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
