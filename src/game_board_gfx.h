// game_board_gfx.h

#ifndef GAME_BOARD_GFX_H
#define GAME_BOARD_GFX_H

// Game Board/Play: Palette Assignment / Management
// These are System-Absolute 16 color palettes in the range of 0-16
#define ABS_PAL_FONT_8x16_YELLOW_OBJ_0      (PAL_ASSIGN_OBJ_0)
#define ABS_PAL_FONT_8x16_PINK_OBJ_1        (PAL_ASSIGN_OBJ_1)
#define ABS_PAL_FONT_8x16_GREY_OBJ_2        (PAL_ASSIGN_OBJ_2)
#define ABS_PAL_PETS_SPECIAL_8x8FONT_OBJ_3  (PAL_ASSIGN_OBJ_3)

#define ABS_PAL_FONT_8x16_YELLOW_BG0_0      (PAL_ASSIGN_BG0_0)
#define ABS_PAL_FONT_8x16_GREY_BG0_1        (PAL_ASSIGN_BG0_1)
#define ABS_PAL_BORDER_BG0_2                (PAL_ASSIGN_BG0_2)
#define ABS_PAL_PETS_SPECIAL_8x8FONT_BG0_3  (PAL_ASSIGN_BG0_3)

#define ABS_PAL_BACKGROUND_BG1_0            (PAL_ASSIGN_BG1_0)
#define ABS_PAL_BOARD_UNDER_BG1_1           (PAL_ASSIGN_BG1_1)

// Layer-Relative palettes
#define REL_PAL_PETS_SPECIAL_8x8FONT_OAM_3  (PAL_3)

#define REL_PAL_BORDER_BG0_2                (PAL_2)
#define REL_PAL_PETS_SPECIAL_8x8FONT_BG0_3  (PAL_3)

#define REL_PAL_BOARD_UNDER_BG1_1           (PAL_1)


#define PIECE_TO_RELATIVE_BGTILE(piece, attrib) ((uint16_t)piece | (((uint16_t)attrib) << 8)) // | BG_PAL(REL_PAL_PETS_SPECIAL_8x8FONT_OAM_3))
#define PIECE_TO_ABSOLUTE_BGTILE(piece, attrib) (((uint16_t)piece + TILES_PET_START_VRAM_ABSOLUTE) | ((uint16_t)attrib << 8)) // | BG_PAL(REL_PAL_PETS_SPECIAL_8x8FONT_OAM_3))

#define CALC_PET_FONT_8x8_NUM_TILE(digit, pet_type) (FONT_8x8_NUMS_PET_COLORED_START + (pet_type * TILE_COUNT_FONT_8x8_PET_DIGIT_COUNT) + digit)

void board_gfx_init(void);
void board_gfx_change_pettiles(void);
void board_gfx_init_pettiles(void);
void board_gfx_init_sprites(void);
void board_gfx_init_background(void);

void board_gfx_tail_animate(void);

#endif // GAME_BOARD_GFX_H


