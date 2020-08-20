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


// Both player consoles start and mostly wait
// in a ready-to-recieve state:
// * External Clock + Transfer Start set
//
// 1. Either console can initiate connection by sending
//    a INITIATE command.
//
// 2. The other console can respond by sending a READY,
//    after sending of that command is completed then
//    **BOTH** consoles immediately set link status as CONNECTED
//
// State A: RESET                   /--Rx:-> CONNECTED (follower)
// Link  A:  -> Tx:INITIATE        /
// Link  B:           \--Rx--> Tx:READY
// State B: RESET                   \------> CONNECTED (controller)


// TODO: what if link is lost?
// TODO: icon to indicate link status? "-vs-" or "]--["


// If a serial transfer with internal clock is performed and no external GameBoy
// is present, a value of $FF will be received in the transfer.


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"

#include "gfx.h"
#include "gfx_print.h"

#include "status_win.h"

const UINT8 status_win_attrib_pal = BG_PAL_7;

const UINT8 popup_win_tiles_top[] = {MKPET_TILE(16), // Left L
                                     // Torso Horiz x 18
                                     MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8),
                                     MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8),
                                     MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8),
                                     MKPET_TILE(8), MKPET_TILE(8), MKPET_TILE(8),
                                     MKPET_TILE(20)}; // Right L

const UINT8 popup_win_tiles_main[] = {MKPET_TILE(12), // Torso Vert
                                     // Blank x 18
                                     TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK,
                                     TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK,
                                     TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK,
                                     TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK, TILE_ID_FONT_BLANK,
                                     MKPET_TILE(12)}; // Torso Vert


const UINT8 popup_win_row_pal[] = {BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7,
                                   BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7,
                                   BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7,
                                   BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7, BG_PAL_7};


// Note: Relies on option_screen_init() for palette init in slot #7
//       and all background tile loading
void status_win_popup_init(void) {

    UINT8 y;
    UINT8 * tile_row;

    WX_REG = 0 + 7;  // Start at 0, window starts offset 7 pixels to the right
    WY_REG = WIN_Y_MAX; // Start hidden off-screen

    // Draw window borders
    for (y = SCREEN_MIN_Y; y <= SCREEN_MAX_Y; y++) {

        // BG tile attribute map
        VBK_REG = 1;
        // Relies on option_screen_init() for palette init
        set_win_tiles(SCREEN_MIN_X, y, SCREEN_MAX_X + 1, 1, popup_win_row_pal );

        // Select BG tile map
        VBK_REG = 0;

        // Draw a border of pet tiles, regular tiles in the middle
        if (y == 0)
            tile_row = (UINT8 *)popup_win_tiles_top;
        else
            tile_row = (UINT8 *)popup_win_tiles_main;

        set_win_tiles(SCREEN_MIN_X, y, SCREEN_MAX_X + 1, 1, tile_row);
    }
}


void status_win_popup_show(UINT8 y_min) {

    UINT8 c;

    HIDE_SPRITES;
    SHOW_WIN;

    for (c = WY_REG; c >= y_min; c--) {
        WY_REG = c;
        delay(WIN_SCROLL_DELAY);
    }
}


void status_win_popup_hide(void) {

    UINT8 c;

    for (c = WY_REG; c < WIN_Y_MAX; c++) {
        WY_REG = c;
        delay(WIN_SCROLL_DELAY);
    }
    HIDE_WIN;
    SHOW_SPRITES;
}
