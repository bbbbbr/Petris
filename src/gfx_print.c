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
UINT8 digits[PRINT_MAX_DIGITS];
const UINT8 print_tile_cleared_attribs = 0x00;

UINT8 print_x  = 0;
UINT8 print_y  = 0;
UINT8 print_target = PRINT_BKG;


// Render a font digit
void print_num_u16(UINT8 x, UINT8 y, UINT16 num) {

    // Initialize index at END of array +1,
    // so that the first pass sets it to the first array position
    UINT8 index = PRINT_MAX_DIGITS;

    // Bounds checking
    if (num <= PRINT_MAX_NUM) {
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
        // VBK_REG = 1; // Select BG tile attribute map
        // set_bkg_tiles(x, y,
        //               (PRINT_MAX_DIGITS - index), 1, // 1 tile high
        //               &digits[index]); // Start at first digit and go to end of array

        // Update BG Tilemap from Game Board
        VBK_REG = 0; // Re-Select regular BG tile map
        set_bkg_tiles(x, y,
                      (PRINT_MAX_DIGITS - index), 1, // 1 tile high
                      &digits[index]); // Start at first digit and go to end of array
    }
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
                case  '!': c = TILES_FONT_CHARS_START + 37; break;
                case '\'': c = TILES_FONT_CHARS_START + 38; break;
                case  '(': c = TILES_FONT_CHARS_START + 39; break;
                case  ')': c = TILES_FONT_CHARS_START + 40; break;
                case  '-': c = TILES_FONT_CHARS_START + 41; break;
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
            // PlayFx(CHANNEL_1, 0,  0x20, 0x81, 0x43, 0x59, 0x86);
    }
}