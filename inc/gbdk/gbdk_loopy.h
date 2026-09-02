#pragma #once

#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

// Casio Loopy Specific


// hardware.h

// This is for Layout 0 (2 x 64x64 tilemaps) in 8x8 mode with 256x224 screen
#define DEVICE_SCREEN_X_OFFSET       0u
#define DEVICE_SCREEN_Y_OFFSET       0u
#define DEVICE_SCREEN_WIDTH          32u
#define DEVICE_SCREEN_HEIGHT         28u
#define DEVICE_SCREEN_BUFFER_WIDTH   64u
#define DEVICE_SCREEN_BUFFER_HEIGHT  64u
#define DEVICE_SCREEN_MAP_ENTRY_SIZE 1u  // In this case 1 x uint16_t (vs gbdk uint8_t perspective)
#define DEVICE_SPRITE_PX_OFFSET_X    0u  // TODO, but I think 0 iirc
#define DEVICE_SPRITE_PX_OFFSET_Y    0u  // TODO, but I think 0 iirc
#define DEVICE_WINDOW_PX_OFFSET_X    0u
#define DEVICE_WINDOW_PX_OFFSET_Y    0u

#define DEVICE_SCREEN_PX_WIDTH (DEVICE_SCREEN_WIDTH * 8)
#define DEVICE_SCREEN_PX_HEIGHT (DEVICE_SCREEN_HEIGHT * 8)



// General GBDK

// Button mapping is based on usable location instead of literal 1:1 name matching.
//
// So Game Boy A/B map to Loopy D/C since they're in the same usable location
// and are more convenient to press than Loopy's A/B

#define J_UP     (GAMEPAD_BTN_UP)
#define J_DOWN   (GAMEPAD_BTN_DOWN)
#define J_LEFT   (GAMEPAD_BTN_LEFT)
#define J_RIGHT  (GAMEPAD_BTN_RIGHT)

#define J_SELECT (GAMEPAD_BTN_A)
#define J_START  (GAMEPAD_BTN_START)
#define J_A      (GAMEPAD_BTN_D)
#define J_B      (GAMEPAD_BTN_C)
// Extras
#define J_LTRIG  (GAMEPAD_BTN_LTRIG)
#define J_RTRIG  (GAMEPAD_BTN_RTRIG)
#define J_X      (GAMEPAD_BTN_B)
#define J_Y      (GAMEPAD_BTN_A)


// No need for BANKREFS, zero them out
#define BANKREF(x)
#define BANKREF_EXTERN(x)

typedef uint16_t palette_color_t;

#define COLS_PER_PAL_4BPP  16u
#define COLS_PER_PAL_8BPP 256u

#define RGB888(r, g, b) (((uint16_t)((((r) >> 3) & 0x1f) << 10)) | ((uint16_t)((((g) >> 3) & 0x1f) << 5)) | (((b) >> 3) & 0x1f))
#define RGB8 RGB888

// Note: Loopy bios vsync also polls controller(s?)
#define vsync  bios_vsync

/** Set background palette(s)

    @param first_palette  Index of the first palette to write (0-7)
    @param nb_palettes    Number of palettes to write (1-8, max depends on first_palette)
    @param rgb_data       Pointer to source palette data

    Writes __nb_palettes__ to background palette data starting
    at __first_palette__, Palette data is sourced from __rgb_data__.

    \li Each Palette is 32 bytes in size: 16 colors x 2 bytes per palette color entry.
    \li Each color (16 per palette) is packed as RGB-555 format (1:5:5:5, MSBit [15] is unused).
    \li Each component (R, G, B) may have values from 0 - 31 (5 bits), 31 is brightest.

 */
void set_bkg_4bpp_palette(unsigned int first_palette, unsigned int nb_palettes, const palette_color_t *rgb_data);



/** Sets VRAM Tile Pattern data in the 4bpp format

    @param first_tile  Index of the first tile to write (0 - 511)
    @param nb_tiles    Number of tiles to write
    @param data        Pointer to source Tile Pattern data.

    Defaults to BG0

    @see set_4bpp_tile_patterns_base_address    
 */
void set_bkg_4bpp_data(unsigned int start, unsigned int ntiles, const uint8_t *src);



/** Sets a rectangular region of Background Tile Map.

    @param x      X Start position in Background Map tile coordinates
    @param y      Y Start position in Background Map tile coordinates
    @param w      Width of area to set in tiles
    @param h      Height of area to set in tiles
    @param tiles  Pointer to source tile map data

    Entries are copied from map at __tiles__ to the Background Tile Map starting at
    __x__, __y__ writing across for __w__ tiles and down for __h__ tiles.

    Use @ref set_bkg_submap() instead when:
    \li Source map is wider than 32 tiles.
    \li Writing a width that does not match the source map width __and__ more
    than one row high at a time.

    One byte per source tile map entry.

    Writes that exceed coordinate 31 on the x or y axis will wrap around to
    the Left and Top edges.

    @see set_bkg_tilemap_base_address()
*/
void set_bkg_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles);

/** Sets a rectangular region of Background Tile Map.
    The offset value in __base_tile__ is added to
    the tile ID for each map entry.

    @param x      X Start position in Background Map tile coordinates
    @param y      Y Start position in Background Map tile coordinates
    @param w      Width of area to set in tiles
    @param h      Height of area to set in tiles
    @param tiles  Pointer to source tile map data
    @param base_tile Offset each tile ID entry of the source map by this value

    This is identical to @ref set_bkg_tiles() except that it
    adds the __base_tile__ parameter for when a tile map's tiles don't
    start at index zero. (For example, the tiles used by the map
    range from 100 -> 120 in VRAM instead of 0 -> 20).

    @see set_bkg_tiles for more details
    @see set_bkg_tilemap_base_address()    
*/
void set_bkg_based_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles, unsigned int base_tile);


/** Set base memory address used by the tilemap writing functions,
    used for selecting between BG0 and BG1
 
  @param p_tilemap_base_address   Should point to the starting (0,0)  Start position in Background Map tile coordinatesto the  a rectangular region of Background Tile Map.

    The Base address of BG0 is fixed to the start of tilemap memory,
    but the base address of BG1 depends on the tilemap settings selected
    in the VDP.BG_CTRL register.
    
   @see BG0_MAP_START(), BG1_MAP_START()
 */
void set_bkg_tilemap_base_address(uint16_t * p_tilemap_base_address);


/** Set base memory address used for 4bpp tile pattern writing functions
 
  @param p_tile_patterns_base_address   Should point to the starting (0,0)  Start position in Background Map tile coordinatesto the  a rectangular region of Background Tile Map.

    The Base address of BG0 is fixed to the start of tilemap memory,
    but the base address of BG1 depends on the tilemap settings selected
    in the VDP.BG_CTRL register.
    
   @see CHAR_VRAM_4BPP_START()
 */
void set_4bpp_tile_patterns_base_address(uint8_t * p_tile_patterns_base_address);


/** Set property flag to determine whether next loaded tiles render on Screen A or B
 
  @param screen_a_or_b   Select target output Screen `LAYER_SCREEN_A` or `LAYER_SCREEN_B`
 */
void set_bkg_tiles_target_screen_a_or_b(unsigned int screen_a_or_b);


/** Delays the given number of milliseconds.
    Uses no timers or interrupts, and can be called with
    interrupts disabled
 */
void delay(uint16_t d);
