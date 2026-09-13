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
#define BG_PAL_BOARD_NEXT_PIECE_PREVIEW BG_PAL_0

// In tiles
#define SCREEN_WIDTH  (DEVICE_SCREEN_WIDTH)
#define SCREEN_HEIGHT (DEVICE_SCREEN_HEIGHT)
#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define SCREEN_MAX_X (SCREEN_WIDTH - 1)
#define SCREEN_MAX_Y (SCREEN_HEIGHT - 1)


#define TILE_SIZE_BYTES          32U // (8x8 pixels, 4 bits per pixel mode)

#define TILE_ID_BOARD_BLANK_BG    (3U)
#define TILE_ID_BOARD_BLANK_BG_BW (0U)
#define TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG  (0U)
#define TILE_ID_BOARD_UP        (63U)


#define TILE_OFFSET_PET_TAIL_REG 0U
#define TILE_OFFSET_PET_TAIL_WAG 33U // Offset to tail wag alternate tiles

#define TILES_PET_START        96U // This requires ^2 alignment
#define TILE_COUNT_PETS        32U
#define TILE_COUNT_PETBLANK     1U
#define TILE_COUNT_PET_ANIM__NOTLOADED_ONSTART__  8U // These don't get loaded at startup, just used to replace existing tiles

#define TILE_COUNT_PETTOTAL      (TILE_COUNT_PETS + TILE_COUNT_PETBLANK)

#define TILES_SPECIAL_START     (TILES_PET_START + TILE_COUNT_PETTOTAL) // Load right after pet tiles
#define TILE_COUNT_PETSPECIAL     4U  // Special pieces and their hinting tiles
#define TILE_COUNT_DROP_HINT      1U  // Crosshair Sights
#define TILE_COUNT_DISSOLVE       3U  // 3 Dissolve tiles
#define TILE_COUNT_LONG_PET_HINT  1U  // Cross shape

#define TILE_COUNT_SPECIALTOTAL (TILE_COUNT_PETSPECIAL + TILE_COUNT_DROP_HINT + TILE_COUNT_DISSOLVE + TILE_COUNT_LONG_PET_HINT)

    // Font gets loaded at the end of tile RAM
    // #define TILE_COUNT_FONT     45U //Tiles in order: <blank>ABC...XYZ0123456789!'()-.:?
    // #define TILES_FONT_START    (255U - TILE_COUNT_FONT) // (TILES_BG_START + TILE_COUNT_BG)

    #define TILE_ID_FONT_BLANK      0u // blank tile
    #define TILES_FONT_BLANK_LEN    1u
    #define TILES_FONT_CHARS_START  (TILES_FONT_BLANK_LEN)
    // Numeric tiles are a subset of main font
    // So these are just for convenience
    #define TILES_FONT_CHARS_LEN   27u
    #define TILES_FONT_NUMS_START  (TILES_FONT_CHARS_LEN)
    #define TILE_COUNT_FONT_NUMS   11U //Tiles in order: 0123456789<blank>

extern uint16_t * p_pet_tiles;
extern uint16_t * p_pet_palette;

void pet_tiles_prepare(void);

#endif // GFX_H


