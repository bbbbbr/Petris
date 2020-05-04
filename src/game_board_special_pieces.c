// game_board_special_pieces.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"

#include "game_board.h"
#include "game_board_special_pieces.h"
#include "game_piece_data.h"
#include "gfx.h"
#include "sound.h"
#include "player_info.h"

void board_handle_special_bomb(UINT8 x,UINT8 y) {

    // TODO: visual effects and sound effects on the tiles
    //       maybe expand: board_clear_tile_xy() to set arbitray pieces and use it to flash the area
    //
    // Clear adjacent 4 tiles

    // TODO: OPTIMIZE: make a loop?
    // TODO: move the effects into board_clear_tile_xy?
    PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);
    board_clear_tile_xy(x - 1, y, BOARD_CLEAR_ANIM_ALWAYS);

        #ifdef CPU_FAST_ENABLED
            delay(300);
        #else
            delay(20);
        #endif

    PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);
    board_clear_tile_xy(x + 1, y, BOARD_CLEAR_ANIM_ALWAYS);

        #ifdef CPU_FAST_ENABLED
            delay(300);
        #else
            delay(20);
        #endif

    PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);
    board_clear_tile_xy(x, y - 1, BOARD_CLEAR_ANIM_ALWAYS);

        #ifdef CPU_FAST_ENABLED
            delay(300);
        #else
            delay(20);
        #endif

    PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0x24, 0x73, 0x86);
    board_clear_tile_xy(x, y + 1, BOARD_CLEAR_ANIM_ALWAYS);
}