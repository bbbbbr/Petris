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

// options_screen.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>


#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "input.h"
#include "common.h"
#include "fade.h"

#include "game_piece_data.h"

#include "options.h"
#include "options_screen.h"

#include "../res/intro_screen_tiles.h"
#include "../res/pet_tiles.h"
#include "../res/font_tiles.h"


#define SPR_OPTIONS_CURSOR 0 // Cursor is sprite "0"
#define PET_DOG_HEAD       ((GP_PET_DOG << GP_PET_UPSHIFT) | (GP_SEG_HEAD << GP_SEG_UPSHIFT))

#define OPTION_MENU_X 3
#define OPTION_CURSOR_X (2 * 8)


const UINT8 options_screen_attrib_pal = BG_PAL_4;
const UINT8 options_screen_tiles[] = {TILES_INTRO_START + 0,
                                      TILES_INTRO_START + 1,
                                      TILES_INTRO_START + 2,
                                      TILES_INTRO_START + 3};

enum {
    OPTION_MENU_MIN = 0,

    OPTION_MENU_TYPE = OPTION_MENU_MIN,
    OPTION_MENU_LEVEL,
    OPTION_MENU_VISUAL_HINTS,
    OPTION_MENU_STARTGAME,

    OPTION_MENU_MAX = OPTION_MENU_STARTGAME
} option_menu_entries;


// Trailing spaces are to clear out previous option value text
const char * options_type[]         = {"MARATHON    ", "LEVEL UP   ", "PET CLEANUP", "LONG PET   ", "COLLECT    "}; // Must match : option_game_type_entries
const char * options_difficulty[]   = {"EASY  ", "NORMAL", "HARD  ", "EXPERT ", "BEAST "}; // Must match : option_difficulty_entries
const char * options_visual_hints[] = {"ON ", "OFF"}; // Must match : option_visual_hints_entries


typedef struct opt_item {

    UINT8          menu_y;      // Y position of option menu entry
    const char *   label;       // Option name

    INT8           opt_entries; // Size of option values array
    const char * * p_text_arr;  // Array of option text strings / values
    INT8 *         p_curval;    // Pointer to (extern) option variable

} option_item;


const option_item options[] = {
        { 6, "TYPE: ",         (INT8)ARRAY_LEN(options_type),         &options_type[0]        , &option_game_type},
        { 8, "LEVEL: ",        (INT8)ARRAY_LEN(options_difficulty),   &options_difficulty[0]  , &option_game_difficulty},
        { 10,"VISUAL HINTS: ", (INT8)ARRAY_LEN(options_visual_hints), &options_visual_hints[0], &option_game_visual_hints},
        { 14,"   START GAME ", (INT8)ARRAY_LEN(options_visual_hints), NULL,                     NULL}
    };



INT8 options_menu_index = OPTION_MENU_STARTGAME;



// Update which option is currenrlt selected
void options_screen_cursor_update(INT8 dir) {

    // Play a sound when moving the cursor
    if (dir != 0)
        PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x14, 0x94, 0x86);

    // Update menu selection
    options_menu_index += dir;

    // Handle wraparound
    if (options_menu_index < OPTION_MENU_MIN) {
        options_menu_index = OPTION_MENU_MAX;
    }
    else if (options_menu_index > OPTION_MENU_MAX) {
        options_menu_index = OPTION_MENU_MIN;
    }

    move_sprite(SPR_OPTIONS_CURSOR,
                OPTION_CURSOR_X,
                (options[options_menu_index].menu_y + 2) * 8);
}



// Update the currently selected option's value
void options_screen_setting_update(INT8 dir) {


    // "Start Game" menu entry has no settings
    if (options_menu_index != OPTION_MENU_STARTGAME) {

        // Play a sound when moving the cursor
        if (dir != 0) {
            PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x14, 0x94, 0x86);
        }

        // Update setting
        *(options[options_menu_index].p_curval) += dir;

        // Handle wraparound
        if (*(options[options_menu_index].p_curval) < OPTION_SETTING_MIN) {
            *(options[options_menu_index].p_curval) = options[options_menu_index].opt_entries - 1; // zero indexed array
        }
        else if (*(options[options_menu_index].p_curval) >= options[options_menu_index].opt_entries) {
            *(options[options_menu_index].p_curval) = OPTION_SETTING_MIN;
        }

        options_screen_setting_draw(options_menu_index);
    }
}



// Print an option's name and current value
void options_screen_setting_draw(INT8 option_id) {

    // Print option Title
    // (they have trailing spaces)
    PRINT(OPTION_MENU_X,
          options[option_id].menu_y,
          options[option_id].label, 0);

    // Next print the current setting value, using the
    // print cursor at the end of the previous print
    //
    // Don't print current settings for "Start Game" menu entry
    if (option_id != OPTION_MENU_STARTGAME) {
        print_text(options[option_id].p_text_arr[ *(options[option_id].p_curval) ], 0);
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

//    set_sprite_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data, pal_type
    fade_set_pal(BG_PAL_0, 4, board_pets_palette, FADE_PAL_SPRITES);
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);

    set_sprite_tile(SPR_OPTIONS_CURSOR, PET_DOG_HEAD);
    set_sprite_prop(SPR_OPTIONS_CURSOR, BG_FLIP_X); // TODO: should prob set palette here

    SHOW_SPRITES;
}



void options_screen_exit_cleanup(void) {

    fade_start(FADE_OUT);
    HIDE_SPRITES;

}



void options_screen_init(void) {

    INT8 c;

    // set_bkg_palette(BG_PAL_4, 4, intro_screen_palette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
    fade_set_pal(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);

    // TODO: OPTIMIZE: consolidate this? the same tiles are used in all screens so far
    set_bkg_data(TILES_INTRO_START,     TILE_COUNT_INTRO,     intro_screen_tiles);
    set_bkg_data(TILES_FONT_START,      TILE_COUNT_FONT,      font_tiles);
    set_bkg_data(TILES_PET_START,       TILE_COUNT_PETTOTAL,  pet_tiles);

    SHOW_BKG;

    options_menu_index = OPTION_MENU_STARTGAME;

    options_screen_draw();

    for (c = OPTION_MENU_MIN; c <= OPTION_MENU_MAX; c++) {
        options_screen_setting_draw(c);
    }

    // Reveal sprite last since other screen setup/drawing is slow
    options_screen_sprites_init();
    options_screen_cursor_update(0);
    fade_start(FADE_IN);
}



void options_screen_handle(void) {

    // Cursor Updates
    if (KEY_TICKED(J_UP)) {

        options_screen_cursor_update(-1);

    } else if (KEY_TICKED(J_DOWN)) {

        options_screen_cursor_update(1);

    }
    // Change value of current option
    else if (KEY_TICKED(J_LEFT)) {

        options_screen_setting_update(-1);

    } else if (KEY_TICKED(J_RIGHT)) {

        options_screen_setting_update(1);

    }
    // Start game
    else if (KEY_TICKED(J_START)) {

        options_screen_exit_cleanup();
        game_state = GAME_READY_TO_START;
    }
    // Start game or update value of current setting
    else if (KEY_TICKED(J_A)) {

        if (options_menu_index == OPTION_MENU_STARTGAME) {

            options_screen_exit_cleanup();
            game_state = GAME_READY_TO_START;
        }
        else {
            options_screen_setting_update(1);
        }
    }
    // Go back to Intro Screen
    else if (KEY_TICKED(J_B)) {

        options_screen_exit_cleanup();
        game_state = GAME_INTRO_INIT;
    }

}