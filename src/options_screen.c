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

#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


#define SPR_OPTIONS_CURSOR 0 // Cursor is sprite "0"


// TODO: these are hardwired for convenience right now
#define PET_DOG_HEAD             3
const UINT8 tile_pet_dog_torso = (TILES_PET_START + 1);
const UINT8 tile_pet_dog_tail = (TILES_PET_START + 0);
const UINT8 tile_pet_dog_head = (TILES_PET_START + 3);
const UINT8 tile_blank = (TILES_PET_START + GP_EMPTY);
// const UINT8 attirb_none = 0x00;
const UINT8 attirb_pal_1  = BG_PAL_1;
const UINT8 attirb_pal_1_flip_x = BG_PAL_1 | BG_FLIP_X;

// GP_EMPTY + TILES_PET_START
#define OPTION_MENU_X 3

#define OPTION_CURSOR_X (2 * 8)


enum {
    OPTION_MENU_MIN = 0,
    OPTION_MENU_TYPE = OPTION_MENU_MIN,
    OPTION_MENU_LEVEL,
    OPTION_MENU_STARTGAME,
    OPTION_MENU_MAX = OPTION_MENU_STARTGAME
} OPTION_MENU_ENTRIES;

const UINT8 option_menu_y[] = {4, 6, 10};
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


void options_screen_draw(void) {

    UINT8 x,y;

    for (y = SCREEN_MIN_Y; y <= SCREEN_MAX_Y; y++) {
        for (x = SCREEN_MIN_X; x <= SCREEN_MAX_X; x++) {

            // Clear BG tile attribute map
            VBK_REG = 1;
            set_bkg_tiles(x, y, 1, 1, &attirb_pal_1);

            // Select BG tile map
            VBK_REG = 0;

            // Draw a border of pet tiles, regular tiles in the middle
            if (y == SCREEN_MIN_Y) {
                set_bkg_tiles(x, y, 1, 1, &tile_pet_dog_torso);

            } else if (y == SCREEN_MAX_Y) {
                set_bkg_tiles(x, y, 1, 1, &tile_pet_dog_torso);

            } else {
                set_bkg_tiles(x, y, 1, 1, &tile_blank);
            }

        } // end x loop
    } // end x loop

    // Clear BG tile attribute map
    VBK_REG = 1;
    set_bkg_tiles(19,  0, 1,1, &attirb_pal_1_flip_x);
    set_bkg_tiles(19, 17, 1,1, &attirb_pal_1_flip_x);

    // Select BG tile map
    VBK_REG = 0;

    set_bkg_tiles(0,   0, 1,1, &tile_pet_dog_head);
    set_bkg_tiles(0,  17, 1,1, &tile_pet_dog_head);
    set_bkg_tiles(19,  0, 1,1, &tile_pet_dog_tail);
    set_bkg_tiles(19, 17, 1,1, &tile_pet_dog_tail);


    PRINT(6,2, "OPTIONS", 0);
}


void options_screen_sprites_init(void) {

    SPRITES_8x8;

    set_sprite_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);

    set_sprite_tile(SPR_OPTIONS_CURSOR, PET_DOG_HEAD);
    set_sprite_prop(SPR_OPTIONS_CURSOR, BG_FLIP_X); // TODO: should prob set palette here

    SHOW_SPRITES;
}


void options_screen_exit_cleanup(void) {

    HIDE_SPRITES;

}


void options_screen_init(void) {

    set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data

//        set_bkg_data(TILES_OPTIONS_START,     TILE_COUNT_OPTIONS,     options_screen_tiles);
    set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
    set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

    // // Load BG tile attribute map
    // VBK_REG = 1;
    // set_bkg_tiles(0, 0, 20, 18, options_screen_mapPLN1);

    // // Load BG tile map
    // VBK_REG = 0;
    // set_bkg_tiles(0, 0, 20, 18, options_screen_mapPLN0);

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