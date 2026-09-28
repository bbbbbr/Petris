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

// game_board.c

#include <gbdk/platform.h>
#include <gbdk/rand.h>
#include <stdint.h>
#include <stdbool.h>

#include "audio_common.h"
#include "gfx.h"
#include "gfx_print.h"
#include "input.h"

#include "game_piece_data.h"
#include "game_board.h"
#include "game_stats.h"
#include "gameover_message.h"

#include "player_info.h"
#include "player_hinting.h"


#define MAXPET_TXT_X       BRD_ST_X + 0
#define MAXPET_TXT_Y       BRD_ST_Y + 0
#define MAXPET_TXT_MSG    " BEST PET " // " LONGEST  "
#define MAXPET_TXT_DELAY  100
#define MAXPET_TILE_DELAY 120
#define MAXPET_MIN_DISPLAY_SIZE 2

#define STATS_TXT_DELAY 80
#define STATS_NUM_DELAY 35
#define STATS_NUM_STEPS 20

#define STATS_TXT_X     BRD_ST_X + 1
#define STATS_TXT_Y     BRD_ST_Y + 1
#define STATS_TXT_OFST  5
#define PETS_TXT_MSG "PETS:"
#define TILES_TXT_MSG "TILES:"
#define PIECES_TXT_MSG "PIECES:"


uint8_t maxpet_pieces[BRD_SIZE];
uint8_t maxpet_attrib[BRD_SIZE];
uint8_t maxpet_x[BRD_SIZE]; // (x << 4 | (y & 0x0f)?
uint8_t maxpet_y[BRD_SIZE];
uint8_t maxpet_tilecount = 0;


void stats_maxpet_reset(void) {

    maxpet_tilecount = 0;
}



// Copy current completed pet if it's the new longest pet
void stats_maxpet_copy_iflongest(void) {

    uint8_t c;
    uint8_t tile_index;


    if (board_tile_clear_count > maxpet_tilecount) {

        // Update cached maxpet size
        maxpet_tilecount = board_tile_clear_count;

        for(c=0; c < maxpet_tilecount; c++) {

            // These are all being copied from globals
            maxpet_x[c] = board_tile_clear_cache_x[c];
            maxpet_y[c] = board_tile_clear_cache_y[c];

            // Calc index into baord arrays
            tile_index = board_tile_clear_cache_x[c]
                         + (board_tile_clear_cache_y[c] * BRD_WIDTH);

            maxpet_pieces[c] = board_pieces[tile_index];
            maxpet_attrib[c] = board_attrib[tile_index];
        }
    }
}



void stats_maxpet_draw(void) {

    uint8_t c, max_x;

    board_hide_all(BRD_CLR_DELAY_CLEAR_MED);
    board_reset();

    PRINTXY(MAXPET_TXT_X, MAXPET_TXT_Y,
          MAXPET_TXT_MSG, MAXPET_TXT_DELAY);

    // Draw each tile from the longest pet with delay between each
    max_x = 0;

    for(c=0; c < maxpet_tilecount; c++) {

        PLAY_SOUND_TILE_CLEAR_BONUS;

        // Track right-most piece for subsequent length overlay
        if (maxpet_x[c] > max_x) max_x = maxpet_x[c];

        board_set_tile_xy((int8_t)maxpet_x[c],
                          (int8_t)maxpet_y[c],
                          maxpet_pieces[c],
                          maxpet_attrib[c],
                          GP_CONNECT_NONE_BITS);

        delay(MAXPET_TILE_DELAY);
    }

    // Show the pet length overlay
    hinting_petlength_reset();
    hinting_petlength_add(max_x + 1,
                          maxpet_y[0],
                          maxpet_tilecount,
                          maxpet_pieces[0]);
    hinting_petlength_turn_on();
    hinting_petlength_showhide();
}



// Display and End of Game stats readout
void stats_display(void) {

    // == Show longest pet size ==

    if (maxpet_tilecount >= MAXPET_MIN_DISPLAY_SIZE) {

        stats_maxpet_draw();
        // Wait for a button press to continue
        waitpadticked_lowcpu(J_START | J_A | J_B);
    }

    // == Numeric Stats Readout ==

    // Re-hide pet length overlay
    hinting_petlength_reset();


    board_hide_all(BRD_CLR_DELAY_CLEAR_MED);

    // Update keys here to allow for pressing
    // buttons A/B to speed through text printing
    stats_show_var(STATS_TXT_X, STATS_TXT_Y, PETS_TXT_MSG,
                   player_numpets); // Pets Completed

    stats_show_var(STATS_TXT_X, STATS_TXT_Y + STATS_TXT_OFST, TILES_TXT_MSG,
                   player_numtiles); // Tiles Cleared

    stats_show_var(STATS_TXT_X, STATS_TXT_Y + (STATS_TXT_OFST * 2), PIECES_TXT_MSG,
                   player_numpieces);  // Pieces player

}



// Display a text label and an incrementing numeric readout for a value
void stats_show_var(uint8_t x, uint8_t y, const char* text, uint16_t value) {

    uint16_t c, increment;

    // Set display increment
    increment = value / STATS_NUM_STEPS;
    if (increment == 0) increment++;

    PRINTXY(x, y, text, STATS_TXT_DELAY);

    // Ramp value up from zero to it's value in N steps
    c = 0;
    print_num_u16(x + 3, y + 2, c, STR_DIGIT_LEN_5); // print initial value

    while(c < value) {

        // Increments in integer don't guarantee it will hit final
        // value evenly, so enforce that once it is reached or passed
        c += increment;
        if (c > value) c = value;

        print_num_u16(x + 3, y + 2, c, STR_DIGIT_LEN_5);

        UPDATE_KEYS();
        // Skip delay and sound if buttons pressed
        if (!KEY_PRESSED(J_B | J_A)) {
            PLAY_SOUND_PRINT_CHAR;
            delay(STATS_NUM_DELAY);
        }
    }

}
