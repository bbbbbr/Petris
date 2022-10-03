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

// gfx_print.c

#include <gbdk/platform.h>
#include "asm/types.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

#include "common.h"
#include "platform_compat.h"

#include "input.h"
#include "audio_common.h"
#include "gfx.h"
#include "gfx_print.h"


UINT8 digits[PRINT_MAX_DIGITS];
UINT8 digits_attribs[PRINT_MAX_DIGITS] = {PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT};

UINT8 print_x  = 0;
UINT8 print_y  = 0;
UINT8 print_target = PRINT_BKG;
UINT8 print_tile_attribs = PRINT_ATTRIB_PAL_DEFAULT;


// Render a font digit
void print_num_u16(UINT8 x, UINT8 y, UINT16 num, UINT8 print_digits) {

    // Initialize index at END of array +1,
    // so that the first pass sets it to the first array position
    UINT8 index = PRINT_MAX_DIGITS;

    if (print_digits > PRINT_MAX_DIGITS)
        print_digits = PRINT_MAX_DIGITS;

    // Optional: Bounds checking
//    if (num <= PRINT_MAX_NUM) {
        // Store individual digits of n in reverse order
        // Starting at the END of the array and working forward
        // (using do-while to handle when initial value == 0)
        do {
            // decrement the counter first, so it finishes as pointing to the current digit in the array
            index--;
            digits[index] = (num % 10) + TILES_FONT_NUMS_START;
            num = num / 10;
        } while (num != 0);

        // Fill remaining spaces with empty tiles
        while (index > 0) {
            index--;
            digits[index] = TILE_ID_FONT_BLANK;
        }

        // Draw the digits on the background

        // Update BG Tilemap from Game Board
        // VBK_REG = 1; // Select BG tile attribute map
        // set_bkg_tiles(x,
        //               y,
        //               print_digits, 1, // 1 tile high
        //               &digits_attribs[index + (PRINT_MAX_DIGITS - print_digits) ]); // Start at first digit and go to end of array
        set_bkg_tiles_attr_compat(x, y,
                                  print_digits, 1, // 1 tile high
                                  &digits_attribs[index + (PRINT_MAX_DIGITS - print_digits) ]); // Start at first digit and go to end of array

        // Update BG Tilemap from Game Board
        VBK_REG = 0; // Re-Select regular BG tile map
        set_bkg_tiles(x,
                      y,
                      print_digits, 1, // 1 tile high
                      &digits[index + (PRINT_MAX_DIGITS - print_digits) ]); // Start at first digit and go to end of array
//    }
}



// Copied from ZGB Print
// Removed some code, removed some characters
void print_text(const char* txt, unsigned char delay_time){

    unsigned char c;
    unsigned char start_x;

    start_x = print_x; // Save start X for newline return

    while(*txt) {

        if(*txt >= 'A' && *txt <= 'Z'){
            c = TILES_FONT_CHARS_START + (unsigned char)(*txt - 'A');
        } else if(*txt >= 'a' && *txt <= 'z') {
            c = TILES_FONT_CHARS_START + (unsigned char)(*txt - 'a');
        } else if(*txt >= '0' && *txt <= '9') {
            c = TILES_FONT_NUMS_START + (unsigned char)(*txt - '0');
        } else {
            switch(*txt) {
                case  '!': c = TILES_FONT_START + 37U; break;
                case '\'': c = TILES_FONT_START + 38U; break;
                case  '(': c = TILES_FONT_START + 39U; break;
                case  ')': c = TILES_FONT_START + 40U; break;
                case  '-': c = TILES_FONT_START + 41U; break;
                case  '.': c = TILES_FONT_START + 42U; break;
                case  ':': c = TILES_FONT_START + 43U; break;
                case  '?': c = TILES_FONT_START + 44U; break;
                case '\n':
                    // Do a carriage return, no printing and skip to top of loop
                    print_x = start_x;
                    print_y++;
                    txt++;
                    continue;
                // Default is blank tile for Space or any other unknown chars
                default: c = TILE_ID_FONT_BLANK; break;
            }
        }

        if(print_target == PRINT_BKG) {

            // VBK_REG = 1; // Select BG tile attribute map
            // set_bkg_tiles(0x1F & (print_x), 0x1F & (print_y), 1, 1, &print_tile_attribs);
            set_bkg_tiles_attr_compat(0x1F & (print_x), 0x1F & (print_y), 1, 1, &print_tile_attribs);

            VBK_REG = 0; // Select BG tile map
            set_bkg_tiles(0x1F & (print_x), 0x1F & (print_y), 1, 1, &c);
        }
        else
            set_win_tiles(print_x, print_y, 1, 1, &c);

        print_x++;
        txt++;

        if (delay_time) {

            UPDATE_KEYS();
            // Skip delay if buttons pressed
            if (!KEY_PRESSED(J_B | J_A)) {
                PLAY_SOUND_PRINT_CHAR;
                delay(delay_time);
            }
        }
    }
}