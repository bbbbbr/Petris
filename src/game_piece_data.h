// game_piece_data.h

#ifndef GAME_PIECE_DATA_H
#define GAME_PIECE_DATA_H


// Pet tile bit mapping order
//
//  bits  :76543210
//  attrib:   SSRPP
//
//         Tile : 31 ...............16...12...8 ...4 ...0
//     Body Seg : HHHH HHHH LLLL LLLL MMMM MMMM TTTT TTTT
//     Rotation : VVVV HHHH VVVV HHHH VVVV HHHH VVVV HHHH
//     Pet Type : FSCD FSCD FSCD FSCD FSCD FSCD FSCD FSCD
//
//     (Head, L-shape, Midriff, Tail)
//
#define GP_EMPTY_MASK 0xE0U // .5+
#define GP_SPECIAL_MASK 0x20U // .5+
#define GP_SEG_MASK   0x18U // .4-3
#define GP_ROT_MASK   0x04U // .2
#define GP_PET_MASK   0x03U // .1-0

#define GP_PET_BITS_MASK  0x1FU

// Extra tiles above pet tiles
#define GP_EMPTY               0x20U // TODO : Tiles, move empty to be after specials?
#define GP_SPECIAL_START       0x21U
#define GP_SPECIAL_BOMB        0x21U
#define GP_SPECIAL_LIGHTENING  0x22U
#define GP_SPECIAL_BOMB_HINT   0x23U
#define GP_SPECIAL_MERGE_HINT  0x24U
#define GP_SPECIAL_DROP_HINT   0x25U
#define GP_DISSOLVE_1          0x26U
#define GP_DISSOLVE_2          0x27U
#define GP_DISSOLVE_3          0x28U
#define GP_CROSS               0x29U


#define GP_PET_UPSHIFT  0
#define GP_PET_DOG      0
#define GP_PET_CAT      1
#define GP_PET_FISH     2
#define GP_PET_SNAKE    3
#define GP_PET_NONE     4
#define GP_PET_MASK_NOSHIFT 0x03


#define GP_SEG_UPSHIFT  3
#define GP_SEG_TAIL     0
#define GP_SEG_TORSO    1
#define GP_SEG_TURN     2
#define GP_SEG_HEAD     3
#define GP_SEG_TAIL_BITS  (0x00U << GP_SEG_UPSHIFT)
#define GP_SEG_TORSO_BITS (0x01U << GP_SEG_UPSHIFT)
#define GP_SEG_TURN_BITS  (0x02U << GP_SEG_UPSHIFT)
#define GP_SEG_HEAD_BITS  (0x03U << GP_SEG_UPSHIFT)


#define GP_ROT_UPSHIFT  2
#define GP_ROT_HORZ     0
#define GP_ROT_VERT     1
#define GP_ROT_HORZ_BITS  (0x00U << GP_ROT_UPSHIFT)
#define GP_ROT_VERT_BITS  (0x01U << GP_ROT_UPSHIFT)

// Piece color palettes
#define GP_PAL_DOG      0x00U
#define GP_PAL_CAT      0x01U
#define GP_PAL_FISH     0x02U
#define GP_PAL_SNAKE    0x03U
#define GP_PAL_EMPTY    0x00U // Use first pet palette
#define GP_PAL_SPECIAL  0x04U
#define GP_PAL_DROPHINT 0x04U
#define GP_PAL_CROSS    0x07U
// NOTE: adding pieces here requires an update here: TILE_COUNT_PETTOTAL


#define GP_ROTATE_0        0
#define GP_ROTATE_90       1
#define GP_ROTATE_180      2
#define GP_ROTATE_270      3
#define GP_ROTATE_DEFAULT  GP_ROTATE_0
#define GP_ROTATE_MIN      GP_ROTATE_0
#define GP_ROTATE_MAX      GP_ROTATE_270
#define GP_ROTATE_SEG_TURN_180_AND_270_USE_MIRROR_BITS 0x02U // **DO NOT UPSHIFT**

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
extern const UINT8 GP_CONNECT_LUT[];
extern const UINT8 GP_CONNECT_MATCHING_LUT[];
extern const  INT8 GP_CONNECT_NEXT_X_LUT[];
extern const  INT8 GP_CONNECT_NEXT_Y_LUT[];


#define GP_CONNECT_WRAP_DOWNSHIFT 4

#define GP_CONNECT_MASK      0x0FU
#define GP_CONNECT_WRAP_MASK 0xF0U

#define GP_CONNECT_RESET  0x00U

#define GP_CONNECT_NONE_BITS   0x00U
#define GP_CONNECT_LEFT_BITS   0x01U
#define GP_CONNECT_UP_BITS     0x02U
#define GP_CONNECT_RIGHT_BITS  0x04U
#define GP_CONNECT_DOWN_BITS   0x08U
#define GP_CONNECT_MIN_BITS    GP_CONNECT_LEFT_BITS
#define GP_CONNECT_MAX_BITS    GP_CONNECT_DOWN_BITS
#define GP_CONNECT_ALL_WAYS_BITS (GP_CONNECT_LEFT_BITS | GP_CONNECT_RIGHT_BITS | GP_CONNECT_UP_BITS | GP_CONNECT_DOWN_BITS)

#endif // GAME_PIECE_DATA_H
