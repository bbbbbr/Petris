
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

#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

#include "gfx.h"
#include "gfx_print.h"
#include "input.h"


#include "sound.h"
// #include "gbt_player.h"
// #include "audio_common.h"

#include "common.h"
// #include "fade.h"

// #include "game_piece_data.h"

#include "options.h"
#include "options_screen.h"

// #include "../res/intro_screen_tiles.h"
// #include "../res/pet_tiles.h"
// #include "../res/font_tiles.h"



#define SPR_OPTIONS_CURSOR 0 // Cursor is sprite "0"
#define PET_DOG_HEAD       ((GP_PET_DOG << GP_PET_UPSHIFT) | (GP_SEG_HEAD << GP_SEG_UPSHIFT))

#define OPTION_MENU_X_START 2
#define OPTION_CURSOR_X     12 // (tiles from Left Edge: 1.5 x 8 pixels per tile)

#define CURSOR_UPDATE_MASK 0x03 // Only update cursor once every 8 frames
#define CURSOR_BITSHIFT    2
#define CURSOR_LUT_MASK    0x07 << CURSOR_BITSHIFT // 8 offset entries in the LUT

#define OPTION_TITLE_PRINT_PAL BG_PAL_3

const uint8_t spr_cursor_offset[] = {0,1,2,3,3,2,1,0};


// TODO: fold this into the options menu array?
enum option_menu_entries {
    OPTION_MENU_MIN = 0,

    OPTION_MENU_TYPE = OPTION_MENU_MIN,
    OPTION_MENU_LEVEL,
    OPTION_MENU_MUSIC,

    OPTION_MENU_STARTGAME,

    OPTION_MENU_LINK2P,
    OPTION_MENU_VISUAL_HINTS,

    OPTION_MENU_MAX = OPTION_MENU_VISUAL_HINTS
};


// Trailing spaces are to clear out previous option value text
const char * options_type[]         = {"LONG PET    ",
                                       "CRUNCH UP   ",
                                       "TAIL CLEANUP",
                                       "LEVEL UP    ",
                                       "MARATHON    "}; // Must match : option_game_type_entries
const char * options_difficulty[]   = {"EASY  ",
                                       "NORMAL",
                                       "HARD  ",
                                       "EXPERT ",
                                       "BEAST "}; // Must match : option_difficulty_entries
const char * options_link2p[]        = {"OFF", "ON "}; // Must match : option_link2p_entries
const char * options_visual_hints[]  = {"ON ", "OFF"}; // Must match : option_visual_hints_entries

const char * options_music[] = {"TWILIGHT",
                                "CHILL   ",
                                "UPBEAT  ",
                                "OFF     "}; // Must match : option_music_entries


typedef struct opt_item {

    uint8_t          menu_y;      // Y position of option menu entry
    const char *   label;       // Option name

    int8_t           opt_entries; // Size of option values array
    const char * * p_text_arr;  // Array of option text strings / values
    int8_t *         p_curval;    // Pointer to (extern) option variable

} option_item;

// See above for meaning of each element
const option_item options[] = {
        {  5,"TYPE :",         (int8_t)ARRAY_LEN(options_type),          &options_type[0],          &option_game_type},
        {  7,"LEVEL:",         (int8_t)ARRAY_LEN(options_difficulty),    &options_difficulty[0],    &option_game_difficulty},
        {  9,"MUSIC:",         (int8_t)ARRAY_LEN(options_music),         &options_music[0],         &option_game_music},
        { 12,"   START GAME ", (int8_t)ARRAY_LEN(options_visual_hints),  NULL, NULL},
        { 14,"2 PLAYER VS.: ", (int8_t)ARRAY_LEN(options_link2p),        &options_link2p[0],        &option_game_link2p},
        { 16,"VISUAL HINTS: ", (int8_t)ARRAY_LEN(options_visual_hints),  &options_visual_hints[0],  &option_game_visual_hints},
    };



int8_t options_menu_index = OPTION_MENU_STARTGAME;



// Update which option is currenrlt selected
void options_screen_cursor_update(int8_t dir) {

/*
    // Play a sound when moving the cursor
    if (dir != 0)
        PLAY_SOUND_OPTION_CURSOR_MOVE;  // TODO
*/
    // Update menu selection
    options_menu_index += dir;

    // Handle wraparound
    if (options_menu_index < OPTION_MENU_MIN) {
        options_menu_index = OPTION_MENU_MAX;
    }
    else if (options_menu_index > OPTION_MENU_MAX) {
        options_menu_index = OPTION_MENU_MIN;
    }

    // Note: Moving the menu cursor sprite is turned off here.
    //       Instead it gets updated in the main options screen handler
}



// Update the currently selected option's value
void options_screen_setting_update(int8_t dir) {


    // "Start Game" menu entry has no settings
    if (options_menu_index != OPTION_MENU_STARTGAME) {

        // Update setting
        *(options[options_menu_index].p_curval) += dir;

        // Handle Min/Max
        if (*(options[options_menu_index].p_curval) < OPTION_SETTING_MIN) {
                *(options[options_menu_index].p_curval) = OPTION_SETTING_MIN;
        }
        else if (*(options[options_menu_index].p_curval) >= options[options_menu_index].opt_entries) {
                *(options[options_menu_index].p_curval) = options[options_menu_index].opt_entries - 1; // zero indexed array
        } else {
            // Only play a sound if the cursor moved and wasn't clamped to min/max
            /*
            if (dir != 0) {
                PLAY_SOUND_OPTION_CURSOR_MOVE;  // TODO
            }
            */
        }


        options_screen_setting_draw(options_menu_index);

        // Turn music on/off to preview music option
        if (options_menu_index == OPTION_MENU_MUSIC) {

            // Can test using the var linked from the options menu
            // here, since it should be updated by now
            MusicUpdateStatus();
        }

    }
}



// Print an option's name and current value
void options_screen_setting_draw(int8_t option_id) {

/*    // Display option Titles using a lighter font (except Start)
    if (option_id != OPTION_MENU_STARTGAME) {
        PRINT_PAL(OPTION_TITLE_PRINT_PAL);
    }

    // Print option Title
    // (they have trailing spaces)
    PRINT(OPTION_MENU_X_START,
          options[option_id].menu_y,
          options[option_id].label, 0);

    // Restore default palette
    PRINT_PAL(PRINT_ATTRIB_PAL_DEFAULT);

    // Next print the current setting value, using the
    // print cursor at the end of the previous print
    //
    // Don't print current settings for "Start Game" menu entry
    if (option_id != OPTION_MENU_STARTGAME) {
        print_text(options[option_id].p_text_arr[ *(options[option_id].p_curval) ], 0);
    }

*/
}


void options_screen_draw(void) {  // TODO

    // PRINT(2,3, "--- OPTIONS ---", 0);
}



void options_screen_sprites_init(void) {  // TODO
/*
    SPRITES_8x8;

//    set_sprite_palette(BG_PAL_0, 4, board_pets_palette); // UBYTE first_palette, UBYTE nb_palettes, uint16_t *rgb_data, pal_type
    // No need for high-contrast here (user has not yet selected high-contrast)
    fade_set_pal(BG_PAL_0, 4, board_pets_palette, FADE_PAL_SPRITES);
    set_sprite_data(0, TILE_COUNT_PETTOTAL, pet_tiles);

    set_sprite_tile(SPR_OPTIONS_CURSOR, PET_DOG_HEAD);
    set_sprite_prop(SPR_OPTIONS_CURSOR, BG_FLIP_X | BG_PAL_0);

    SHOW_SPRITES;
*/
}



void options_screen_exit_cleanup(void) {
/*
    fade_start(FADE_OUT);
    HIDE_SPRITES;
*/
}


// Assumes and relies on "intro_screen" (title) having run and initialized
void options_screen_init(void) {  // TODO
/*
    // 
    int8_t c;

    // Note: The popup status window is relying on the gfx initialization here
    //       See: status_win_popup_init()

    // Add a lighter font for options
    fade_set_pal(OPTION_TITLE_PRINT_PAL, 1, option_title_palette, FADE_PAL_BKG);
    // Upper 4 palettes from intro screen
    fade_set_pal(BG_PAL_4, 4, intro_screen_palette, FADE_PAL_BKG);

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

    // Update music status to match option menu setting
    MusicUpdateStatus();
    */
}



void options_screen_try_gamestart(void) {
/*
    // If serial link 2-Player versus is being initiated
    // then try to link up with the other player
    // before starting the game.
    if (option_game_link2p == OPTION_LINK2P_ON) {

        // TODO: 2-Player game start
    } else {
        // 1-Player game start
        options_screen_exit_cleanup();
        game_state = GAME_READY_TO_START;
    }
    */
}



void options_screen_handle(void) {
/*
    // Cursor Updates
    if (KEY_TICKED(J_UP)) {

        options_screen_cursor_update(-1);
    }
    else if (KEY_TICKED(J_DOWN)) {

        options_screen_cursor_update(1);
    }

    // Change value of current option
    else if (KEY_TICKED(J_LEFT)) {

        options_screen_setting_update(-1);
    }
    else if (KEY_TICKED(J_RIGHT)) {

        options_screen_setting_update(1);
    }

    // Start game or update value of current setting
    // NOTE: Left/Right not used for starting game since it's
    // easier to accidentally toggle them without intention
    else if (KEY_TICKED(J_A)) {

        if (options_menu_index == OPTION_MENU_STARTGAME) {

            options_screen_try_gamestart();
        } else {

            // Change value of current option
            options_screen_setting_update(1);
        }
    }
    else if (KEY_TICKED(J_B)) {

        if (options_menu_index == OPTION_MENU_STARTGAME) {

            options_screen_try_gamestart();
        } else {

            // Change value of current option
            options_screen_setting_update(-1);
        }

    }

    // Start game
    else if (KEY_TICKED(J_START)) {

        options_screen_try_gamestart();
    }

    // NOTE: For now, A/B are also used to increase/decrease option settings
    //       so don't use B to return to main intro screen
    //
    // // Go back to Intro Screen
    // else if (KEY_TICKED(J_B)) {
    //
    //     options_screen_exit_cleanup();
    //     game_state = GAME_INTRO_INIT;
    // }

    // Update cursor every N frames
    if ((sys_time & CURSOR_UPDATE_MASK) == CURSOR_UPDATE_MASK) {
        // Animate the cursor with a mild bounce
        move_sprite(SPR_OPTIONS_CURSOR,
                    OPTION_CURSOR_X,
                    ((options[options_menu_index].menu_y + 2) * 8)  // + 2 is sprite vs bg offset
                    + spr_cursor_offset[(sys_time & CURSOR_LUT_MASK) >> CURSOR_BITSHIFT] - 2);
    }
    */
}
