// game_pieces.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_pieces.h"


// This gets applied to BG Tile/Sprite INDEX:0x10
const UINT8 GP_ROT_LUT_TILE[4] =
                {GP_ROT_HORZ_BITS,     // GP_ROTATE_0
                 GP_ROT_VERT_BITS,     // GP_ROTATE_90
                 GP_ROT_HORZ_BITS,     // GP_ROTATE_180
                 GP_ROT_VERT_BITS};    // GP_ROTATE_270

// This gets BG Tile/Sprite ATTRIB:0x60
const UINT8 GP_ROT_LUT_ATTR[4] =
                {GP_MIRROR_OFF,
                 GP_MIRROR_OFF,
                 GP_MIRROR_X,
                 GP_MIRROR_Y };

