// gfx_print.h


#ifndef GFX_PRINT_H
#define GFX_PRINT_H

void print_num_u16(UINT8, UINT8, UINT16, UINT8);

#define DIGITS_5 5
#define DIGITS_3 3
#define DIGITS_2 2

#define PRINT_MAX_DIGITS  5
#define PRINT_MAX_NUM     99999 // ((10 ^ PRINT_MAX_DIGITS) - 1)
#define PRINT_ATTRIB_PAL_MASK    0x03
#define PRINT_ATTRIB_PAL_DEFAULT 0x04

extern UINT8 print_x, print_y, font_idx, print_target, print_tile_attribs;

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


#endif // GFX_PRINT_H