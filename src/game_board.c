// game_board.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_board.h"
#include "game_pieces.h"
#include "gfx.h"


UINT8 board_pieces[BRD_SIZE];
UINT8 board_attrib[BRD_SIZE];


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
void board_draw_tile(UINT8 x, UINT8 y, UINT8 tile_index) {
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

    UINT8 x,y;
    // TODO: OPTIMIZE: convert to single loop counter

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = GP_EMPTY + TILES_PET_START;
            // TODO: USE EMPTY PIECE PALETTE INSTEAD?
            // Set palette based on pet type
            board_attrib[x + (y * BRD_WIDTH)] = GP_PAL_EMPTY;
        }
    }

    board_redraw_all();
}



// Given X, Y board coordinate, is it open for a piece to move there?
//
UINT8 board_check_open(UINT8 x, UINT8 y) {

    // TODO: Bounds checking for board here (could then remove from calling function)
    return (board_pieces[x + (y * BRD_WIDTH)] == (GP_EMPTY + TILES_PET_START));
}


void board_set_tile(UINT8 x, UINT8 y, UINT8 piece) {

    UINT8 tile_index;

    tile_index = x + (y * BRD_WIDTH);

    // Add in offset to start of BG tile piece data
    piece +=  TILES_PET_START;

    // TODO: Bounds checking for board here
    board_pieces[tile_index] = piece;
    // Set palette based on pet type (CGB Pal bits are 0x07)
    board_attrib[tile_index] = ((piece & GP_PET_MASK) >> GP_PET_UPSHIFT);

    board_draw_tile(x, y, tile_index);
}


void board_fill_random(void) {

    UINT8 x,y;
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
