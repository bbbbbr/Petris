// game_board.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_board.h"
#include "game_pieces.h"
#include "gfx.h"


#define BRD_ST_X   8
#define BRD_ST_Y   0
#define BRD_WIDTH  10
#define BRD_HEIGHT 17
#define BRD_SIZE   BRD_WIDTH * BRD_HEIGHT

#define CGB_BG_ATTRIB_PAL_MASK 0x03

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


void board_reset(void) {

    UINT8 x,y;
    // TODO: OPTIMIZE: convert to single loop counter

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = GP_EMPTY;
            // TODO: USE EMPTY PIECE PALETTE INSTEAD?
            // Set palette based on pet type
            board_attrib[x + (y * BRD_WIDTH)] = GP_PAL_EMPTY;
        }
    }

    board_redraw_all();
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
