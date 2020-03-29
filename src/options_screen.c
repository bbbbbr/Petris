// options_screen.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>


#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "input.h"
#include "common.h"

#include "game_piece_data.h"

#include "../res/intro_screen_tiles.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


#define SPR_OPTIONS_CURSOR 0 // Cursor is sprite "0"
#define PET_DOG_HEAD       ((GP_PET_DOG << GP_PET_UPSHIFT) | (GP_SEG_HEAD << GP_SEG_UPSHIFT))


const UINT8 options_screen_tiles[] = {TILES_INTRO_START + 0,
                                      TILES_INTRO_START + 1,
                                      TILES_INTRO_START + 2,
                                      TILES_INTRO_START + 3};
// const UINT8 attirb_none = 0x00;
const UINT8 options_screen_attrib_pal = BG_PAL_4;

#define OPTION_MENU_X 3

#define OPTION_CURSOR_X (2 * 8)


enum {
    OPTION_MENU_MIN = 0,
    OPTION_MENU_TYPE = OPTION_MENU_MIN,
    OPTION_MENU_LEVEL,
    OPTION_MENU_STARTGAME,
    OPTION_MENU_MAX = OPTION_MENU_STARTGAME
} OPTION_MENU_ENTRIES;

const UINT8 option_menu_y[] = {6, 8, 12};
const char * option_menu_text[] = {"TYPE:   MARATHON",
                                  "LEVEL:  NORMAL  ",
                                  "  START GAME    "};

INT8 options_menu_index = 0;


void options_screen_cursor_update(INT8 dir) {

    // Play a sound when moving the cursor
    if (dir != 0)
        PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x14, 0x94, 0x86);

    // Update menu selection
    options_menu_index += dir;

    if (options_menu_index < OPTION_MENU_MIN)
        options_menu_index = OPTION_MENU_MAX;

    else if (options_menu_index > OPTION_MENU_MAX)
        options_menu_index = OPTION_MENU_MIN;

    move_sprite(SPR_OPTIONS_CURSOR,
                OPTION_CURSOR_X,
                (option_menu_y[options_menu_index] + 2) * 8);
}


void options_screen_setting_draw(void) {

    UINT8 c;
    for (c = OPTION_MENU_MIN; c <= OPTION_MENU_MAX; c++) {
        PRINT(OPTION_MENU_X,option_menu_y[c],  option_menu_text[c], 0);
    }
}


// TODO: OPTIMIZE: rendering this to a global, shared, temp array would be much faster global_bg_scratch_tilemap[], global_bg_scratch_attribmap[]
void options_screen_draw(void) {

    UINT8 x,y;

    for (y = SCREEN_MIN_Y; y <= SCREEN_MAX_Y; y++) {
        for (x = SCREEN_MIN_X; x <= SCREEN_MAX_X; x++) {

            // Clear BG tile attribute map
            VBK_REG = 1;
            set_bkg_tiles(x, y, 1, 1, &options_screen_attrib_pal);

            // Select BG tile map
            VBK_REG = 0;

            // Draw a border of pet tiles, regular tiles in the middle
            if (y <= 2) {
                set_bkg_tiles(x, y, 1, 1, &options_screen_tiles[y]);

            } else {
                set_bkg_tiles(x, y, 1, 1, &options_screen_tiles[3]);
            }

        } // end x loop
    } // end x loop


    PRINT(6,4, "OPTIONS", 0);
}


void options_screen_sprites_init(void) {

    SPRITES_8x8;

    set_sprite_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);

    set_sprite_tile(SPR_OPTIONS_CURSOR, PET_DOG_HEAD);
    set_sprite_prop(SPR_OPTIONS_CURSOR, BG_FLIP_X); // TODO: should prob set palette here

    SHOW_SPRITES;
}


void options_screen_exit_cleanup(void) {

    HIDE_SPRITES;

}


void options_screen_init(void) {

    set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

    set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
    set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
    set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

    SHOW_BKG;

    options_menu_index = OPTION_MENU_STARTGAME;

    options_screen_sprites_init();
    options_screen_cursor_update(0);

    options_screen_draw();
    options_screen_setting_draw();
}



void options_screen_handle(void) {

    if (KEY_TICKED(J_UP)) {

        options_screen_cursor_update(-1);

    } else if (KEY_TICKED(J_DOWN)) {

        options_screen_cursor_update(1);

    } else if (KEY_TICKED(J_START | J_A | J_B)) {

        if (options_menu_index == OPTION_MENU_STARTGAME)

            options_screen_exit_cleanup();
            game_state = GAME_READY_TO_START;
    }

}