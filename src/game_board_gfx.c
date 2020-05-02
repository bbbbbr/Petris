// board_gfx.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_piece.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "game_board_gfx.h"
#include "fade.h"
#include "gfx.h"

#include "../res/intro_screen_tiles.h"
#include "../res/game_board_map.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


void board_gfx_init(void) {

    board_gfx_init_background();
    board_gfx_init_sprites();
    fade_start(FADE_IN);
}


void board_gfx_init_sprites(void) {

    SPRITES_8x8;
    //set_sprite_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    //set_sprite_palette(BG_PAL_4, 1, board_specials_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    fade_set_pal(BG_PAL_0, 4, board_pets_palette,     FADE_PAL_SPRITES);
    fade_set_pal(BG_PAL_4, 1, board_specials_palette, FADE_PAL_SPRITES);
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);
}


// TODO: rename to board_init_background(); move to board_gfx.c
void board_gfx_init_background(void) {

        //set_bkg_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        //set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        fade_set_pal(BG_PAL_0, 4, board_pets_palette,   FADE_PAL_BKG);
        fade_set_pal(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);

        set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
        // TODO: move fonts to a global shared gfx init: print_gfx_init() ?
        set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
        set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

        // Load BG tile attribute map
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 20, 18, game_board_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 20, 18, game_board_mapPLN0);

        SHOW_BKG;
}
