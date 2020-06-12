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

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include "asm/types.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>


#include "gfx.h"
#include "gfx_print.h"


#define PRINT_MAX_DIGITS  5
#define PRINT_MAX_NUM     99999 // ((10 ^ PRINT_MAX_DIGITS) - 1)
#define PRINT_ATTRIB_PAL_MASK    0x03
#define PRINT_ATTRIB_PAL_DEFAULT 0x04
UINT8 digits[PRINT_MAX_DIGITS];
UINT8 digits_attribs[PRINT_MAX_DIGITS] = {PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT};
UINT8 print_tile_cleared_attribs = PRINT_ATTRIB_PAL_DEFAULT;

UINT8 print_x  = 0;
UINT8 print_y  = 0;
UINT8 print_target = PRINT_BKG;



// Set font to use for printing
void print_font_palette_set(UINT8 pal_num) {
    print_tile_cleared_attribs = pal_num & PRINT_ATTRIB_PAL_MASK;
}



// Render a font digit
void print_num_u16(UINT8 x, UINT8 y, UINT16 num, UINT8 print_digits) {

    // Initialize index at END of array +1,
    // so that the first pass sets it to the first array position
    UINT8 index = PRINT_MAX_DIGITS;

    if (print_digits > PRINT_MAX_DIGITS)
        print_digits = PRINT_MAX_DIGITS;

    // Bounds checking
// TODO: REMOVE?
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

        // TODO: OPTION TO SELECT LEFT / RIGHT ALIGN for padding

        // Fill remaining spaces with empty tiles
        while (index > 0) {
            index--;
            digits[index] = TILE_ID_FONT_BLANK;
        }

        // Draw the digits on the background

        // Update BG Tilemap from Game Board
        VBK_REG = 1; // Select BG tile attribute map
        set_bkg_tiles(x,
                      y,
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

    UINT8 idx = 0;
    unsigned char c;
    unsigned char start_x;

    start_x = print_x; // Save start X for newline return

    while(*txt) {
        // Handle speedup-escape from delay printing
        // if (joypad()) // TODO: remove or re-enable (delay printing escape)
        //     delay_time = 0;

        if(*txt == ' ') {
            c = TILE_ID_FONT_BLANK;
        } else if(*txt >= 'A' && *txt <= 'Z'){
            c = TILES_FONT_CHARS_START + *txt - 'A';
        } else if(*txt >= 'a' && *txt <= 'z') {
            c = TILES_FONT_CHARS_START + *txt - 'a';
        } /* else if(*txt >= '0' && *txt <= '9') {
            c = font_idx + 27 + *txt - '0';
        } */ else {
            switch(*txt) {
                case  '!': c = TILES_FONT_START + 37; break;
                case '\'': c = TILES_FONT_START + 38; break;
                case  '(': c = TILES_FONT_START + 39; break;
                case  ')': c = TILES_FONT_START + 40; break;
                case  '-': c = TILES_FONT_START + 41; break;
                case  '.': c = TILES_FONT_START + 42; break;
                case  ':': c = TILES_FONT_START + 43; break;
                case  '?': c = TILES_FONT_START + 44; break;
                case '\n':
                    // Do a carriage return, no printing and skip to top of loop
                    print_x = start_x;
                    print_y++;
                    txt++;
                    if (delay_time)
                        delay(delay_time);
                    continue;
            }
        }

        if(print_target == PRINT_BKG) {

            VBK_REG = 1; // Select BG tile attribute map
            set_bkg_tiles(0x1F & (print_x), 0x1F & (print_y), 1, 1, &print_tile_cleared_attribs);

            VBK_REG = 0; // Select BG tile map
            set_bkg_tiles(0x1F & (print_x), 0x1F & (print_y), 1, 1, &c);
        }
        else
            set_win_tiles(print_x, print_y, 1, 1, &c);

        print_x++;
        txt++;
        // TODO: Optional - re-enable delay printing
        // if (delay_time)
        //     delay(delay_time);
            // OPTIONAL: enable or remove sounds while printing text
            // PLAY_SOUND_PRINT_CHAR;
    }
}