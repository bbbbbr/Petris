#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "../res/bg_map.h"
#include "../res/bg_tiles.h"

void init (void);
void init_background (void);

void init_interrupts(void);
void vbl_update(void);


void video_setflip(UINT8);


UINT8 vbl_count;

UINT8 keys;
UINT8 previous_keys;
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

void init_background (void) {

        set_bkg_palette(0, 8, bgPalette); // UBYTE first_palette, UBYTE nb_palettes, UWORD *rgb_data
        set_bkg_data(0, 63, bg_tiles);

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
            video_setflip(BG_FLIP_Y);
        }

    }
}