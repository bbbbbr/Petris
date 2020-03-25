// gfx.h

#ifndef GFX_H
#define GFX_H

#define BG_PAL_0    0x00
#define BG_PAL_1    0x01
#define BG_PAL_2    0x02
#define BG_PAL_3    0x03
#define BG_PAL_4    0x04
#define BG_PAL_5    0x05
#define BG_PAL_6    0x06
#define BG_PAL_7    0x07

#define BG_PRIORITY 0x80
#define BG_FLIP_Y   0x40
#define BG_FLIP_X   0x20

#define SCREEN_WIDTH  20
#define SCREEN_HEIGHT 18

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define SCREEN_MAX_X 19
#define SCREEN_MAX_Y 17


#define TILES_BG_START          0
#define TILE_COUNT_BG          16

#define TILES_INTRO_START      0
#define TILE_COUNT_INTRO        53


#define TILES_PET_START        64 // This requires ^2 alignment
#define TILE_COUNT_PETS        32
#define TILE_COUNT_PETBLANK     2
#define TILE_COUNT_PETTOTAL    (TILE_COUNT_PETS + TILE_COUNT_PETBLANK)

// Font gets loaded at the end of tile RAM
#define TILE_COUNT_FONT     45 //Tiles in order: <blank>ABC...XYZ0123456789!'()-.:?
#define TILES_FONT_START    (255 - TILE_COUNT_FONT) // (TILES_BG_START + TILE_COUNT_BG)
    #define TILE_ID_FONT_BLANK  TILES_FONT_START // blank tile
    #define TILES_FONT_CHARS_START  (TILES_FONT_START  + 1)
    // Numeric tiles are a subset of main font
    // So these are just for convenience
    #define TILES_FONT_NUMS_START  (TILES_FONT_START  + 27)
    #define TILE_COUNT_FONT_NUMS   11 //Tiles in order: 0123456789<blank>

extern const UWORD bgPalette[];

#endif // GFX_H


