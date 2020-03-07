// gfx.h

#ifndef GFX_H
#define GFX_H

#define BG_PRIORITY 0x80
#define BG_FLIP_Y   0x40
#define BG_FLIP_X   0x20

#define TILES_BG_START       0
#define TILES_PET_START     64
#define TILE_COUNT_BG       16
#define TILE_COUNT_PETS     32
#define TILE_COUNT_PETBLANK  2
#define TILE_COUNT_PETTOTAL  TILE_COUNT_PETS + TILE_COUNT_PETBLANK


extern const UWORD bgPalette[];

void gfx_init(void);
void gfx_init_sprites(void);
void gfx_init_background(void);

void video_setflip(UINT8);

#endif // GFX_H


