// game_board.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "game_board.h"
#include "game_pieces.h"
#include "gfx.h"


UINT8 board_pieces[BRD_SIZE];
UINT8 board_attrib[BRD_SIZE];
UINT8 board_connect[BRD_SIZE];

UINT8 board_tile_clear_cache_x[BRD_SIZE];
UINT8 board_tile_clear_cache_y[BRD_SIZE];
UINT8 board_tile_clear_count;

// UINT8 board_connect_cache[BRD_CON_SIZE]; // TODO: this could be Bitpacked


void board_redraw_all(void) {
    // Update BG Tilemap from Game Board
    VBK_REG = 1; // Select BG tile attribute map
    set_bkg_tiles(BRD_ST_X, BRD_ST_Y,
                  BRD_WIDTH, BRD_HEIGHT,
                  &board_attrib[0]);

    // Update BG Attrib Map from Game Board
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


void board_debug_show_connect_entire_board(void) {

    INT8 x, y;
    // TODO: OPTIMIZE: convert to single loop counter

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = board_connect[x + (y * BRD_WIDTH)] + TILES_PET_START;

            // TODO: USE EMPTY PIECE PALETTE INSTEAD?
            // Set palette based on pet type
            board_attrib[x + (y * BRD_WIDTH)] = ((board_pieces[x + (y * BRD_WIDTH)] & GP_PET_MASK) >> GP_PET_UPSHIFT);

            // Reset board connection bits
            board_connect[x + (y * BRD_WIDTH)] = GP_CONNECT_NONE_BITS;
        }
    }

    board_redraw_all();
}



// Given X, Y board coordinate, is it open for a piece to move there?
//
UINT8 board_check_open_xy(INT8 x, INT8 y) {

    // TODO: Bounds checking for board here (could then remove from calling function)
    return (board_pieces[x + (y * BRD_WIDTH)] == (GP_EMPTY + TILES_PET_START));
}

/*
UINT8 board_piece_get(UINT8 x, UINT8 y, UINT8 * p_piece, UINT8 * p_rotate) {

    if ((x >= BRD_MIN_X) &&
        (x <= BRD_MAX_X) &&
        (y >= BRD_MIN_Y) &&
        (y <= BRD_MAX_Y))
        *p_piece = board_pieces[x + (y * BRD_WIDTH)];

        *p_rotate = ((*p_piece) & GP_ROT_MASK) >> GP_ROT_UPSHIFT;
        if (board_attrib[x + (y * BRD_WIDTH)] & (GP_MIRROR_X | GP_MIRROR_X)) // TODO: ROTATION HACK: yuck!
            *p_rotate += GP_ROTATE_180; // Extract remaining rotation if needed

        *p_rotate = board_attrib[x + (y * BRD_WIDTH)];
        return (true);
    else
        return (GP_EMPTY); // TODO Not well implemented-  should really return distinct error
}


void board_connect_update_xy(UINT8 x, UINT8 y, UINT8 piece, UINT8 connect) {

    UINT8 t_piece, t_rotate;
// Left  ( 0  deg)

    // Reset connection
    board_connect[x + (y * BRD_CON_WIDTH)] = BRD_CONNECT_NO;

    // Make sure pieces match
    if ( board_piece_get(x - 1, y, &t_piece, &t_rotate) ) {
        if  ((t_piece & GP_PET_MASK) == (piece  & GP_PET_MASK)) {

            if (GP_CONNECT_LUT[connect] &
                GP_CONNECT_MATCHING_LUT[board_connect_get(t_piece, t_rotate)]) {

                board_connect[x + (y * BRD_CON_WIDTH)] = BRD_CONNECT_YES;
            }
        }
    }

    // Up    ( 90 deg)
    // Right (180 deg)
    // Down  (270 deg)
}
*/

void board_clear_tile_xy(INT8 x, INT8 y) {

    UINT8 tile_index;

    tile_index = x + (y * BRD_WIDTH);

    // TODO: Bounds checking for board here
    board_pieces[tile_index] = GP_EMPTY + TILES_PET_START;
    // Set palette based on pet type (CGB Pal bits are 0x07)
    board_attrib[tile_index] = GP_PAL_EMPTY;
    // Update connection setting
    board_connect[tile_index] = GP_CONNECT_NONE_BITS;

    board_draw_tile_xy(x, y, tile_index);
}



void board_set_tile_xy(INT8 x, INT8 y, UINT8 piece, UINT8 attrib, UINT8 connect) {

    UINT8 tile_index;

    tile_index = x + (y * BRD_WIDTH);

    // Add in offset to start of BG tile piece data
    piece += TILES_PET_START;

    // TODO: Bounds checking for board here
    board_pieces[tile_index] = piece;
    // Set palette based on pet type (CGB Pal bits are 0x07)
    board_attrib[tile_index] = attrib;
    // Update connection setting
    board_connect[tile_index] = connect;

    //    board_connect_update_xy(x, y, piece, connect); // TODO REMOVE
    board_draw_tile_xy(x, y, tile_index);

    board_check_completed_pet_xy(x, y, piece, connect); // TODO: use result
    // if (board_check_completed_pet_xy(x, y, piece, connect)) {

    // }
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


UINT8 board_check_connected_xy(INT8 x, INT8 y, UINT8 piece, UINT8 * p_this_connect) {

    UINT8 adj_piece, adj_connect;

    // Get requested board piece
    if (board_piece_get_xy(x, y, &adj_piece, &adj_connect)) {

        // TODO: could check for if (adj_piece != GP_EMPTY), but the connect test below should cover that

        // Check connection between pieces
        if (GP_CONNECT_MATCHING_LUT[(*p_this_connect)] & adj_connect) {
            // Make sure the pet type matches
            if ((piece & GP_PET_MASK) == (adj_piece & GP_PET_MASK)) {
                // Conneection successful

                // Find and return next connect direction on this adjacent piece
                // by excluding current connection
                *p_this_connect = adj_connect & ~GP_CONNECT_MATCHING_LUT[(*p_this_connect)];
                return (TRUE);
            }
        }
    }

    return (FALSE);
}


// NOTE: no bounds checking, assumes incoming X,Y are valid board positions
//
// TODO: add mode (CHECK vs REMOVE TILES)
//
UINT8 board_check_completed_pet_xy(INT8 start_x, INT8 start_y, UINT8 piece, UINT8 connect) {

    UINT8 piece_count, headtail_count;
     INT8 next_x, next_y;
    UINT8 this_connect;
    UINT8 c;

    if (piece != GP_EMPTY) {

        // Reset tile clear cache (add one entry, current piece)
        board_tile_clear_cache_x[0] = start_x; // = start_x + (start_y * BRD_WIDTH);
        board_tile_clear_cache_y[0] = start_y;
        board_tile_clear_count = 1;

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
        for (c = GP_CONNECT_MIN_BITS; c <= GP_CONNECT_MAX_BITS; c <<= 1) {

            // If there is a connection in this direction
            if (c & connect) {

                // These need to be reset for each new direction
                // tested from original piece location
                next_x = start_x + GP_CONNECT_NEXT_X_LUT[c];
                next_y = start_y + GP_CONNECT_NEXT_Y_LUT[c];
                this_connect = connect;

                // The original piece gets used in this loop
                while (board_check_connected_xy(next_x, next_y, piece, &this_connect)) {

                    // Cache the tile location for clearing later if this pet is completed
                    board_tile_clear_cache_x[board_tile_clear_count] = next_x; // board_tile_clear_cache[board_tile_clear_count] = next_x + (next_y * BRD_WIDTH);
                    board_tile_clear_cache_y[board_tile_clear_count] = next_y;
                    board_tile_clear_count++;

                    // Add current piece to the total
                    piece_count++;

                    if (this_connect) {

                        // If the adjacent piece has another open connection then
                        // load the new X and Y location, then try it next (in this loop)
                        next_x += GP_CONNECT_NEXT_X_LUT[this_connect];
                        next_y += GP_CONNECT_NEXT_Y_LUT[this_connect];

                        // Check to see if the pet is a loop with no head/tail (next position == starting position)
                        if ((next_x == start_x) && (next_y == start_y)) {

                            // If so, the pet has been completed
                            // Now exit loop (without needing a head or tail, or dead-end )
                            headtail_count += 2;
                            break; // exit loop
                        }
                    } else {
                        // If next_connect is empty (only 1 connection, to previous tile)
                        // that means it's a HEAD or TAIL segment
                        // and this end is completed
                        headtail_count++;
                        break; // exit loop
                    }

                } // end: while board_check_connected_xy()
            } // end: if (c & connect)
        } // end: for (c=GP_CONNECT_MIN_BITS;

        // Check if a completed pet was found
        if ((headtail_count >= 2) && (piece_count >= 2)) {
            // TODO: re-call this function to remove pieces
            // TODO: count points/etc

            // clear the tiles
            for (c = 0; c < board_tile_clear_count; c++) {
                // TODO: OPTIMIZE: modulo is SLOW! convert to storing X/Y?
                // derive x,y from board index
                // next_x = (board_tile_clear_cache[c] % BRD_WIDTH);
                // next_y = (board_tile_clear_cache[c] - next_x) / BRD_WIDTH;
                board_clear_tile_xy(board_tile_clear_cache_x[c],
                                    board_tile_clear_cache_y[c]);
            }
            return (TRUE);
        }

    } // end: if (piece != GP_EMPTY)

    return (FALSE);
}

