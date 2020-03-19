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


#define PRINT_MAX_DIGITS  4
#define PRINT_MAX_NUM     9999 // ((10 ^ PRINT_MAX_DIGITS) - 1)
UINT8 digits[PRINT_MAX_DIGITS];

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
            digits[index] = TILE_FONT_NUMS_BLANK;
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
