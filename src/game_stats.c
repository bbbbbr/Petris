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

// game_board.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>
#include <rand.h>

#include "audio_common.h"
#include "gfx.h"
#include "gfx_print.h"
#include "game_piece_data.h"
#include "game_board.h"
#include "player_info.h"
#include "player_hinting.h"

#include "gameover_message.h"
#include "input.h"

#include "game_stats.h"


#define MAXPET_TXT_X       BRD_ST_X + 0
#define MAXPET_TXT_Y       BRD_ST_Y + 0
#define MAXPET_TXT_MSG    " BEST PET " // " LONGEST  "
#define MAXPET_TXT_DELAY  100
#define MAXPET_TILE_DELAY 120
#define MAXPET_MIN_DISPLAY_SIZE 3

#define STATS_TXT_DELAY 80
#define STATS_NUM_DELAY 35
#define STATS_NUM_STEPS 20

#define TILES_TXT_X  BRD_ST_X + 1
#define TILES_TXT_Y  BRD_ST_Y + 1
#define PETS_TXT_MSG "PETS  \nCLEARED:"
#define PIECES_TXT_MSG "PIECES \nPLAYED:"
#define TILES_TXT_MSG "TILES \nCLEARED:"


UINT8 maxpet_pieces[BRD_SIZE];
UINT8 maxpet_attrib[BRD_SIZE];
UINT8 maxpet_x[BRD_SIZE]; // (x << 4 | (y & 0x0f)?
UINT8 maxpet_y[BRD_SIZE];
UINT8 maxpet_tilecount;


void stats_maxpet_reset(void) {

    maxpet_tilecount = 0;
}


void stats_maxpet_copy_iflongest(void) {

    UINT8 c;
    UINT8 tile_index;

    // Copy current completed pet if it's the new longest pet
    if (board_tile_clear_count > maxpet_tilecount) {

        maxpet_tilecount = board_tile_clear_count;

        for(c=0; c < maxpet_tilecount; c++) {

            // These are all being copied from globals
            maxpet_x[c] = board_tile_clear_cache_x[c];
            maxpet_y[c] = board_tile_clear_cache_y[c];

            tile_index = board_tile_clear_cache_x[c]
                         + (board_tile_clear_cache_y[c] * BRD_WIDTH);

            // To extract the piece from the board, strip the TILES_PET_START offset
            maxpet_pieces[c] = board_pieces[tile_index] - TILES_PET_START;
            maxpet_attrib[c] = board_attrib[tile_index];
        }
    }
}


void stats_maxpet_display(void) {

    UINT8 c, max_x;

    if (maxpet_tilecount >= MAXPET_MIN_DISPLAY_SIZE) {

        board_hide_all(BRD_CLR_DELAY_CLEAR_MED);
        board_reset();

        PRINT(MAXPET_TXT_X, MAXPET_TXT_Y,
              MAXPET_TXT_MSG, MAXPET_TXT_DELAY);

        max_x = 0;
        for(c=0; c < maxpet_tilecount; c++) {

            PLAY_SOUND_TILE_CLEAR_BONUS;
            // Track right-most piece for subsequent length overlay
            if (maxpet_x[c] > max_x) max_x = maxpet_x[c];

            board_set_tile_xy((INT8)maxpet_x[c],
                              (INT8)maxpet_y[c],
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


        // == Numeric Stats Readout ==

        waitpadticked_lowcpu(J_START | J_A | J_B, NULL);
        // Re-hide pet length overlay
        hinting_petlength_reset();


        board_hide_all(BRD_CLR_DELAY_CLEAR_MED);

        stats_show_var(TILES_TXT_X, TILES_TXT_Y, PETS_TXT_MSG,
                       player_numpets);

        stats_show_var(TILES_TXT_X, TILES_TXT_Y + 4, PIECES_TXT_MSG,
                       player_numpieces);

        stats_show_var(TILES_TXT_X, TILES_TXT_Y + 8, TILES_TXT_MSG,
                       player_numtiles);
    }
}

void stats_show_var(UINT8 x, UINT8 y, const char* text, UINT16 value) {

    UINT16 c, increment;

    // Set display increment
    increment = value / STATS_NUM_STEPS;
    if (increment == 0) increment++;

    PRINT(x, y, text, STATS_TXT_DELAY);

    c = 0;
    while(c < value) {

        c += increment;
        if (c > value) c = value;

        print_num_u16(x + 3, y + 2, c, DIGITS_5);
        PLAY_SOUND_PRINT_CHAR;
        delay(STATS_NUM_DELAY);
    }

}