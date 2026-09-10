// gfx_print.h


#ifndef GFX_PRINT_H
#define GFX_PRINT_H

#define SPR_8x8_HEIGHT  8u
#define SPR_8x8_WIDTH   8u

#define SPR_PRINT_SPACING_X 7  // (SPR_8x8_WIDTH  + 1u)  // 1 pixels between character sprites
#define SPR_PRINT_SPACING_Y (SPR_8x8_HEIGHT + 8u)  // 8 pixels between character sprites


/*
void print_num_u16(uint8_t, uint8_t, uint16_t, uint8_t);

#define DIGITS_5 5
#define DIGITS_3 3
#define DIGITS_2 2

#define PRINT_MAX_DIGITS  5
#define PRINT_MAX_NUM     99999 // ((10 ^ PRINT_MAX_DIGITS) - 1)
#define PRINT_ATTRIB_PAL_MASK    0x03
#define PRINT_ATTRIB_PAL_DEFAULT 0x04

extern uint8_t print_x, print_y, font_idx, print_target, print_tile_attribs;

typedef enum {
    PRINT_BKG,
    PRINT_WIN
} PRINT_TARGET;

void print_text(const char* txt, unsigned char delay_time);

#define PRINT_PAL(SELECTED_PAL) print_tile_attribs = SELECTED_PAL
#define PRINT_POS(X, Y) print_x = X; print_y  = Y
#define PRINT(X, Y, TXT, DELAY) PRINT_POS(X,Y); print_text(TXT, DELAY)
#define SET_PRINT_TARGET(TARGET) print_target = TARGET
#define INIT_FONT(FONT_TILE_START, TARGET) font_idx = FONT_TILE_START; print_target = TARGET
*/

uint16_t load_8x16_font_tiles(uint16_t tile_id_start);
uint16_t print_to_sprites(uint16_t oam_id, uint16_t print_x, uint16_t print_y, uint8_t pal, const char * txt);


#endif // GFX_PRINT_H