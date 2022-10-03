// gfx.h

#ifndef GFX_H
#define GFX_H

// #define DEBUG_SHOW

#define BG_PAL_0    0x00U
#define BG_PAL_1    0x01U
#define BG_PAL_2    0x02U
#define BG_PAL_3    0x03U
#define BG_PAL_4    0x04U
#define BG_PAL_5    0x05U
#define BG_PAL_6    0x06U
#define BG_PAL_7    0x07U

 #ifdef NINTENDO
    #define BG_PAL_BOARD_NEXT_PIECE_PREVIEW BG_PAL_0
#elif defined (GAMEGEAR)
    // Make sure board tiles get forced to second bkg palette when reset
    #define BG_PAL_BOARD_NEXT_PIECE_PREVIEW  (GG_TILE_ATTR_MSB_PAL_1 | 0x00U) // Use first pet palette
 #endif

#define BG_PRIORITY 0x80U
#define BG_FLIP_Y   0x40U
#define BG_FLIP_X   0x20U

#define SCREEN_WIDTH  20
#define SCREEN_HEIGHT 18

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define SCREEN_MAX_X 19
#define SCREEN_MAX_Y 17


//#define TILES_BG_START          0
//#define TILE_COUNT_BG          16
#define TILE_SIZE_BYTES          16U // (8x8 pixels, 2 bits per pixel)

#define TILES_INTRO_START      0U
#define TILE_COUNT_INTRO       75U
#define TILE_ID_BOARD_BLANK_BG    (TILES_INTRO_START + 3U)
#define TILE_ID_BOARD_BLANK_BG_BW (TILES_INTRO_START + 0U)
// #define TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG  (TILES_INTRO_START + 0U)
#define TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG  (GP_EMPTY + TILES_PET_START) // change to use same as board so hi-contrast updates automatically

#define TILE_ID_BOARD_UP        (TILES_INTRO_START + 63U)

// Clouds are stored at end of intro tiles
#define TILE_COUNT_CLOUDS      9U
#define TILE_ID_CLOUDS_START   (TILE_ID_BOARD_UP + 1U)
#define TILE_OFFSET_CLOUDS     (TILE_ID_CLOUDS_START * TILE_SIZE_BYTES)

#define TILE_ID_2P_LINK_OFF     TILES_INTRO_START
#define TILE_COUNT_2P_LINK      2U
#define TILE_ID_2P_LINK_START   (TILE_ID_CLOUDS_START + TILE_COUNT_CLOUDS)
#define TILE_OFFSET_2P_LINK     (TILE_ID_2PLINK_START * TILE_SIZE_BYTES)


#define TILE_OFFSET_PET_TAIL_REG 0U
#define TILE_OFFSET_PET_TAIL_WAG 65U // Offset to tail wag alternate tiles


#define TILES_PET_START        96U // This requires ^2 alignment
#define TILE_COUNT_PETS        64U
#define TILE_COUNT_PETBLANK     1U
#define TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__  16U // These don't get loaded at startup, just used to replace existing tiles

#define TILE_COUNT_PETTOTAL      (TILE_COUNT_PETS + TILE_COUNT_PETBLANK)


#define TILES_SPECIAL_START     (TILES_PET_START + TILE_COUNT_PETTOTAL) // Load right after pet tiles
#define TILE_COUNT_PETSPECIAL     4U  // Special pieces and their hinting tiles
#define TILE_COUNT_DROP_HINT      1U  // Crosshair Sights
#define TILE_COUNT_DISSOLVE       3U  // 3 Dissolve tiles
#define TILE_COUNT_LONG_PET_HINT  1U  // Cross shape

#define TILE_COUNT_SPECIALTOTAL (TILE_COUNT_PETSPECIAL + TILE_COUNT_DROP_HINT + TILE_COUNT_DISSOLVE + TILE_COUNT_LONG_PET_HINT)

// Font gets loaded at the end of tile RAM
#define TILE_COUNT_FONT     45U //Tiles in order: <blank>ABC...XYZ0123456789!'()-.:?
#define TILES_FONT_START    (255U - TILE_COUNT_FONT) // (TILES_BG_START + TILE_COUNT_BG)
    #define TILE_ID_FONT_BLANK  TILES_FONT_START // blank tile
    #define TILES_FONT_BLANK_LEN    1U
    #define TILES_FONT_CHARS_START  (TILES_FONT_START  + TILES_FONT_BLANK_LEN)
    // Numeric tiles are a subset of main font
    // So these are just for convenience
    #define TILES_FONT_CHARS_LEN   27U // Includes empty space
    #define TILES_FONT_NUMS_START  (TILES_FONT_START  + TILES_FONT_CHARS_LEN)
    #define TILE_COUNT_FONT_NUMS   11U //Tiles in order: 0123456789<blank>

extern const UINT8 * p_pet_tiles;
extern UWORD * p_pet_palette;
extern UINT8 pet_tiles_hicontrast_ram[];


extern const UWORD board_pets_palette[];
extern const UWORD board_pets_pal_high_contrast[];
extern const UWORD board_pets_pal_high_contrast_2[];
extern const UWORD board_pets_pal_med_contrast[];

extern const UWORD clouds_sprite_palette[];
extern const UWORD option_title_palette[];
extern const UWORD intro_screen_logo_palette[];
extern const UWORD intro_screen_palette[];

extern const const UWORD intro_cat_palette[];

// void print_font_palette_set(UINT8);
void pet_tiles_prepare();

#endif // GFX_H


