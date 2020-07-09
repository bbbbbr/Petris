// gfx.h

#ifndef GFX_H
#define GFX_H

// #define DEBUG_SHOW

#define BG_PAL_0    0x00
#define BG_PAL_1    0x01
#define BG_PAL_2    0x02
#define BG_PAL_3    0x03
#define BG_PAL_4    0x04
#define BG_PAL_5    0x05
#define BG_PAL_6    0x06
#define BG_PAL_7    0x07
#define BG_PAL_BOARD_NEXT_PIECE_PREVIEW BG_PAL_0

#define BG_PRIORITY 0x80
#define BG_FLIP_Y   0x40
#define BG_FLIP_X   0x20

#define SCREEN_WIDTH  20
#define SCREEN_HEIGHT 18

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define SCREEN_MAX_X 19
#define SCREEN_MAX_Y 17


//#define TILES_BG_START          0
//#define TILE_COUNT_BG          16
#define TILE_SIZE_BYTES          16 // (8x8 pixels, 2 bits per pixel)

#define TILES_INTRO_START      0
#define TILE_COUNT_INTRO       73 // v3  // v2: 53 // v1: 69
#define TILE_ID_BOARD_BLANK_BG    (TILES_INTRO_START + 3)
#define TILE_ID_BOARD_BLANK_BG_BW (TILES_INTRO_START + 0)
#define TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG  (TILES_INTRO_START + 0)
#define TILE_ID_BOARD_UP        (TILES_INTRO_START + 63)

// Clouds are stored at end of intro tiles
#define TILE_COUNT_CLOUDS      9
#define TILE_ID_CLOUDS_START   (TILE_COUNT_INTRO - TILE_COUNT_CLOUDS)
#define TILE_OFFSET_CLOUDS     (TILE_ID_CLOUDS_START * TILE_SIZE_BYTES)

#define TILE_OFFSET_PET_TAIL_REG 0
#define TILE_OFFSET_PET_TAIL_WAG 33 // Offset to tail wag alternate tiles


#define TILES_PET_START        96 // This requires ^2 alignment
#define TILE_COUNT_PETS        32
#define TILE_COUNT_PETBLANK     1
#define TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__  8 // These don't get loaded at startup, just used to replace existing tiles

#define TILE_COUNT_PETTOTAL      (TILE_COUNT_PETS + TILE_COUNT_PETBLANK)


#define TILES_SPECIAL_START     (TILES_PET_START + TILE_COUNT_PETTOTAL) // Load right after pet tiles
#define TILE_COUNT_PETSPECIAL     4  // Special pieces and their hinting tiles
#define TILE_COUNT_DROP_HINT      1  // Crosshair Sights
#define TILE_COUNT_DISSOLVE       3  // 3 Dissolve tiles
#define TILE_COUNT_LONG_PET_HINT  1  // Cross shape

#define TILE_COUNT_SPECIALTOTAL (TILE_COUNT_PETSPECIAL + TILE_COUNT_DROP_HINT + TILE_COUNT_DISSOLVE + TILE_COUNT_LONG_PET_HINT)

// Font gets loaded at the end of tile RAM
#define TILE_COUNT_FONT     45 //Tiles in order: <blank>ABC...XYZ0123456789!'()-.:?
#define TILES_FONT_START    (255 - TILE_COUNT_FONT) // (TILES_BG_START + TILE_COUNT_BG)
    #define TILE_ID_FONT_BLANK  TILES_FONT_START // blank tile
    #define TILES_FONT_BLANK_LEN    1
    #define TILES_FONT_CHARS_START  (TILES_FONT_START  + TILES_FONT_BLANK_LEN)
    // Numeric tiles are a subset of main font
    // So these are just for convenience
    #define TILES_FONT_CHARS_LEN   27
    #define TILES_FONT_NUMS_START  (TILES_FONT_START  + TILES_FONT_CHARS_LEN)
    #define TILE_COUNT_FONT_NUMS   11 //Tiles in order: 0123456789<blank>

extern UINT8 * p_pet_tiles;
extern UWORD * p_pet_palette;
extern UINT8 pet_tiles_hicontrast_ram[];


extern const UWORD board_pets_palette[];
extern const UWORD board_pets_palette_high_contrast[];
extern const UWORD board_pets_palette_med_contrast[];

extern const UWORD clouds_sprite_palette[];
extern const UWORD option_title_palette[];
extern const UWORD intro_screen_logo_palette[];
extern const UWORD intro_screen_palette[];


// void print_font_palette_set(UINT8);
void pet_tiles_hicontrast_prepare();

#endif // GFX_H


