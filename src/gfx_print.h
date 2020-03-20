// gfx_print.h


#ifndef GFX_PRINT_H
#define GFX_PRINT_H

void print_num_u16(UINT8, UINT8, UINT16);


extern UINT8 print_x, print_y, font_idx, print_target;

typedef enum {
    PRINT_BKG,
    PRINT_WIN
} PRINT_TARGET;

void print_text(const char* txt, unsigned char delay_time);
#define PRINT_POS(X, Y) print_x = X; print_y  = Y
#define PRINT(X, Y, TXT, DELAY) PRINT_POS(X,Y); print_text(TXT, DELAY)
#define SET_PRINT_TARGET(TARGET) print_target = TARGET
#define INIT_FONT(FONT_TILE_START, TARGET) font_idx = FONT_TILE_START; print_target = TARGET


#endif // GFX_PRINT_H