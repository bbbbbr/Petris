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

#define FONT_8x16_BLANK_LEN   1u
#define FONT_8x16_CHARS_LEN   26u
#define FONT_8x16_NUMS_LEN    11u

#define FONT_8x16_TILE_ID_BLANK 0 // First font tile is blank
#define FONT_8x16_TILE_ID_CHARS (FONT_8x16_BLANK_LEN)
#define FONT_8x16_TILE_ID_START  (FONT_8x16_TILE_ID_CHARS + FONT_8x16_CHARS_LEN)


uint16_t gfx_tiles_font_base = 0;

uint16_t str_digit_tiles[PRINT_MAX_DIGITS * FONT_8x16_TILE_HEIGHT];

uint16_t print_x  = 0;
uint16_t print_y  = 0;
uint16_t print_tile_attribs = PRINT_ATTRIB_PAL_DEFAULT;



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


void load_8x16_font_tilemap_palettes(void) {
   // Set palettes for tile printing (yellow, pink, grey) on BG0 layer
    set_bkg_4bpp_palette(PAL_ASSIGN_BG0_0, font_8x16_out_PALETTE_COUNT, font_8x16_out_palettes);
    set_bkg_4bpp_palette(PAL_ASSIGN_BG0_1, 1, font_8x16_out_palette_PINK);
    set_bkg_4bpp_palette(PAL_ASSIGN_BG0_2, 1, font_8x16_out_palette_GREY);
}


void load_8x16_font_sprite_palettes(void) {
    // Set palettes for Sprite printing (yellow, pink, grey) on OBJ layer(s)
    set_bkg_4bpp_palette(PAL_ASSIGN_OBJ_0, font_8x16_out_PALETTE_COUNT, font_8x16_out_palettes);
    set_bkg_4bpp_palette(PAL_ASSIGN_OBJ_1,  1, font_8x16_out_palette_PINK);
    set_bkg_4bpp_palette(PAL_ASSIGN_OBJ_2, 1, font_8x16_out_palette_GREY);
}

// Loads 8x16 font tiles into vram and sets gfx_tiles_font_base
// Returns next tile index after last used
uint16_t load_8x16_font_tiles(uint16_t tile_id_start) {

    gfx_tiles_font_base = tile_id_start;

    // Load font and it's default yellow palette
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_4bpp_data(gfx_tiles_font_base, font_8x16_out_TILE_COUNT, font_8x16_out_tiles);

     return (gfx_tiles_font_base + font_8x16_out_TILE_COUNT);
}

// Returns: Next OAM entry after last used
uint16_t print_to_sprites(uint16_t oam_id, uint8_t pal, const char * txt) {

    uint16_t c;
    uint16_t start_x = print_x;

    while (*txt) {

        if (*txt >= 'A' && *txt <= 'Z'){
            c = FONT_8x16_TILE_ID_CHARS + (unsigned char)(*txt - 'A');
        } else if(*txt >= 'a' && *txt <= 'z') {
            c = FONT_8x16_TILE_ID_CHARS + (unsigned char)(*txt - 'a');
        } else if(*txt >= '0' && *txt <= '9') {
            c = FONT_8x16_TILE_ID_START + (unsigned char)(*txt - '0');
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
                default: c = FONT_8x16_TILE_ID_BLANK; break;
            }
        }

        c *= FONT_8x16_TILE_HEIGHT + gfx_tiles_font_base; // 2 tiles per character
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


// TODO: tilemap print: so much duplicated code with sprite printing, find a way to merge them that's not annoying
void print_to_tilemap(const char * txt, uint16_t delay_time) {

    uint16_t c;
    uint16_t start_x = print_x;

    while (*txt) {

        if (*txt >= 'A' && *txt <= 'Z'){
            c = FONT_8x16_TILE_ID_CHARS + (unsigned char)(*txt - 'A');
        } else if(*txt >= 'a' && *txt <= 'z') {
            c = FONT_8x16_TILE_ID_CHARS + (unsigned char)(*txt - 'a');
        } else if(*txt >= '0' && *txt <= '9') {
            c = FONT_8x16_TILE_ID_START + (unsigned char)(*txt - '0');
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
                    print_y += BG_PRINT_SPACING_Y;
                    txt++;
                    continue;
                // Default is blank tile for Space or any other unknown chars
                default: c = FONT_8x16_TILE_ID_BLANK; break;
            }
        }

        c = (c * FONT_8x16_TILE_HEIGHT) + gfx_tiles_font_base; // 2 tiles per character
        c |= print_tile_attribs;           // Sets palette, screen, etc
        // Print top then bottom of character (2 tiles)
        set_bkg_tile_xy(print_x, print_y, c);
        set_bkg_tile_xy(print_x, print_y+1, c+1);

        print_x++;
        txt++;
        
/*        if (delay_time) {  // TODO: print delay time

            UPDATE_KEYS();
            // Skip delay if buttons pressed
            if (!KEY_PRESSED(J_B | J_A)) {
                // PLAY_SOUND_PRINT_CHAR;
                // delay(delay_time);
            }
        }
*/
    }
}



// TODO: Does this need a smaller 8x8 font? Looks easily do-able with a 1px drop shadow instead of 2px
// Render a font digit
void print_num_u16(uint16_t x, uint16_t y, uint16_t num, uint16_t fixed_str_length) {

    // Initialize index at END of array +1,
    // so that the first pass sets it to the first array position
    const uint16_t blank_tile = (FONT_8x16_TILE_ID_BLANK * FONT_8x16_TILE_HEIGHT) + gfx_tiles_font_base;  // * 2 is for two tiles per character
    uint16_t index = fixed_str_length;

    if (fixed_str_length > PRINT_MAX_DIGITS)
        fixed_str_length = PRINT_MAX_DIGITS;

    // Max output is 99999 (but won't reach that due to 16 bit unsigned)
    // Store individual digits of n in reverse order
    // Starting at the END of the array and working forward
    // (using do-while to handle when initial value == 0)
    //
    // Note: Small optimization could be to two digits at a time with %100 and have a LUT for 0-99 pairs
    do {
        // decrement the counter first, so it finishes as pointing to the current digit in the array
        index--;
        uint16_t chr  = (((num % 10) + FONT_8x16_TILE_ID_START) * FONT_8x16_TILE_HEIGHT) + gfx_tiles_font_base; // * 2 is for two tiles per character
        chr |= print_tile_attribs;           // Sets palette, screen, etc

        // Print top then bottom of character (2 tiles)
        str_digit_tiles[index] = chr;
        str_digit_tiles[index + fixed_str_length] = ++chr;  // Wrap around to next tile row down for bottom of character
        num = num / 10;
    } while (num != 0);

    // Fill remaining spaces with empty tiles
    while (index > 0) {
        index--;
        // Print top then bottom of character (2 tiles)
        str_digit_tiles[index] = blank_tile;
        str_digit_tiles[index + fixed_str_length] = blank_tile + 1;  // Wrap around to next tile row down
    }

    // Draw the digits on the background tilemap
    set_bkg_tiles(x, y, fixed_str_length, 2, // 1 tile high
                  &str_digit_tiles[index]); // Start at first digit and go to end of digits (length depends on fixed_str_length)
}
