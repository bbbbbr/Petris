// Copyright 2020 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include <gbdk/platform.h>

#include "platform_compat.h"

#if defined(SEGA)
    // Map GG pelette colors to CGB sub-palettes
    const uint16_t gg_pal_select[] = {
        COMPAT_PALETTE(0,1,2,3),
        COMPAT_PALETTE(4,5,6,7),
        COMPAT_PALETTE(8,9,10,11),
        COMPAT_PALETTE(12,13,14,15),
    };
    // Map GG pelette colors to CGB sub-palettes (first color always transparent 0)
    const uint16_t gg_spr_pal_select[] = {
        COMPAT_PALETTE(0,1,2,3),
        COMPAT_PALETTE(0,5,6,7),
        COMPAT_PALETTE(0,9,10,11),
        COMPAT_PALETTE(0,13,14,15),
    };
#endif

// ==== Tile Data Per Pixel ====
//      MSbit LSBit
//      3..0
// GG:  CCCC
// CGB:   CC
//
// CGB: C = Color select (0 - 3) - Palette via BG map/Sprite Attribs only
// GG:  C = Color Select (0 - 15)
//      Map Palettes 0-3 -> to MSBits of tile data
//
// ===== Tile Map =====
//       MSB          LSB
//       15..............0
// GG:   ---pcvhn nnnnnnnn
// CGB:  pvh-bccc nnnnnnnn



#define CGB_MIRROR_BITS 0x60u       // This is (S_FLIPX | S_FLIPY), but the CGB bits here in GG mode
#define CGB_2_GG_MIRROR_DWN_SHIFT 4
#define CGB_PAL_MSBIT   0x04u
#define CGB_2_GG_PAL_MSBIT_UP_SHIFT 1
#define CGB_PAL_LSBITS   0x03u


// TODO:GAMEGEAR: merge this function with the one below
//
// Merges in CGB palette number attribute while loading sprite data on GG
void set_sprite_data_pal_compat(uint8_t tiles_start, uint8_t tile_count,  const uint8_t * p_tile_data, const uint8_t * p_tile_pal_list) {

    uint8_t tileid;

    #ifdef NINTENDO
        set_sprite_data(x, y, width, height, p_attr_map);
    #elif defined (GAMEGEAR)
        for (tileid = tiles_start;tileid < (tiles_start + tile_count); tileid++) {

            set_2bpp_palette(gg_spr_pal_select[*p_tile_pal_list & 0x03u]); // Set palette based on index, repeating 0..3
            set_sprite_data(tileid, 1, p_tile_data);

            p_tile_data += 16; // next tile
            p_tile_pal_list++; // next tile pal
        }
    #endif
}


// Merges in CGB palette number attribute while loading tile data on GG
void set_bkg_data_pal_compat(uint8_t tiles_start, uint8_t tile_count,  const uint8_t * p_tile_data, const uint8_t * p_tile_pal_list) {

    uint8_t tileid;

    #ifdef NINTENDO
        set_bkg_data(x, y, width, height, p_attr_map);
    #elif defined (GAMEGEAR)
        for (tileid = tiles_start;tileid < (tiles_start + tile_count); tileid++) {

            set_2bpp_palette(gg_pal_select[*p_tile_pal_list & 0x03u]); // Set palette based on index, repeating 0..3
            set_bkg_data(tileid, 1, p_tile_data);

            p_tile_data += 16; // next tile
            p_tile_pal_list++; // next tile pal
        }
    #endif
}


// Rewrites CGB map attributes to GG compatibile style if needed
// * Mirror bits and Palette MSBit get bit shifted and then written to tile map
void set_bkg_tiles_attr_compat(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t * p_attr_map) {//, uint8_t * p_tileid_map) {

    uint8_t mx,my;
    uint8_t map_attrib;

    VBK_REG = 1; // Select BG tile attribute map

    #ifdef NINTENDO
        set_bkg_tiles(x, y, width, height, p_attr_map);
    #elif defined (GAMEGEAR)
        // Try to unpack CGB palette and flipping attributes from map data
        for (my = y; my < (y + height); my++) {
            for (mx = x; mx < (x + width); mx++) {

                // Rewrite CGB map attribs
                // map_attrib = (((*p_attr_map) & CGB_MIRROR_BITS) >> CGB_2_GG_MIRROR_DWN_SHIFT);  // V and H flip bits
                map_attrib = ((((*p_attr_map) & CGB_MIRROR_BITS) >> CGB_2_GG_MIRROR_DWN_SHIFT) |  // V and H flip bits
                              (((*p_attr_map) & CGB_PAL_MSBIT)   << CGB_2_GG_PAL_MSBIT_UP_SHIFT)); // Palette Select 0..15 or 16..31
                set_bkg_tile_xy(mx,my, map_attrib);
                p_attr_map++;
            }

        }
    #endif

    VBK_REG = 0; // Re-Select regular BG tile map
}