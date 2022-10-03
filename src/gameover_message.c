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

#include <gbdk/platform.h>

#include "common.h"
#include "platform_compat.h"

#include "game_board.h"
#include "gfx.h"
#include "player_gfx.h"
#include "game_piece_data.h"

#include "gameover_message.h"

#ifdef NINTENDO
    #define CHR2SPR(chr) ((UINT8)(chr -'A') + TILES_FONT_CHARS_START)
#elif defined(GAMEGEAR)
    #define CHR2SPR(chr) ((UINT8)(chr -'A') + SPRITE_TILE_FONT_CHARS_START + TILES_FONT_BLANK_LEN)
#endif

const UINT8 SPR_GAMEOVER_CHARS[SPR_GAMEOVER_COUNT] = {
            CHR2SPR('G'),
            CHR2SPR('A'),
            CHR2SPR('M'),
            CHR2SPR('E'),
            CHR2SPR('O'),
            CHR2SPR('V'),
            CHR2SPR('E'),
            CHR2SPR('R')};

const UINT8 SPR_YOU_LOST_CHARS[SPR_GAMEOVER_COUNT] = {
            CHR2SPR('Y'),
            CHR2SPR('O'),
            CHR2SPR('U'),
            CHR2SPR(' '),
            CHR2SPR('L'),
            CHR2SPR('O'),
            CHR2SPR('S'),
            CHR2SPR('T')};

const UINT8 SPR_YOU_WON_CHARS[SPR_GAMEOVER_COUNT] = {
            CHR2SPR('Y'),
            CHR2SPR('O'),
            CHR2SPR('U'),
            CHR2SPR(' '),
            CHR2SPR('W'),
            CHR2SPR('O'),
            CHR2SPR('N'),
            TILES_FONT_START + 37U /* '!' char */ };

#define SPR_PAL_PRINT BG_PAL_5

#define SPR_GAMEOVER_MAX_Y ((BRD_ST_Y + 8U) * 8U)
#define SPR_GAMEOVER_START_X (((BRD_ST_X + 1U) * 8U) + 2U) // 2 Pixels right of left board edge

const UINT8 SPR_GAMEOVER_LUT_X[] = {SPR_GAMEOVER_START_X,
                                    SPR_GAMEOVER_START_X + (8U * 1U) + 1U, // 1 pixel between each letter
                                    SPR_GAMEOVER_START_X + (8U * 2U) + 2U,
                                    SPR_GAMEOVER_START_X + (8U * 3U) + 3U,
                                    // Gap between GANE -- and -- OVER
                                    SPR_GAMEOVER_START_X + (8U * 4U) + 9U,
                                    SPR_GAMEOVER_START_X + (8U * 5U) + 10U,
                                    SPR_GAMEOVER_START_X + (8U * 6U) + 11U,
                                    SPR_GAMEOVER_START_X + (8U * 7U) + 12U};


// Pre-calculated gravity drop bounce LUT
// For calculation ref commented out version of gameover_message_animate() below
const INT8 SPR_GAMEOVER_LUT_Y[] = {
    0x01, 0x03, 0x06, 0x0A, 0x0F, 0x15, 0x1C, 0x24,
    0x2D, 0x37, 0x38, 0x31, 0x2B, 0x26, 0x22, 0x1F,
    0x1D, 0x1C, 0x1C, 0x1D, 0x1F, 0x22, 0x26, 0x2B,
    0x31, 0x38, 0x40, 0x3B, 0x37, 0x34, 0x32, 0x31,
    0x31, 0x32, 0x34, 0x37, 0x3B, 0x40, 0x3E, 0x3D,
    0x3D, 0x3E, 0x40, 0x3F, 0x3F, 0x40, 0x40};

#define SPR_GAMEOVER_LUT_Y_MAX (ARRAY_LEN(SPR_GAMEOVER_LUT_Y) - 1)

#define GAMEOVER_UPDATE_MASK 0x03 // Only update cursor once every 8 frames
#define SPR_GAMEOVER_GRAVITY 1
#define SPR_GAMEOVER_LANDED  127

UINT8 spr_gameover_y_idx[SPR_GAMEOVER_COUNT];
const UINT8 * p_gameover_chars = NULL;


// Drop "G A M E   O V E R" letters with a bounce, starting from left to right
// Expects font data to already be loaded (see PRINT/etc)
void gameover_message_animate(void) {

    UINT8 c;
    UINT8 min_spr = 0; // Used to slowly exit the loop as pieces land
    UINT8 max_spr = 0; // Used for delay launch left to right


    // Load text message into tiles
    for (c = 0; c< SPR_GAMEOVER_COUNT; c++) {
        set_sprite_tile(SPR_GAMEOVER_START + c, p_gameover_chars[c]);
    }

    // Loop exits when all sprites have landed
    while (min_spr != SPR_GAMEOVER_COUNT) {

        // Add some delay and go easy on the processor
        wait_vbl_done();

        // Periodic Update
        if ((sys_time & GAMEOVER_UPDATE_MASK) == GAMEOVER_UPDATE_MASK) {

            // Delay launch from left to right
            // by adding one more sprite during each animation pass
            if (max_spr < SPR_GAMEOVER_COUNT)
                max_spr++;

            for (c = min_spr; c < max_spr; c++) {

                // Move sprite to next Y LUT position
                spr_gameover_y_idx[c]++;

                // If it's at the end of the LUT then it's landing
                // is complete. Increment the starting sprite past it
                // so that on the next iteration it's excluded
                if (spr_gameover_y_idx[c] >= SPR_GAMEOVER_LUT_Y_MAX) {
                    min_spr++;
                }

                move_sprite(SPR_GAMEOVER_START + c,
                            SPR_GAMEOVER_LUT_X[c] + SPRITE_OFS_X,
                            SPR_GAMEOVER_LUT_Y[ spr_gameover_y_idx[c] ] + SPRITE_OFS_Y);

            } // for (c = min_spr; c < max_spr; c++) {
        } // if ((sys_time & 0x03) == 0x03)
    } // while (min_spr != SPR_GAMEOVER_COUNT)
}

// Hides gameover message sprites and initializes their CGB attributes
// Expects font data to already be loaded (see PRINT/etc)
void gameover_message_reset(void) {

    UINT8 c;

    for (c = 0; c< SPR_GAMEOVER_COUNT; c++) {
        spr_gameover_y_idx[c] = 0; // Reset Y LUT position
        move_sprite(SPR_GAMEOVER_START + c, 0,0);

        // Sprite tiles will get set just before display
        // in order to set the desired message

        // Use solid color palette for all letters
        set_sprite_prop(SPR_GAMEOVER_START + c, SPR_PAL_PRINT);
    }
}

