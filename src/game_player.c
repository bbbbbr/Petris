// game_player.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "game_player.h"

#include "game_pieces.h"
#include "game_board.h"


// {
//    UINT8 x;
//    UINT8 y;
//    UINT8 piece;
// }

#define SPR_PLAYER 0 // Player is sprite "0"
UINT8 player_x = 120;
UINT8 player_y = 0;


// TODO: Sprite rotation (use LUT or DEFINES for rotation/mirroring bits)
void player_init(void) {

    // Mediocre-initialize the random number generator
    initarand(DIV_REG);
    SHOW_SPRITES;
}


void player_update_xy(INT8 dir_x, INT8 dir_y) {
    // test if direction is available
    // then update sprite if needed

    player_x += dir_x * 8;
    player_y += dir_y * 8;
    move_sprite(SPR_PLAYER, player_x, player_y);
}

void player_update_gfx(UINT8 new_piece) {

        set_sprite_tile(SPR_PLAYER, (new_piece & GP_TILE_MASK));

        set_sprite_prop(SPR_PLAYER, ((new_piece & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                          // TODO: Sprite mirror bits
                           );
}


//void player_visible(UINT8 do_show, INT8 X, INT8 Y) {
    // Show hide on/offscreen
    // move_sprite(SPR_PLAYER,
    //     stars_x[c],
    //     stars_y[c]);
//}


// #define S_FLIPX      0x20U
// #define S_FLIPY      0x40U
// #define S_CGB_PALETTE    0x03U




