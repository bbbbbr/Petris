// game_board.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"

#include "game_board.h"
#include "game_board_special_pieces.h"
#include "game_piece_data.h"
#include "gfx.h"
#include "gfx_print.h"
#include "sound.h"
#include "player_info.h"

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

            #ifdef CPU_FAST_ENABLED
                delay(delay_amount * 2);
            #else
                delay(delay_amount);
            #endif
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

        #ifdef CPU_FAST_ENABLED
            delay(1000);
        #else
            delay(500);
        #endif

        // print provided text
        PRINT(start_x, start_y, text,0);

        #ifdef CPU_FAST_ENABLED
            delay(1000);
        #else
            delay(500);
        #endif
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

    // TODO: Bounds checking for board here
    if ((x >= BRD_MIN_X) &&
        (x <= BRD_MAX_X) &&
        (y >= BRD_MIN_Y) &&
        (y <= BRD_MAX_Y))
    {
        tile_index = x + (y * BRD_WIDTH);

        board_pieces[tile_index] = GP_EMPTY + TILES_PET_START;
        // Set palette based on pet type (CGB Pal bits are 0x07)
        board_attrib[tile_index] = GP_PAL_EMPTY;
        // Update connection setting
        board_connect[tile_index] = GP_CONNECT_NONE_BITS;

        board_draw_tile_xy(x, y, tile_index);
    }
}






void board_set_tile_xy(INT8 x, INT8 y, UINT8 piece, UINT8 attrib, UINT8 connect) {

    UINT8 tile_index;

    // TODO: move this into a function if it grows board_set_special_xy(INT8 x, INT8 y, UINT8 piece, UINT8 attrib, UINT8 connect)
    if (piece & GP_SPECIAL_MASK) {

        switch (piece) {
            case GP_SPECIAL_BOMB:
                board_handle_special_bomb(x,y);
                break;

            case GP_SPECIAL_LIGHTENING:
                // NOTE: Piece is only passed in for testing, not put on the board.
                //       **DON'T** set pieces on board with more than 2 connection bits
                //       set (such as the 4-way Merge), it's an unhandled condition.
                board_check_completed_pet_xy(x,y, piece,
                                             GP_CONNECT_ALL_WAYS_BITS,
                                             BRD_CHECK_FLAGS_IGNORE_PET_TYPE);
                break;
        }

    } else {

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

        board_check_completed_pet_xy(x, y, piece, connect, BRD_CHECK_FLAGS_NONE); // TODO: use result
    }
}


void board_fill_random(void) {

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
        #ifdef CPU_FAST_ENABLED
            delay(300);
        #else
            delay(150);
        #endif
        c++;

        // TODO: increment score as the pieces clear?
        // TODO: animate the peices clearing (with sprite?)
    }

    //  Player gets no credit when special piece is used
    if (!(flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE))
        score_update((UINT16)board_tile_clear_count);


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
     INT8 check_x, check_y;
    UINT8 this_connect;
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
        if ( ((piece & GP_SEG_MASK) == GP_SEG_TAIL) ||
             ((piece & GP_SEG_MASK) == GP_SEG_HEAD) ) {
            headtail_count++;
        }


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


        // Check if a completed pet was found
        if ( ((headtail_count >= 2) && (piece_count >= 2)) ||
             (flags & BRD_CHECK_FLAGS_IGNORE_PET_TYPE)) {

            board_handle_pet_completed(flags);
            return (TRUE);
        }

//    } // end: if (piece != GP_EMPTY)
    return (FALSE);
}

