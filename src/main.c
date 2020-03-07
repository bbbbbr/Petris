#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions


#include "game_pieces.h"
#include "game_board.h"
#include "input.h"
#include "gfx.h"

void init (void);
void init_interrupts(void);
void vbl_update(void);


UINT8 vbl_count;


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

    gfx_init_background();
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
            board_reset();
        }
    }
}