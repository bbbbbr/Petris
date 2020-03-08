// game_pieces.h

#ifndef GAME_PIECES_H
#define GAME_PIECES_H



#define GP_EMPTY_MASK 0xE0U // .5+
#define GP_ROT_MASK   0x10U // .4
#define GP_PET_MASK   0x0CU // .3-2
#define GP_SEG_MASK   0x03U // .1-0

#define GP_TILE_MASK  0x1FU

#define GP_EMPTY      0x20U

#define GP_PET_UPSHIFT  2
#define GP_PET_DOG      0
#define GP_PET_CAT      1
#define GP_PET_FISH     2
#define GP_PET_SNAKE    3

#define GP_SEG_UPSHIFT  0
#define GP_SEG_TAIL     0
#define GP_SEG_TORSO    1
#define GP_SEG_TURN     2
#define GP_SEG_HEAD     3
#define GP_SEG_TURN_BITS 0x2U

#define GP_ROT_UPSHIFT  4
#define GP_ROT_HORZ     0
#define GP_ROT_VERT     1
#define GP_ROT_HORZ_BITS  0x00
#define GP_ROT_VERT_BITS  0x10

// Piece color palettes
#define GP_PAL_DOG      0x00U
#define GP_PAL_CAT      0x01U
#define GP_PAL_FISH     0x02U
#define GP_PAL_SNAKE    0x03U
#define GP_PAL_EMPTY    0x03U // 0x04U // TODO FIXME

#define GP_ROTATE_0        0
#define GP_ROTATE_90       1
#define GP_ROTATE_180      2
#define GP_ROTATE_270      3
#define GP_ROTATE_DEFUALT  GP_ROTATE_0
#define GP_ROTATE_MIN      GP_ROTATE_0
#define GP_ROTATE_MAX      GP_ROTATE_270
#define GP_ROTATE_SEG_TURN_MIRROR_BITS 0x02U

// BG Map Attributes (CGB Mode only)
// Bit 0-2  Background Palette number  (BGP0-7)
// Bit 3    Tile VRAM Bank number      (0=Bank 0, 1=Bank 1)
// Bit 4    Not used
// Bit 5    Horizontal Flip            (0=Normal, 1=Mirror horizontally)
// Bit 6    Vertical Flip              (0=Normal, 1=Mirror vertically)
// Bit 7    BG-to-OAM Priority         (0=Use OAM priority bit, 1=BG Priority)

#define GP_MIRROR_OFF  0x00U
#define GP_MIRROR_X    S_FLIPX
#define GP_MIRROR_Y    S_FLIPY

extern const UINT8 GP_ROT_LUT_TILE[];
extern const UINT8 GP_ROT_LUT_ATTR[];


#endif // GAME_PIECES_H
