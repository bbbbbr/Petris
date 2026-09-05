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

// gfx_print.c

#include <gbdk/platform.h>

#include "input.h"
// #include "audio_common.h"  // TODO
#include "gfx.h"
#include "gfx_print.h"

#include "font_8x16_out.h"

uint16_t gfx_tiles_font_base = 0;

const palette_color_t font_8x16_out_palette_PINK[16] = {
    RGB8(  0,  0,  0), RGB8(175,134,178), RGB8(228,195,231), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0)
    };

const palette_color_t font_8x16_out_palette_GREY[16] = {
    RGB8(  0,  0,  0), RGB8(148,148,148), RGB8(227,227,227), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0),
    RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0), RGB8(  0,  0,  0)
    };



// Loads 8x16 font tiles into vram and sets gfx_tiles_font_base
// Returns next tile index after last used
uint16_t load_8x16_font_tiles(uint16_t tile_id_start) {

    gfx_tiles_font_base = tile_id_start;

    // Load font and it's default yellow palette
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_4bpp_palette(PAL_8, font_8x16_out_PALETTE_COUNT, font_8x16_out_palettes);
    set_bkg_4bpp_data(gfx_tiles_font_base, font_8x16_out_TILE_COUNT, font_8x16_out_tiles);

    // Set secondary Pink and Grey palettes
    set_bkg_4bpp_palette(PAL_9,  1, font_8x16_out_palette_PINK);
    set_bkg_4bpp_palette(PAL_10, 1, font_8x16_out_palette_GREY);

    return (tile_id_start + gfx_tiles_font_base);
}


// Returns: Next OAM entry after last used
uint16_t print_to_sprites(uint16_t oam_id, uint16_t print_x, uint16_t print_y, uint8_t pal, const char * txt) {

    uint16_t c;
    uint16_t start_x = print_x;

    while (*txt) {

        if (*txt >= 'A' && *txt <= 'Z'){
            c = TILES_FONT_CHARS_START + (unsigned char)(*txt - 'A');
        } else if(*txt >= 'a' && *txt <= 'z') {
            c = TILES_FONT_CHARS_START + (unsigned char)(*txt - 'a');
        } else if(*txt >= '0' && *txt <= '9') {
            c = TILES_FONT_NUMS_START + (unsigned char)(*txt - '0');
        } else {
            switch(*txt) {
                case  '!': c = 37U; break;
                case '\'': c = 38U; break;
                case  '(': c = 39U; break;
                case  ')': c = 40U; break;
                case  '-': c = 41U; break;
                case  '.': c = 42U; break;
                case  ':': c = 43U; break;
                case  '?': c = 44U; break;
                case '\n':
                    // Do a carriage return, no printing and skip to top of loop
                    print_x = start_x;
                    print_y += SPR_PRINT_SPACING_Y;
                    txt++;
                    continue;
                // Default is blank tile for Space or any other unknown chars
                default: c = TILE_ID_FONT_BLANK; break;
            }
        }

        c *= 2 + gfx_tiles_font_base; // 2 tiles per character
        set_sprite_tile(oam_id, c++);  // Set sprite and advance to next 8x8 tile
        set_sprite_prop(oam_id, S_8x8 | pal);
        move_sprite(oam_id++, print_x, print_y);

        set_sprite_tile(oam_id, c);
        set_sprite_prop(oam_id, S_8x8 | pal);
        move_sprite(oam_id++, print_x, print_y + SPR_8x8_HEIGHT);

        print_x += SPR_PRINT_SPACING_X;

        print_x++;
        txt++;

        /*
        if (delay_time) {  // TODO: Sprite print delay time

            UPDATE_KEYS();
            // Skip delay if buttons pressed
            if (!KEY_PRESSED(J_B | J_A)) {
                PLAY_SOUND_PRINT_CHAR;
                delay(delay_time);
            }
        }
        */
    }
    return oam_id;
}
/*
uint8_t digits[PRINT_MAX_DIGITS];
uint8_t digits_attribs[PRINT_MAX_DIGITS] = {PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT,
                                          PRINT_ATTRIB_PAL_DEFAULT};

uint8_t print_x  = 0;
uint8_t print_y  = 0;
uint8_t print_target = PRINT_BKG;
uint8_t print_tile_attribs = PRINT_ATTRIB_PAL_DEFAULT;


// Render a font digit
void print_num_u16(uint8_t x, uint8_t y, uint16_t num, uint8_t print_digits) {

    // Initialize index at END of array +1,
    // so that the first pass sets it to the first array position
    uint8_t index = PRINT_MAX_DIGITS;

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

            VBK_REG = 1; // Select BG tile attribute map
            set_bkg_tiles(0x1F & (print_x), 0x1F & (print_y), 1, 1, &print_tile_attribs);

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
*/