// game_piece.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <rand.h>

#include "game_piece.h"
#include "game_piece_data.h"

UINT8 game_piece_next;
UINT8 game_piece_next_override;



void game_piece_next_generate(void) {

    // Check to see if there is a special override for the next game piece
    if (game_piece_next_override != GAME_PIECE_NEXT_OVERRIDE_NONE) {

        game_piece_next = game_piece_next_override;

    } else {

        // TODO: IMPROVE NEW PIECE SELECTION
        // For now, choose single random pet tile (using div register)
        game_piece_next = ((UINT8)DIV_REG & 0x1F);
        // game_piece = ((GP_PET_DOG  << GP_PET_UPSHIFT) |
        //                 (GP_SEG_TAIL << GP_SEG_UPSHIFT) |
        //                  GP_ROT_HORZ);// + TILES_PET_START;
    }

    // Clear override
    game_piece_next_override = GAME_PIECE_NEXT_OVERRIDE_NONE;
}



UINT8 game_piece_next_get(void) {
    return game_piece_next;
}



void game_piece_next_set(UINT8 override_piece) {
    game_piece_next_override = override_piece;
}



void game_piece_next_show(UINT8 do_show) {

    // if (game_difficulty_get() == GAME_DIFFICULTY_EASY)  // TODO

    // TODO: this could get animated to launch the next piece
    //       to the top of the board

    if (do_show) {

        // TODO: consolidate sprite calculation with player_piece_update_gfx() where this is duplicated from?

        // Update player rotation (clear rotate bits and then set)
        set_sprite_tile(SPR_PLAYER_NEXT,
                        (game_piece_next & ~GP_ROT_MASK)
                        | GP_ROT_LUT_TILE[GP_ROTATE_DEFAULT]);

        // Set palette based on pet type (CGB Pal bits are 0x07)
        // And mirror bits based on rotation setting from LUT
        set_sprite_prop(SPR_PLAYER_NEXT,
                        ((game_piece_next & GP_PET_MASK) >> GP_PET_UPSHIFT) // Palette
                        | GP_ROT_LUT_ATTR[GP_ROTATE_DEFAULT]);               // Rotation sprite mirror bits




        move_sprite(SPR_PLAYER_NEXT,
                    GAME_PIECE_NEXT_PREVIEW_X,
                    GAME_PIECE_NEXT_PREVIEW_Y);
    } else {
        // Hide sprite
        move_sprite(SPR_PLAYER_NEXT, 0,0);
    }
}
