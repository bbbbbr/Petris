// gfx_print.h


#ifndef GFX_PRINT_H
#define GFX_PRINT_H

#define SPR_8x8_HEIGHT  8u
#define SPR_8x8_WIDTH   8u

#define SPR_PRINT_SPACING_X 7  // (SPR_8x8_WIDTH  + 1u)  // 1 pixels between character sprites
#define SPR_PRINT_SPACING_Y (SPR_8x8_HEIGHT + 8u)  // 8 pixels between character sprites

#define STR_DIGIT_LEN_5 5
#define STR_DIGIT_LEN_3 3
#define STR_DIGIT_LEN_2 2

#define PRINT_MAX_DIGITS  5
#define PRINT_MAX_NUM     99999 // ((10 ^ PRINT_MAX_DIGITS) - 1)

#define PRINT_ATTRIB_PAL_DEFAULT 0x0000u

#define PRINT_PAL_TILE_YELLOW (0u << 12)
#define PRINT_PAL_TILE_PINK   (1u << 12)
#define PRINT_PAL_TILE_GREY   (2u << 12)

#define PRINT_PAL_OAM_YELLOW (S_PAL0)
#define PRINT_PAL_OAM_PINK   (S_PAL1)
#define PRINT_PAL_OAM_GREY   (S_PAL2)

extern uint16_t print_x, print_y, print_tile_attribs;


#define PRINT_PAL(SELECTED_PAL) print_tile_attribs = ((print_tile_attribs & ~BG_TILEMAP_SUBPAL_MASK) | SELECTED_PAL)
#define PRINT_POS(X, Y) print_x = X; print_y  = Y
#define PRINTXY(X, Y, TXT, DELAY) PRINT_POS(X,Y); print_to_tilemap(TXT, DELAY)
#define PRINT(TXT, DELAY) print_to_tilemap(TXT, DELAY)
#define SET_PRINT_TARGET(TARGET) set_bkg_tilemap_base_address(TARGET)


void print_num_u16(uint16_t x, uint16_t y, uint16_t num, uint16_t fixed_str_length);

void load_8x16_font_tilemap_palettes(void);
void load_8x16_font_sprite_palettes(void);
uint16_t load_8x16_font_tiles(uint16_t tile_id_start);


uint16_t print_to_sprites(uint16_t oam_id, uint8_t pal, const char * txt);
void print_to_tilemap(const char * txt, uint16_t delay_time);


#endif // GFX_PRINT_H