
// Copyright 2026 (c) bbbbbr
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
#include <gbdk/metasprites.h>

#include <stdint.h>
#include <stdbool.h>

#include "gfx.h"
#include "gfx_print.h"
#include "input.h"


#include "sound.h"
// #include "gbt_player.h"
// #include "audio_common.h"

#include "common.h"
#include "fade.h"

#include "game_piece_data.h"

#include "options.h"
#include "options_screen.h"
#include "cartsave.h"

#include "intro_screen_out.h"  // For determining where to start loading font tiles
#include "pet_and_special_tiles_out.h"


#define TILE_LOAD_OFFSET_FONT_VRAM_ABS          (intro_screen_out_TILE_COUNT + SHARED_4BPP_TRANSP_TILE_COUNT)
#define TILE_LOAD_OFFSET_CURSOR_SPRITE (OBJ_TILEGROUP_BASE_512)

#define PET_DOG_HEAD       ((GP_PET_DOG << GP_PET_UPSHIFT) | (GP_SEG_HEAD << GP_SEG_UPSHIFT))
#define SPR_TILE_CURSOR    (PET_DOG_HEAD)  // Tile ID is offset from the Base automatically in hardware

#define SPR_OPTIONS_CURSOR 0 // Cursor is sprite "0"

#define OPTION_MENU_Y_START 0u
#define OPTION_MENU_X_START ((DEVICE_SCREEN_WIDTH - 16u) / 2)     // 16 is about max width of option name + text
#define OPTION_CURSOR_X     ((OPTION_MENU_X_START * 8u) - 16u)    // x8 for Tile width, -16 for space between start of text and cursor sprite
#define OPTION_CURSOR_X_ITEM_INDENT (OPTION_CURSOR_X + (8u * 3u))  // Inset cursor further for Start menu entry since it's inset

#define CURSOR_UPDATE_MASK 0x03 // Only update cursor once every 8 frames
#define CURSOR_BITSHIFT    3
#define CURSOR_LUT_MASK    0x07 << CURSOR_BITSHIFT // 8 offset entries in the LUT

// #define PRINT_PAL_TITLE      (PRINT_PAL_TILE_YELLOW)
// #define PRINT_PAL_HEADING    (PRINT_PAL_TILE_GREY)
#define PRINT_PAL_VALUE      (PRINT_PAL_TILE_YELLOW)
#define PRINT_PAL_HIGHLIGHT  (PRINT_PAL_TILE_PINK)

#define C_YELLOW (PRINT_PAL_TILE_YELLOW)
#define C_GREY   (PRINT_PAL_TILE_GREY)

const uint8_t spr_cursor_offset[] = {0,1,2,3,3,2,1,0};


// TODO: fold this into the options menu array?
enum option_menu_entries {
    OPTION_MENU_MIN = 0,

    OPTION_MENU_TYPE = OPTION_MENU_MIN,
    OPTION_MENU_LEVEL,
    OPTION_MENU_MUSIC,

    OPTION_MENU_STARTGAME,

    // OPTION_MENU_LINK2P,
    OPTION_MENU_FILTER_DIAGONALS,
    OPTION_MENU_HARD_DROP,
    OPTION_MENU_VISUAL_HINTS,

    OPTION_MENU_RESTORE_DEFAULTS,

    OPTION_MENU_MAX = OPTION_MENU_RESTORE_DEFAULTS
};


// Trailing spaces are to clear out previous option value text
const char * options_type[OPTION_GAME_TYPE_COUNT] = {
                                       "LONG PET    ",
                                       "CRUNCH UP   ",
                                       "TAIL CLEANUP",
                                       "LEVEL UP    ",
                                       "MARATHON    "}; // Must match : game_options.type_entries

const char * options_difficulty[OPTION_DIFFICULTY_COUNT] = {
                                       "EASY  ",
                                       "NORMAL",
                                       "HARD  ",
                                       "EXPERT ", // TODO: is this trailing space here a typo?
                                       "BEAST "}; // Must match : option_difficulty_entries

// const char * options_link2p[]        = {"OFF", "ON "}; // Must match : option_link2p_entries
const char * options_hard_drop[]        = {" ON", "OFF"}; // Must match : option_hard_drop_entries
const char * options_filter_diagonals[] = {"     NORMAL ", " BLOCK DIAG",}; // Must match : option_filter_diagonals_entries
const char * options_visual_hints[]  = {" ON", "OFF"}; // Must match : option_visual_hints_entries

const char * options_music[] = {"TWILIGHT",
                                "CHILL   ",
                                "UPBEAT  ",
                                "OFF     "}; // Must match : option_music_entries


typedef struct opt_item {

    uint16_t       menu_y;      // Y position of option menu entry
    int            color;       // Color used for label
    const char *   label;       // Option name

    int8_t         opt_entries; // Size of option values array
    const char * * p_text_arr;  // Array of option text strings / values
    int8_t *       p_curval;    // Pointer to (extern) option variable

} option_item;

const char str_options_heading[] = "---- OPTIONS ----";

#define MENU_Y(n) (OPTION_MENU_Y_START + n)

#define OPTION_NUM_NONE (0)


// See above for meaning of each element
const option_item options[] = {
        { MENU_Y( 3), C_GREY,   "TYPE : ",        (int8_t)ARRAY_LEN(options_type),          &options_type[0],          &game_options.type},
        { MENU_Y( 5), C_GREY,   "LEVEL: ",        (int8_t)ARRAY_LEN(options_difficulty),    &options_difficulty[0],    &game_options.difficulty},
        { MENU_Y( 7), C_GREY,   "MUSIC: ",        (int8_t)ARRAY_LEN(options_music),         &options_music[0],         &game_options.music},
        { MENU_Y(10), C_YELLOW, "   START GAME ",  OPTION_NUM_NONE,                         NULL, NULL},
        // TODO: High Score menu item here or at bottom of list
        // { MENU_Y(13), C_GREY,   "2 PLAYER VS:  ", (int8_t)ARRAY_LEN(options_link2p),        &options_link2p[0],           &game_options.link2p},
        { MENU_Y(13), C_GREY,   "HARD DROP:    ", (int8_t)ARRAY_LEN(options_hard_drop),        &options_hard_drop[0],        &game_options.hard_drop},
        { MENU_Y(15), C_GREY,   "DPAD: ", (int8_t)ARRAY_LEN(options_filter_diagonals), &options_filter_diagonals[0], &game_options.filter_diagonals},
        { MENU_Y(17), C_GREY,   "VISUAL HINTS: ", (int8_t)ARRAY_LEN(options_visual_hints),     &options_visual_hints[0],     &game_options.visual_hints},
        //
        { MENU_Y(20), C_YELLOW, "   LOAD DEFAULTS", OPTION_NUM_NONE,                             NULL, NULL},
    };



int8_t options_menu_index = OPTION_MENU_STARTGAME;


static void option_draw_all_menu_items(void);
static void options_screen_cursor_update(int8_t);
static void options_screen_setting_update(int8_t);
static void options_screen_setting_draw(int8_t);
static void options_screen_draw(void);
static void options_screen_exit_cleanup(void);
static void options_screen_try_gamestart(void);
static void cursor_animate(void);


// Redraw all menu entries
static void option_draw_all_menu_items(void) {
    for (int c = OPTION_MENU_MIN; c <= OPTION_MENU_MAX; c++) {  // TODO
        options_screen_setting_draw(c);
    }
}


// Update which option is currenrlt selected
static void options_screen_cursor_update(int8_t dir) {

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
static void options_screen_setting_update(int8_t dir) {

    const option_item * p_opt = &options[options_menu_index];

    // Only update entries that have settings
    if (p_opt->opt_entries > 0) {

        // Update setting
        *(p_opt->p_curval) += dir;

        // Handle Min/Max
        if (*(p_opt->p_curval) < OPTION_SETTING_MIN) {
                *(p_opt->p_curval) = OPTION_SETTING_MIN;
        }
        else if (*(p_opt->p_curval) >= p_opt->opt_entries) {
                *(p_opt->p_curval) = p_opt->opt_entries - 1; // zero indexed array
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
static void options_screen_setting_draw(int8_t option_id) {

    const option_item * p_opt = &options[option_id];

    // Display option Titles using a different color (except Start)
    PRINT_PAL(p_opt->color);

    // Print option Title
    // (they have trailing spaces)
    PRINTXY(OPTION_MENU_X_START,
            p_opt->menu_y,
            p_opt->label, 0);

    // Restore default palette
    PRINT_PAL(PRINT_PAL_VALUE);

    // Next print the current setting value, using the
    // print cursor at the end of the previous print
    //
    // Don't print current settings for "Start Game" menu entry
    if (p_opt->opt_entries > 0) {
        PRINT(p_opt->p_text_arr[ *(p_opt->p_curval) ], 0);
    }
}


// Now obsolete
/*static void options_screen_title_draw(void) {

    PRINT_PAL(PRINT_PAL_TITLE);
    PRINTXY(OPTION_MENU_X_START - 1u, MENU_Y(0), str_options_heading, 0);
    // PRINTXY((DEVICE_SCREEN_WIDTH - ARRAY_LEN(str_title)) / 2,3, str_title, 0);
}
*/


static void options_screen_sprites_init(void) {

    // Use a pet tile as a cursor, so load the pet tiles into the chosen tile obj area
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_4bpp_data(TILE_LOAD_OFFSET_CURSOR_SPRITE, pet_and_special_tiles_out_TILE_COUNT, pet_and_special_tiles_out_tiles);
    set_fade_target_4bpp_palette(PAL_ASSIGN_OBJ_0, pet_and_special_tiles_out_PALETTE_COUNT, pet_and_special_tiles_out_palettes);

    set_sprite_tile(SPR_OPTIONS_CURSOR, SPR_TILE_CURSOR);
    set_sprite_prop(SPR_OPTIONS_CURSOR, S_8x8 | S_FLIPX | S_PAL0);

    SHOW_SPRITES;
}


static void options_screen_exit_cleanup(void) {

    // Save updated settings to SRAM
    cartsave_save_options();

    palette_fade_start(FADE_TO_BLACK);
    palette_fade_wait_completed();

    HIDE_SPRITES;  // TODO

    // TODO: Fill BG0 tilemap with empty tiles
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);

    // Clear BG0 (where the text is printed)
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, SHARED_4BPP_TRANSP_TILE_ID_START);
    VDP.BG_SCROLL[BG0_SCROLL_Y] = 0;
}


// Assumes and relies on "intro_screen" (title) having run and initialized
void options_screen_init(void) {  // TODO

    // Rely on Title screen graphics loading for Background image and Font tiles / drawing setup
    load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT_VRAM_ABS, 0u);  // No use of sprite printing, OAM tile load start is a shim

    // SHOW_BKG;

    options_menu_index = OPTION_MENU_STARTGAME;

    load_8x16_font_tilemap_palettes();

    // options_screen_title_draw();  // obsolete
    option_draw_all_menu_items();

    // // Reveal sprite last since other screen setup/drawing is slow
    options_screen_sprites_init();
    options_screen_cursor_update(0);
    palette_fade_start(FADE_TO_TARGET);

    // // Update music status to match option menu setting
    // MusicUpdateStatus();

}



static void options_screen_try_gamestart(void) {
/*
    // If serial link 2-Player versus is being initiated
    // then try to link up with the other player
    // before starting the game.
    if (game_options.link2p == OPTION_LINK2P_ON) {

        // TODO: 2-Player game start
    } else {
    */
        // 1-Player game start
        options_screen_exit_cleanup();
        game_state = GAME_READY_TO_START;
    /*
    }
    */
}


static void cursor_animate(void) {
    // Update cursor every N frames
    if ((sys_time & CURSOR_UPDATE_MASK) == CURSOR_UPDATE_MASK) {
        // Animate the cursor with a mild bounce
        // Indent if it's one of the fixed/control menu items
        uint8_t cursor_x = (options[options_menu_index].opt_entries > 0) ? OPTION_CURSOR_X : OPTION_CURSOR_X_ITEM_INDENT;
        move_sprite(SPR_OPTIONS_CURSOR,
                    cursor_x,
                    (((options[options_menu_index].menu_y) * 8) + 4)  // + 2 is sprite vs bg offset
                    + spr_cursor_offset[(sys_time & CURSOR_LUT_MASK) >> CURSOR_BITSHIFT] - 2);
    }
}


void options_screen_handle(void) {

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
        } else if (options_menu_index == OPTION_MENU_RESTORE_DEFAULTS) {

            options_reset_to_defaults();
            option_draw_all_menu_items();
        } else {

            // Change value of current option
            options_screen_setting_update(1);
        }
    }
    else if (KEY_TICKED(J_B)) {

        if (options_menu_index == OPTION_MENU_STARTGAME) {

            options_screen_try_gamestart();
        } else if (options_menu_index == OPTION_MENU_RESTORE_DEFAULTS) {

            options_reset_to_defaults();
            option_draw_all_menu_items();
        } else {

            // Change value of current option
            options_screen_setting_update(-1);
        }

    }

    // Start game
    else if (KEY_TICKED(J_START)) {

        options_screen_try_gamestart();
    }

    // Go back to Intro Screen
    else if (KEY_TICKED(J_Y)) {

        options_screen_exit_cleanup();
        game_state = GAME_INTRO_INIT;
    }

    cursor_animate();
}
