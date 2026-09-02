#pragma once

#include "loopy.h"


#define  BYTES_PER_4BPP_TILE      32u
#define  BYTES_PER_8BPP_TILE      64u

#define  TILES_PER_VRAM_ROW       8u
#define  BYTES_PER_8BPP_TILE_ROW  (BYTES_PER_8BPP_TILE * TILES_PER_VRAM_ROW)


extern const uint32_t CHARBASE_OFFSETS[8];
extern const uint32_t BG1_OFFSETS[8];


#define BG_PAL_SETUP(sp0,sp1,sp2,sp3) ( ((sp3) << 12) | ((sp2) << 8) | ((sp1) << 4) | (sp0))
#define BG_CTRL_LAYOUT_GET() ((VDP.BG_CTRL) & 0x07u)


#define BG0_MAP_START()  ((uint16_t *)VDP.TILE_VRAM)  // BG0 Starts at base of tile vram
#define BG1_MAP_START()  (BG0_MAP_START() + BG1_OFFSETS[BG_CTRL_LAYOUT_GET()])


#define CHAR_VRAM_4BPP_START() ((uint8_t *)VDP.TILE_VRAM + CHARBASE_OFFSETS[BG_CTRL_LAYOUT_GET()] + (VDP.CHARBASE * BYTES_PER_8BPP_TILE_ROW))
