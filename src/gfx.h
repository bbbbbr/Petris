// gfx.h

#ifndef GFX_H
#define GFX_H

// #define DEBUG_SHOW

// Casio Loopy Hardware x Layer palette assignment
#define PAL_ASSIGN_BG0_0  (PAL_0)
#define PAL_ASSIGN_BG0_1  (PAL_1)
#define PAL_ASSIGN_BG0_2  (PAL_2)
#define PAL_ASSIGN_BG0_3  (PAL_3)

#define PAL_ASSIGN_BG1_0  (PAL_4)
#define PAL_ASSIGN_BG1_1  (PAL_5)
#define PAL_ASSIGN_BG1_2  (PAL_6)
#define PAL_ASSIGN_BG1_3  (PAL_7)

#define PAL_ASSIGN_OBJ_0  (PAL_8)
#define PAL_ASSIGN_OBJ_1  (PAL_9)
#define PAL_ASSIGN_OBJ_2  (PAL_10)
#define PAL_ASSIGN_OBJ_3  (PAL_11)

#define PAL_ASSIGN_BM_0  (PAL_12)
#define PAL_ASSIGN_BM_1  (PAL_12)
#define PAL_ASSIGN_BM_2  (PAL_12)
#define PAL_ASSIGN_BM_3  (PAL_12)


#define BG_PAL_0    0x00U
#define BG_PAL_1    0x01U
#define BG_PAL_2    0x02U
#define BG_PAL_3    0x03U
#define BG_PAL_4    0x04U
#define BG_PAL_5    0x05U
#define BG_PAL_6    0x06U
#define BG_PAL_7    0x07U
#define BG_PAL_BOARD_NEXT_PIECE_PREVIEW BG_PAL_0

#define TILE_HEIGHT_8x8  8u
#define TILE_WIDTH_8x8   8u

// In tiles
#define SCREEN_WIDTH  (DEVICE_SCREEN_WIDTH)
#define SCREEN_HEIGHT (DEVICE_SCREEN_HEIGHT)
#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define SCREEN_MAX_X (SCREEN_WIDTH - 1)
#define SCREEN_MAX_Y (SCREEN_HEIGHT - 1)

#define PIECE_TO_RELATIVE_BGTILE(piece, attrib) ((uint16_t)piece | (((uint16_t)attrib) << 8))
#define PIECE_TO_ABSOLUTE_BGTILE(piece, attrib) (((uint16_t)piece | (((uint16_t)attrib) << 8)) + TILES_PET_START_VRAM_ABSOLUTE)

#define CALC_PET_FONT_8x8_NUM_TILE(digit, pet_type) (FONT_8x8_NUMS_PET_COLORED_START + (pet_type * TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT) + digit)

#define TILE_4BPP_SIZE_BYTES           32U // (8x8 pixels, 4 bits per pixel mode)
#define TILE_4BPP_SIZE_U16             16U // (8x8 pixels, 4 bits per pixel mode)

#define TILE_ID_BOARD_BLANK       (TILE_OFFSET_PETBLANK)
#define TILE_ID_BOARD_NEXT_PIECE_PREVIEW_BG  (0U)
#define TILE_ID_BOARD_UP          (TILE_OFFSET_DISSOLVE_2)



#define TILES_PET_START_VRAM_ABSOLUTE (OBJ_TILEGROUP_BASE_512) // This requires ^2 alignment
    #define TILE_PET_TAIL_REG_START    0U  // This requires ^2 alignment
    #define TILE_COUNT_PETS           32U
    #define TILE_COUNT_PETBLANK        1U
    #define TILE_COUNT_PET_TOTAL      (TILE_COUNT_PETS + TILE_COUNT_PETBLANK)
        #define TILE_OFFSET_PETBLANK  (TILE_PET_TAIL_REG_START + TILE_COUNT_PETS)

#define TILES_SPECIAL_START           (TILE_PET_TAIL_REG_START + TILE_COUNT_PET_TOTAL) // Load right after pet tiles
    #define TILE_COUNT_PETSPECIAL     4U  // Special pieces and their hinting tiles
    #define TILE_COUNT_DROP_HINT      1U  // Crosshair Sights
    #define TILE_COUNT_DISSOLVE       3U  // 3 Dissolve tiles
    #define TILE_COUNT_LONG_PET_HINT  1U  // Cross shape
    #define TILE_COUNT_SPECIAL_TOTAL   (TILE_COUNT_PETSPECIAL + TILE_COUNT_DROP_HINT + TILE_COUNT_DISSOLVE + TILE_COUNT_LONG_PET_HINT)
        #define TILE_OFFSET_DISSOLVE_2 (TILES_SPECIAL_START + TILE_COUNT_PETSPECIAL + TILE_COUNT_DROP_HINT + 1u)

// #define TILES_PET_AND_SPECIAL_LOAD_COUNT (TILE_COUNT_PET_TOTAL + TILE_COUNT_SPECIAL_TOTAL)  // Ignore this, enough tile vram to load some that aren't used

#define TILE_PET_TAIL_WAG_START       (TILES_SPECIAL_START + TILE_COUNT_SPECIAL_TOTAL) // Offset to tail wag alternate tiles
    #define TILE_COUNT_PET_TAIL_WAGS  8U       // These don't get loaded at startup, just used to replace existing tiles
    #define TILE_COUNT_PET_TAIL_WAGS_TOTAL  8U

#define FONT_8x8_NUMS_PET_COLORED_START          (TILE_PET_TAIL_WAG_START + TILE_COUNT_PET_TAIL_WAGS_TOTAL) // Offset to pet colored 8x8 font
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT  10U  // Ten digits
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_DOG    0U
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_CAT    (TILE_COUNT_FONT_8x8_PET_DIGIT_DOG   + TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT)
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_SNAKE  (TILE_COUNT_FONT_8x8_PET_DIGIT_CAT   + TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT)
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_FISH   (TILE_COUNT_FONT_8x8_PET_DIGIT_SNAKE + TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT)
    #define TILE_COUNT_FONT_8x8_PET_DIGIT_SETS   4U
    #define TILE_COUNT_PET_TAIL_WAGSTOTAL  8U

    // Font gets loaded at the end of tile RAM
    // #define TILE_COUNT_FONT     45U //Tiles in order: <blank>ABC...XYZ0123456789!'()-.:?
    // #define TILES_FONT_START    (255U - TILE_COUNT_FONT) // (TILES_BG_START + TILE_COUNT_BG)

    // #define TILE_ID_FONT_BLANK      0u // blank tile        // TODO
    // #define TILES_FONT_BLANK_LEN    1u
    // #define TILES_FONT_CHARS_START  (TILES_FONT_BLANK_LEN)
    // Numeric tiles are a subset of main font
    // So these are just for convenience
    // #define TILES_FONT_CHARS_LEN   27u
    // #define TILES_FONT_NUMS_START  (TILES_FONT_CHARS_LEN)
    // #define TILE_COUNT_FONT_NUMS   11U //Tiles in order: 0123456789<blank>

extern const uint16_t * p_pet_tiles;
extern const uint16_t * p_special_tiles;
extern const uint16_t * p_pet_wag_tiles;
extern const uint16_t * p_font_pet_colored_tiles;
// extern uint16_t * p_pet_palette;


void pet_tiles_prepare(void);

#endif // GFX_H


