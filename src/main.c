#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

// Assets & Resources
#include "../res/bg_map.h"
#include "../res/bg_tiles.h"
#include "../res/pet_tiles.h"

//
#include "game_pieces.h"

void init (void);
void init_background (void);
void init_interrupts(void);

void vbl_update(void);


void video_setflip(UINT8);
void board_fill_random(void);


UINT8 vbl_count;

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
#define UPDATE_KEYS() previous_keys = keys; keys = joypad()

#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K)   ((keys & (K)) && !(previous_keys & (K)))
#define KEY_RELEASED(K) ((previous_keys & (K)) && !(keys & (K)))

#define ANY_KEY_PRESSED (keys)
#define ANY_KEY_TICKED  (keys != previous_keys) // TODO: this is sloppy

#define BG_PRIORITY 0x80
#define BG_FLIP_Y   0x40
#define BG_FLIP_X   0x20


// These palettes replace the default palette with the appropriate colors. See cgb.h for more defined colors.

UWORD bgPalette[] = {
    bg_tilesCGBPal0c0, bg_tilesCGBPal0c1, bg_tilesCGBPal0c2, bg_tilesCGBPal0c3,
    bg_tilesCGBPal1c0, bg_tilesCGBPal1c1, bg_tilesCGBPal1c2, bg_tilesCGBPal1c3,
    bg_tilesCGBPal2c0, bg_tilesCGBPal2c1, bg_tilesCGBPal2c2, bg_tilesCGBPal2c3,
    bg_tilesCGBPal3c0, bg_tilesCGBPal3c1, bg_tilesCGBPal3c2, bg_tilesCGBPal3c3,
    bg_tilesCGBPal4c0, bg_tilesCGBPal4c1, bg_tilesCGBPal4c2, bg_tilesCGBPal4c3,
    bg_tilesCGBPal5c0, bg_tilesCGBPal5c1, bg_tilesCGBPal5c2, bg_tilesCGBPal5c3,
    bg_tilesCGBPal6c0, bg_tilesCGBPal6c1, bg_tilesCGBPal6c2, bg_tilesCGBPal6c3,
    bg_tilesCGBPal7c0, bg_tilesCGBPal7c1, bg_tilesCGBPal7c2, bg_tilesCGBPal7c3,
};

void vbl_update() {
    vbl_count ++;
}


void init_interrupts() {
    disable_interrupts();
    add_VBL(vbl_update);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();
}

void init (void) {
    // Switch CGB fast mode
    cpu_fast();
    SPRITES_8x8;

    init_background();
}

#define TILES_BG_START       0
#define TILES_PET_START     64
#define TILE_COUNT_BG       16
#define TILE_COUNT_PETS     32
#define TILE_COUNT_PETBLANK  2
#define TILE_COUNT_PETTOTAL  TILE_COUNT_PETS + TILE_COUNT_PETBLANK

void init_background (void) {

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        set_bkg_data(TILES_BG_START, TILE_COUNT_BG, bg_tiles);
        set_bkg_data(TILES_PET_START, TILE_COUNT_PETTOTAL, pet_tiles);

        // Load BG tile attribute map
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN1);

        // Load BG tile map
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 20, 18, bg_mapPLN0);

        SHOW_BKG;

        DISPLAY_ON;
}

void video_setflip(UINT8 bg_attrib) {

    // int x,y;
    // UINT8 video_buff;

    // VBK_REG = 1; // Select BG tile attribute map

    //     for (x=0; x < 20; x++) {
    //         for (y=0; y < 18; y++) {

    //             get_bkg_tiles(x, y, 1, 1, &video_buff);
    //             video_buff ^= bg_attrib;
    //             set_bkg_tiles(x, y, 1, 1, &video_buff);
    //         }
    //     }

    // VBK_REG = 0; // Re-Select regular BG tile map


    #define VBUF_SIZE 18 * 20
    int x;
    UINT8 video_buff[ VBUF_SIZE ];

    VBK_REG = 1; // Select BG tile attribute map

        get_bkg_tiles(0, 0, 20, 18, &video_buff[0]);

        for (x=0; x < VBUF_SIZE; x++) {
            video_buff[x] ^= bg_attrib;
        }

        set_bkg_tiles(0, 0, 20, 18, &video_buff[0]);

    VBK_REG = 0; // Re-Select regular BG tile map

}

#define BRD_ST_X   8
#define BRD_ST_Y   0
#define BRD_WIDTH  10
#define BRD_HEIGHT 17
#define BRD_SIZE   BRD_WIDTH * BRD_HEIGHT

#define CGB_BG_ATTRIB_PAL_MASK 0x03

UINT8 board_pieces[BRD_SIZE];
UINT8 board_attrib[BRD_SIZE];

void board_fill_random(void) {

    UINT8 x,y;
    UINT8 piece;

    // piece = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
    //         (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
    //          GP_ROT_HORZ)
    //         + TILES_PET_START;;

    for (x=0; x < BRD_WIDTH; x++) {
        for (y=0; y < BRD_HEIGHT; y++) {
            // weak random, limit to 32 entries
            piece = ((UINT8)DIV_REG & 0x1F) + TILES_PET_START;
            //piece = (x  & 0x1F) + TILES_PET_START;

            // Set pet piece
            board_pieces[x + (y * BRD_WIDTH)] = piece;
            // Set palette based on pet type (
            board_attrib[x + (y * BRD_WIDTH)] = (piece >> GP_PET_UPSHIFT) & CGB_BG_ATTRIB_PAL_MASK;
        }
    }

    // Update BG Tilemap from Game Board
    VBK_REG = 1; // Select BG tile attribute map
    set_bkg_tiles(BRD_ST_X, BRD_ST_Y,
                  BRD_WIDTH, BRD_HEIGHT,
                  &board_attrib[0]);

    VBK_REG = 0; // Re-Select regular BG tile map
    set_bkg_tiles(BRD_ST_X, BRD_ST_Y,
                  BRD_WIDTH, BRD_HEIGHT,
                  &board_pieces[0]);

}

void main(void){
    init();

    while(1) {
        // Wait for vertical blank
        // (skip if already happened)
        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;

        UPDATE_KEYS();

        // if (ANY_KEY_TICKED) {
        //     video_setflip(keys);
        // }
        if (KEY_TICKED(J_A)) {
            video_setflip(BG_FLIP_X);
        }
        else if (KEY_TICKED(J_B)) {
            //video_setflip(BG_FLIP_Y);
            board_fill_random();
        }
        else if (KEY_TICKED(J_START)) {
            //board_fill_random();
        }
    }
}