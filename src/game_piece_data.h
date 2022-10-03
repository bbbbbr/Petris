// game_piece_data.h

#ifndef GAME_PIECE_DATA_H
#define GAME_PIECE_DATA_H


// Locations for loading Game Piece tile sprite data
#define SPRITE_TILE_PET_START  0U // Load at start of sprite tile RAM
#define SPRITE_TILE_SPECIAL_START  (SPRITE_TILE_PET_START + TILE_COUNT_PETTOTAL) // Load directly after pet tiles

// Start sprite font digit tiles after pet tiles
#define SPRITE_TILE_FONT_DIGITS_START (SPRITE_TILE_SPECIAL_START + TILE_COUNT_SPECIALTOTAL)
#define SPRITE_TILE_FONT_CHARS_START (SPRITE_TILE_FONT_DIGITS_START + TILE_COUNT_FONT_NUMS)




// Pet tile bit mapping order
//
//  Old
//  bits  :76543210
//  attrib:   SSRPP

//  New/GG
//  bits  :76543210
//  attrib:  SSRRPP
//
//  S[0-3]: Body-Segment
//  R[0-3]: Rotation
//  P[0-3]: Pet-type
//
//         Tile : 31 ...............16...12...8 ...4 ...0
//     Body Seg : HHHH HHHH LLLL LLLL MMMM MMMM TTTT TTTT
//     Rotation : VVVV HHHH VVVV HHHH VVVV HHHH VVVV HHHH
//     Pet Type : FSCD FSCD FSCD FSCD FSCD FSCD FSCD FSCD
//
//     (Head, L-shape, Midriff, Tail)
//
#define GP_EMPTY_MASK   0xC0U // .6+ // Is this wrong?
#define GP_SPECIAL_MASK 0x40U // .6+
#define GP_SEG_MASK     0x30U // .5-4
#define GP_ROT_MASK     0x0CU // .3-2
#define GP_PET_MASK     0x03U // .1-0

#define GP_PET_BITS_MASK  (GP_SEG_MASK | GP_ROT_MASK | GP_PET_MASK)


// Extra tiles above pet tiles
// Just one that gets loaded: the board empty tile piece
#define GP_EMPTY               64U // 0x20U

#define GP_SPECIAL_START       (SPRITE_TILE_SPECIAL_START)

// Special tiles (loaded from separate set)
#define GP_SPECIAL_BOMB        (0U + GP_SPECIAL_START)
#define GP_SPECIAL_LIGHTENING  (1U + GP_SPECIAL_START)
#define GP_SPECIAL_BOMB_HINT   (2U + GP_SPECIAL_START)
#define GP_SPECIAL_MERGE_HINT  (3U + GP_SPECIAL_START)
#define GP_SPECIAL_DROP_HINT   (4U + GP_SPECIAL_START)
#define GP_DISSOLVE_1          (5U + GP_SPECIAL_START)
#define GP_DISSOLVE_2          (6U + GP_SPECIAL_START)
#define GP_DISSOLVE_3          (7U + GP_SPECIAL_START)
#define GP_CROSS               (8U + GP_SPECIAL_START)


#define GP_PET_UPSHIFT  0U
#define GP_PET_DOG      0U
#define GP_PET_CAT      1U
#define GP_PET_FISH     2U
#define GP_PET_SNAKE    3U
#define GP_PET_NONE     4U
#define GP_PET_MASK_NOSHIFT 0x03U


#define GP_SEG_UPSHIFT  4U
#define GP_SEG_TAIL     0U
#define GP_SEG_TORSO    1U
#define GP_SEG_TURN     2U
#define GP_SEG_HEAD     3U
#define GP_SEG_TAIL_BITS  (GP_SEG_TAIL  << GP_SEG_UPSHIFT)
#define GP_SEG_TORSO_BITS (GP_SEG_TORSO << GP_SEG_UPSHIFT)
#define GP_SEG_TURN_BITS  (GP_SEG_TURN  << GP_SEG_UPSHIFT)
#define GP_SEG_HEAD_BITS  (GP_SEG_HEAD  << GP_SEG_UPSHIFT)


// 256
// 260
// 264
// 268

#define GP_ROT_UPSHIFT  2U
// #define GP_ROT_HORZ     0U
// #define GP_ROT_VERT     1U
// #define GP_ROT_HORZ_BITS  (0x00U << GP_ROT_UPSHIFT)
// #define GP_ROT_VERT_BITS  (0x01U << GP_ROT_UPSHIFT)
#define GP_ROTATE_0        0U
#define GP_ROTATE_90       1U
#define GP_ROTATE_180      2U
#define GP_ROTATE_270      3U

#define GP_ROTATE_DEFAULT  GP_ROTATE_0
#define GP_ROTATE_MIN      GP_ROTATE_0
#define GP_ROTATE_MAX      GP_ROTATE_270

#define GP_ROTATE_0_BITS   (GP_ROTATE_0   << GP_ROT_UPSHIFT)
#define GP_ROTATE_90_BITS  (GP_ROTATE_90  << GP_ROT_UPSHIFT)
#define GP_ROTATE_180_BITS (GP_ROTATE_180 << GP_ROT_UPSHIFT)
#define GP_ROTATE_270_BITS (GP_ROTATE_270 << GP_ROT_UPSHIFT)
#define GP_ROTATE_DEFAULT_BITS  GP_ROTATE_0_BITS
// #define GP_ROTATE_SEG_TURN_180_AND_270_USE_MIRROR_BITS 0x02U // **DO NOT UPSHIFT**

// Piece color palettes
#define GP_PAL_DOG      0x00U
#define GP_PAL_CAT      0x01U
#define GP_PAL_FISH     0x02U
#define GP_PAL_SNAKE    0x03U
#ifdef NINTENDO
    #define GP_PAL_EMPTY    0x00U // Use first pet palette
#elif defined (GAMEGEAR)
    // Make sure board tiles get forced to second bkg palette when reset
    #define GP_PAL_EMPTY    (GG_TILE_ATTR_MSB_PAL_1 | 0x00U) // Use first pet palette
#endif
#define GP_PAL_SPECIAL  0x04U
#define GP_PAL_CLEAR    GP_PAL_SPECIAL
#define GP_PAL_DROPHINT GP_PAL_SPECIAL
#define GP_PAL_CROSS    GP_PAL_SPECIAL
// NOTE: adding pieces here requires an update here: TILE_COUNT_PETTOTAL



// BG Map Attributes (CGB Mode only)
// Bit 0-2  Background Palette number  (BGP0-7)
// Bit 3    Tile VRAM Bank number      (0=Bank 0, 1=Bank 1)
// Bit 4    Not used
// Bit 5    Horizontal Flip            (0=Normal, 1=Mirror horizontally)
// Bit 6    Vertical Flip              (0=Normal, 1=Mirror vertically)
// Bit 7    BG-to-OAM Priority         (0=Use OAM priority bit, 1=BG Priority)

// #define GP_MIRROR_OFF  0x00U
// #define GP_MIRROR_X    S_FLIPX
// #define GP_MIRROR_Y    S_FLIPY

// extern const UINT8 GP_ROT_LUT_TILE[];
// extern const UINT8 GP_ROT_LUT_ATTR[];
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
