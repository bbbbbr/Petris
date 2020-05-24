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

#include "common.h"

#include "game_board.h"
#include "game_board_special_pieces.h"
#include "game_piece_data.h"
#include "game_types.h"
#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "options.h"
#include "player_info.h"
#include "player_piece.h"
#include "player_hinting.h"

UINT8 board_pieces[BRD_SIZE];
UINT8 board_attrib[BRD_SIZE];
UINT8 board_connect[BRD_SIZE];

UINT8 board_tile_clear_cache_x[BRD_SIZE];
UINT8 board_tile_clear_cache_y[BRD_SIZE];
UINT8 board_tile_clear_count;

// 10 tiles wide for clearing the game board one row at a time
// This MUST match game board width
// TODO: move this to game_board_gfx.h
const UINT8 board_blank_row[] = {TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG,
                                 TILE_ID_BOARD_BLANK_BG};

const UINT8 board_x_row[] = {TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP,
                             TILE_ID_BOARD_UP};

const UINT8 board_blank_row_pal[] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};

const UINT8 gp_dissolve_anim[] = {GP_DISSOLVE_1 + TILES_PET_START,
                                  GP_DISSOLVE_2 + TILES_PET_START,
                                  GP_DISSOLVE_3 + TILES_PET_START};



// Initializes global vars for board
void board_init(void) {
    board_tile_clear_count = 0;
}

// Clears the game board
//
// * Called during PAUSE
void board_hide_all(UINT16 delay_amount) {

    INT8 y;

    // for (y = BRD_ST_Y; y < (BRD_ST_Y + BRD_HEIGHT); y++) {
    // Clear board from bottom up
    for (y = BRD_END_Y; y >= BRD_ST_Y; y--) {

        // Update BG Attribs from Game Board
        VBK_REG = 1;
        set_bkg_tiles(BRD_ST_X, y,
                      sizeof(board_blank_row_pal), 1,
                      &board_blank_row_pal[0]);

        // Update BG Tilemap from Game Board
        VBK_REG = 0;

        if (delay_amount) {
            set_bkg_tiles(BRD_ST_X, y,
                          sizeof(board_x_row), 1,
                          &board_x_row[0]);

            delay(delay_amount);
        }

        set_bkg_tiles(BRD_ST_X, y,
                      sizeof(board_blank_row), 1,
                      &board_blank_row[0]);
    }
}


// Redraws the game board from the game board arrays
void board_redraw_all(void) {

    // Update BG Attrib Map from Game Board
    VBK_REG = 1; // Select BG tile attribute map
    set_bkg_tiles(BRD_ST_X, BRD_ST_Y,
                  BRD_WIDTH, BRD_HEIGHT,
                  &board_attrib[0]);

    // Update BG Tilemap from Game Board
    VBK_REG = 0; // Re-Select regular BG tile map
    set_bkg_tiles(BRD_ST_X, BRD_ST_Y,
                  BRD_WIDTH, BRD_HEIGHT,
                  &board_pieces[0]);
}


// Redraws single tile on board
//
void board_draw_tile_xy(INT8 x, INT8 y, UINT8 tile_index) {
    // Update BG Tilemap from Game Board
    VBK_REG = 1; // Select BG tile attribute map
    set_bkg_tiles(BRD_ST_X + x, BRD_ST_Y + y,
                  1, 1,
                  &board_attrib[tile_index]);

    // Update BG Attrib Map from Game Board
    VBK_REG = 0; // Re-Select regular BG tile map
    set_bkg_tiles(BRD_ST_X + x, BRD_ST_Y + y,
                  1, 1,
                  &board_pieces[tile_index]);
}


void board_reset(void) {

    INT8 x, y;
    // TODO: OPTIMIZE: convert to single loop counter

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = GP_EMPTY + TILES_PET_START;

            // TODO: USE EMPTY PIECE PALETTE INSTEAD?
            // Set palette based on pet type
            board_attrib[x + (y * BRD_WIDTH)] = GP_PAL_EMPTY;

            // Reset board connection bits
            board_connect[x + (y * BRD_WIDTH)] = GP_CONNECT_NONE_BITS;
        }
    }

    board_redraw_all();
}


void board_flash_message(UINT8 start_x, UINT8 start_y, char * text, char * ctext, UINT8 repeat) {

    UINT8 c;

    // Hide the game board and player piece
    board_hide_all(BRD_CLR_DELAY_NONE);

    for (c = 0; c < repeat; c++) {
        // blank print text using the provided ctext
        PRINT(start_x, start_y, ctext,0);
        delay(500);

        // print provided text
        PRINT(start_x, start_y, text,0);
        delay(500);
    }
}


// Given x,y board coordinate, what is the lowest continuously open
// coordinate below the piece?
//
// Used for drop-piece hinting
//
// TODO: Bounds checking for board here (could then remove from calling function)
INT8 board_find_lowest_open_in_column(INT8 x, INT8 y_st) {

    UINT8 offset; // 8 bits is ok sicne the board array is smaller than 255
    INT8 y;

    // Start at the Y location of the player piece in board array
    y = y_st;
    offset = x + (y_st * BRD_WIDTH);

    // Keep checking until either the bottom of the board is reached,
    // or an occupied (non-open) tile is found
    while ((y <= BRD_MAX_Y) &&
           (board_pieces[offset] == (GP_EMPTY + TILES_PET_START))) {

        // Move to next row down and then test again
        y++;
        offset += BRD_WIDTH;
    }

    // Last free coordinate is -1 above the piece the loop stopped on
    return y - 1;
}



// Given X, Y board coordinate, is it open for a piece to move there?
//
UINT8 board_check_open_xy(INT8 x, INT8 y) {

    // TODO: Bounds checking for board here (could then remove from calling function)
    return (board_pieces[x + (y * BRD_WIDTH)] == (GP_EMPTY + TILES_PET_START));
}



// TODO: animation?
void board_clear_tile_xy(INT8 x, INT8 y) {

    UINT8 tile_index;
    UINT8 c;
    UINT8 is_tail = FALSE;

    // TODO: Bounds checking for board here
    if ((x >= BRD_MIN_X) &&
        (x <= BRD_MAX_X) &&
        (y >= BRD_MIN_Y) &&
        (y <= BRD_MAX_Y))
    {
        tile_index = x + (y * BRD_WIDTH);

        // Set the tail flag if the tile isn't empty and it's a tail (in any direction)
        if (board_pieces[tile_index] != GP_EMPTY + TILES_PET_START) {
            is_tail = ((board_pieces[tile_index] - TILES_PET_START) & GP_SEG_MASK) == GP_SEG_TAIL_BITS;
        }

        // Animate removal of board tile, even if blank - i,e called by a bomb/etc
        // Use attribs/color from existing tile as-is
        for (c=0; c < ARRAY_LEN(gp_dissolve_anim); c++) {

            board_pieces[tile_index] = gp_dissolve_anim[c];
            board_draw_tile_xy(x, y, tile_index);
            delay(40);
        }

        board_pieces[tile_index] = GP_EMPTY + TILES_PET_START;
        // Set palette based on pet type (CGB Pal bits are 0x07)
        board_attrib[tile_index] = GP_PAL_EMPTY;
        // Update connection setting
        board_connect[tile_index] = GP_CONNECT_NONE_BITS;


        board_draw_tile_xy(x, y, tile_index);

        // Last extra delay between subsequent tile clearings
        delay(20);
    }

    // Decrement tail counter if needed
    if (is_tail) {
        game_type_pet_cleanup_decrement_tail_count();
    }

}



// Should NOT be called if (piece & GP_SPECIAL_MASK)
void board_set_tile_xy(INT8 x, INT8 y, UINT8 piece, UINT8 attrib, UINT8 connect) {

    UINT8 tile_index;

    if (!(piece & GP_SPECIAL_MASK)) {

        if ((piece & GP_SEG_MASK) == GP_SEG_TAIL_BITS) {
            game_type_pet_cleanup_increment_tail_count();
        }

        tile_index = x + (y * BRD_WIDTH);

        // Add in offset to start of BG tile piece data
        piece += TILES_PET_START;

        // TODO: Bounds checking for board here
        board_pieces[tile_index] = piece;
        // Set palette based on pet type (CGB Pal bits are 0x07)
        board_attrib[tile_index] = attrib;
        // Update connection setting
        board_connect[tile_index] = connect;

        board_draw_tile_xy(x, y, tile_index);
    }
}



void board_handle_new_piece(INT8 x, INT8 y, UINT8 piece, UINT8 connect) {

    // TODO: move this into a function if it grows board_set_special_xy(INT8 x, INT8 y, UINT8 piece, UINT8 attrib, UINT8 connect)
    if (piece & GP_SPECIAL_MASK) {

        switch (piece) {
            case GP_SPECIAL_BOMB:
                board_handle_special_bomb(x,y);
                // Check to see if any piece clearing related level
                // changes need to happen. (such as if a tail was removed in Cleanup game mode)
                // * (board_tile_clear_count == 0)
                // * Also gets called by other pieces via board_check_completed_pet_xy()
                board_handle_pet_completed(BRD_CHECK_FLAGS_DONT_ADD_POINTS);
                break;

            case GP_SPECIAL_LIGHTENING:
                // NOTE: Piece is only passed in for testing, not put on the board.
                //       **DON'T** set pieces on board with more than 2 connection bits
                //       set (such as the 4-way Merge), it's an unhandled condition.
                board_check_completed_pet_xy(x,y, piece,
                                             GP_CONNECT_ALL_WAYS_BITS,
                                             BRD_CHECK_FLAGS_IGNORE_PET_TYPE
                                             | BRD_CHECK_FLAGS_DONT_ADD_POINTS);
                break;
        }
    } else {

        board_check_completed_pet_xy(x, y, piece, connect, BRD_CHECK_FLAGS_NONE); // TODO: use result
    }
}


void game_board_fill_random_tails(UINT8 tail_count) {

    UINT8 x, y;
    UINT8 piece;
    UINT8 attrib;
    UINT8 connect;

    while (tail_count) {

        // Choose X and Y coordinates randomly
        // and within board bounds
        x = (UINT8)DIV_REG % (BRD_WIDTH + 1);

        y = ((UINT8)DIV_REG % (BRD_HEIGHT - BRD_MIN_Y_RANDOM_FILL + 1)) + BRD_MIN_Y_RANDOM_FILL ;

        // Loop until the randomly selected spot is free
        if (board_piece_get_xy(x, y, &piece, &connect)) {

            if (piece == (GP_EMPTY + TILES_PET_START)) {

                // Only decrement the count of added tails
                // once one is successfully placed on the board
                tail_count--;

                piece = (((UINT8)DIV_REG & GP_PET_MASK_NOSHIFT) << GP_PET_UPSHIFT) |
                        (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
                        (GP_ROT_VERT << GP_ROT_UPSHIFT);

                // Set palette based on pet type (CGB Pal bits are 0x07)
                // And mirror bits based on rotation setting from LUT
                attrib = ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                          | GP_ROT_LUT_ATTR[GP_ROTATE_270];               // Rotation sprite mirror bits

                board_set_tile_xy((INT8)x, (INT8)y,
                                  piece, attrib,
                                  player_piece_connect_get(piece, GP_ROTATE_270));
            }
        }
    }

}

// TODO: remove debug function
/* void board_fill_random(void) {

    INT8 x, y;
    UINT8 piece;

    // piece = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
    //         (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
    //          GP_ROT_HORZ)
    //         + TILES_PET_START;

    // TODO: OPTIMIZE: convert to single loop counter

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // weak random, limit to 32 entries
            piece = ((UINT8)DIV_REG & 0x1F) + TILES_PET_START;
            //piece = (x  & 0x1F) + TILES_PET_START;

            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = piece;
            // Set palette based on pet type (CGB Pal bits are 0x07)
            board_attrib[x + (y * BRD_WIDTH)] = ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT);
        }
    }

    board_redraw_all();
}
*/


UINT8 board_piece_get_xy(INT8 x, INT8 y, UINT8 * p_piece, UINT8 * p_connect) {

    UINT8 tile_index;

    if ((x >= BRD_MIN_X) &&
        (x <= BRD_MAX_X) &&
        (y >= BRD_MIN_Y) &&
        (y <= BRD_MAX_Y)) {

        tile_index = x + (y * BRD_WIDTH);
        *p_piece = board_pieces[tile_index];
        *p_connect = board_connect[tile_index];

        return (TRUE);
    }
    else
        return (FALSE);
}


UINT8 board_check_connected_xy(INT8 x, INT8 y, UINT8 piece, UINT8 * p_this_connect, UINT8 flags) {

    UINT8 adj_piece, adj_connect;

    // Get requested board piece
    // (Fails if edge board piece and returns false to break out of calling loop)
    if (board_piece_get_xy(x, y, &adj_piece, &adj_connect)) {

        // TODO: could check for if (adj_piece != GP_EMPTY), but the connect test below should cover that

        // Check connection between pieces
        if (GP_CONNECT_MATCHING_LUT[(*p_this_connect)] & adj_connect) {
            // Make sure the pet type matches
            if ( ((piece & GP_PET_MASK) == (adj_piece & GP_PET_MASK)) ||
                 (flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)) {
                // Conneection successful

                // Find and return next connect direction on this adjacent piece
                // by excluding current connection
                *p_this_connect = adj_connect ^ GP_CONNECT_MATCHING_LUT[(*p_this_connect)];
                return (TRUE);
            }
        }
    }

    return (FALSE);
}


void board_handle_pet_completed(UINT8 flags) {

    UINT8 c = 0;

    while (c < board_tile_clear_count) {

        // TODO: OPTIMIZE: smaller arrays could be used. or pre-calc BG tile location)
        // TODO: OPTION: PET-PILE MODE (don't clear tiles, level is up and totaled when screen is filled

        // Play special sound (per tile) when more than N tiles have been cleared for a pet
        // TODO: or just play a single special sound when crossing the threshold instead?
        // TODO: suppress bonus threshold on special pieces? if (!(flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE))
        if (flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)
            PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86); // Special piece no points sound (bomb) // PlayFx(CHANNEL_1, 30, 0x45, 0xC3, 0x53, 0x00, 0x87);
        else if (c >= BRD_TILE_COUNT_BONUS_SOUND_THRESHOLD)
            PlayFx(CHANNEL_1, 30, 0x76, 0xC3, 0x53, 0x80, 0x87); // Bonus sound
        else
            PlayFx(CHANNEL_1, 30, 0x76, 0xC3, 0x53, 0x40, 0x87); // Normal sound

        // TODO: move the effects into board_clear_tile_xy?

        board_clear_tile_xy(board_tile_clear_cache_x[c],
                            board_tile_clear_cache_y[c]);
        c++;

        // TODO: increment score as the pieces clear?
        // TODO: animate the peices clearing (with sprite?)
    }

    //  Player gets no credit when special piece is used
    if (flags & BRD_CHECK_FLAGS_DONT_ADD_POINTS) {
        score_update(BRD_PIECE_CLEAR_COUNT_NONE);
    } else {
        // Check completed pet size against level-up size requirement if it's Long pet game type
        if (option_game_type == OPTION_GAME_TYPE_LONG_PET) {
            game_type_long_pet_check_size(board_tile_clear_count);
        }

        score_update((UINT16)board_tile_clear_count);
    }

    // Reset global pet size var
    board_tile_clear_count = 0;
}

// NOTE: no bounds checking, assumes incoming X,Y are valid board positions
//
// NOTE: Assumes that incompatible special pieces (Bomb/etc) are not passed in
// NOTE: IMPORTANT no piece on board should ever have more than 2 connection bits
//       set. It's ok to pass those in as params, but never land them on the board.
//
// NOTE: Special-MERGE piece is passed in, but *NOT* copied onto the board array beforehand.
//       Any connection that tests it's board location will find an EMPTY entry (no connects)
//
UINT8 board_check_completed_pet_xy(INT8 start_x, INT8 start_y, UINT8 piece, UINT8 connect, UINT8 flags) {

    UINT8 piece_count, headtail_count;
     INT8 check_x = 0, check_y = 0;  // Inits here and this_connect are just to quiet the compiler re long pet hinting
    UINT8 this_connect = 0;
    UINT8 last_connect;
    UINT8 source_cur_dir;


        // Reset tile clear cache (add one entry, current piece)
        board_tile_clear_cache_x[0] = start_x; // = start_x + (start_y * BRD_WIDTH);
        board_tile_clear_cache_y[0] = start_y;
        board_tile_clear_count = 1; // TODO: this could just piggy back on piece_count

        // Initialize piece count (include current)
        piece_count = 1;

        // Initialize end seg count
        headtail_count = 0;
        // If current piece is an end (tail or head) then increment the end_count
        if ( ((piece & GP_SEG_MASK) == GP_SEG_TAIL_BITS) ||
             ((piece & GP_SEG_MASK) == GP_SEG_HEAD_BITS) ) {
            headtail_count++;
        }


player_hinting_pet_length_remove(start_x, start_y);

        // Loop through possible connect directions (Left/Right/Up/Down)
        for (source_cur_dir = GP_CONNECT_MIN_BITS;
             source_cur_dir <= GP_CONNECT_MAX_BITS;
             source_cur_dir <<= 1) {

            // Stop searching if a pet has been completed
            if ( (headtail_count >= 2) &&
                 !(flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE))
                break;

            // If there is a connection in this direction
            if (source_cur_dir & connect) {

                // These need to be reset for each new direction
                // tested from original piece location
                check_x = start_x + GP_CONNECT_NEXT_X_LUT[source_cur_dir];
                check_y = start_y + GP_CONNECT_NEXT_Y_LUT[source_cur_dir];
                this_connect = source_cur_dir;

                // Store previous connection bit
                last_connect = this_connect;

                // The original piece gets used in this loop
                while (board_check_connected_xy(check_x, check_y, piece, &this_connect, flags)) {

                    // The loop is entered with :
                    // * check_x/y -> Not yet been updated (will be below)
                    //                Has current position just tested as connecting to the previous
                    //                Will get updated using the direction pointed to by: this_connect
                    //
                    // * this_connect -> Gets updated by board_check_connected_xy()
                    //                   Now has the next-connections bits pointing from the just-tested position
                    //                   toward the next x/y to check (with it's preceding direction bits masked out)

player_hinting_pet_length_remove(check_x, check_y);

                    // Check to see if the pet is a loop with no head/tail (current position == starting position)
                    // Important: must be tested *before* incrementing piece_count and updating check_x/Y
                    if ((check_x == start_x) && (check_y == start_y)) {

                        // If so, the pet has been completed
                        // Now exit loop (without needing a head or tail, or dead-end )
                        // Signal completion via adding 2 head/tails
                        headtail_count += 2;
                        break; // exit loop
                    }

                    // Cache the tile location for clearing later if this pet is completed
                    board_tile_clear_cache_x[board_tile_clear_count] = check_x; // board_tile_clear_cache[board_tile_clear_count] = check_x + (check_y * BRD_WIDTH);
                    board_tile_clear_cache_y[board_tile_clear_count] = check_y;
                    board_tile_clear_count++;

                    // Add current piece to the total
                    piece_count++;

                    if (this_connect) {

                        // If the adjacent piece has another open connection then
                        // load the new X and Y location, then try it next (in this loop)
                        check_x += GP_CONNECT_NEXT_X_LUT[this_connect];
                        check_y += GP_CONNECT_NEXT_Y_LUT[this_connect];

                    } else {

                        // If check_connect is empty (only 1 connection, to previous tile)
                        // that means it's a HEAD or TAIL segment
                        // and this end is completed
                        headtail_count++;
                        break; // exit loop
                    }

                    // Store previous connection bit
                    last_connect = this_connect;

                } // end: while board_check_connected_xy()

                // Prevent special-merge from re-testing already
                // handled pieces (in case of a loop shape)
                if ((check_x == start_x) && (check_y == start_y)
                    && (flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)) {
                    // Clear the (copied) connection for the original piece
                    // connection that the loop just completed on
                    connect ^= GP_CONNECT_MATCHING_LUT[(last_connect)];
                }

            } // end: if (c & connect)
        } // end: for (c=GP_CONNECT_MIN_BITS;

// BUG-1: bomb is completing pets
// BUG-2: bomb needs to remove pet segments before re-testing pet for removal
        // Bug-2A (predicted) - need to remove hint before bomb removes piece
        // Move hint deletion into bomb special instead of in here?
        // (use cached list of pet tiles)
        // it could be cleaner... but still have to print pet length here
        // Bug-2B, removing piece before would prevent proper testing
        //  of whether removed piece actually connected to a pet
        // so instead maybe remove it after using cached list?
        // (it would be first in list)
//
        if ( ((headtail_count >= 2) && (piece_count >= 2)) ||
             (flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)) {

            if (!(flags & BRD_CHECK_FLAGS_DONT_HANDLE_PET_COMPLETED)) {

                // Check if a completed pet was found
                board_handle_pet_completed(flags);
                return (TRUE);
            }
        } else {

            // Reduncant with above, don't check
            // // Only process if it's not a special Merge piece
            // if (!(flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)) {


            // Only update Pet length info in Long Pet mode
            // Don't update if called via a bomb special piece
            if ((option_game_type == OPTION_GAME_TYPE_LONG_PET) &&
                (piece_count > 1) &&
                (!(flags & BRD_CHECK_FLAGS_DONT_HANDLE_PET_COMPLETED)))
            {
                // Display last length of completed segment
                // even if it's not a full pet
                print_num_u16(DISPLAY_NUMPETS_X + 1, DISPLAY_NUMPETS_Y + 1, (UINT16)piece_count, DIGITS_2);

                // if (piece_count == game_type_long_pet_required_size - 1) {
                if (piece_count >= game_type_long_pet_required_size - 1) {
                    // If current piece is an end (tail or head)
                    // then use the opposite piece
                    if ( ((piece & GP_SEG_MASK) == GP_SEG_TAIL_BITS) ||
                         ((piece & GP_SEG_MASK) == GP_SEG_HEAD_BITS) ) {
                        // Piece landed was a head/tail, so put hint at opposite end
                        // Fix x,y location being 1 past the end of pet
                        // by reverse the next piece connection lookup
                        player_hinting_pet_length_add(check_x - GP_CONNECT_NEXT_X_LUT[this_connect],
                                                      check_y - GP_CONNECT_NEXT_Y_LUT[this_connect]);
                    } else {
                        // piece landed was non head/tail so put hint on landed piece
                        player_hinting_pet_length_add(start_x, start_y);
                    }
                }
            }
//            }

        }

//    } // end: if (piece != GP_EMPTY)

    // Reset global pet size var now that processing is completed
    board_tile_clear_count = 0;

    return (FALSE);
}

