#ifndef _GBDK_LOOPY_H
#define _GBDK_LOOPY_H

#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#undef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((always_inline))

#define INTERRUPT __attribute__((interrupt_handler))
#define SMALLFUNC __attribute__((section(".smallfunc")))

// Casio Loopy Specific


// hardware.h

/** Amount of hardware sprites in OAM
*/
#define MAX_HARDWARE_SPRITES 128

#define VSYNC_OAM_COPY_SZ      16u  // Copy N OAM entries in a row in vsync() current shim manual OAM copy
#define SHADOW_OAM_MAX_SPRITES 128u  // For current shim oam copy, MUST BE MULTIPLE OF VSYNC_OAM_COPY_SZ. Controls size of Shadow OAM and how much of it that gets copied to hardware OAM

// This is for Layout 0 (2 x 64x64 tilemaps) in 8x8 mode with 256x224 screen
#define DEVICE_SCREEN_X_OFFSET       0u
#define DEVICE_SCREEN_Y_OFFSET       0u
#define DEVICE_SCREEN_WIDTH          32u
#define DEVICE_SCREEN_HEIGHT         28u
#define DEVICE_SCREEN_BUFFER_WIDTH   64u // Assumes one of the BG_CTRL 64 x 64 layouts (split preferred)
#define DEVICE_SCREEN_BUFFER_HEIGHT  64u
#define DEVICE_SCREEN_MAP_ENTRY_SIZE 1u  // In this case 1 x uint16_t (vs gbdk uint8_t perspective)
#define DEVICE_SPRITE_PX_OFFSET_X    0u  // TODO, but I think 0 iirc
#define DEVICE_SPRITE_PX_OFFSET_Y    0u  // TODO, but I think 0 iirc
#define DEVICE_WINDOW_PX_OFFSET_X    0u
#define DEVICE_WINDOW_PX_OFFSET_Y    0u

#define DEVICE_SCREEN_PX_WIDTH (DEVICE_SCREEN_WIDTH * 8u)
#define DEVICE_SCREEN_PX_HEIGHT (DEVICE_SCREEN_HEIGHT * 8u)

#define DEVICE_BITMAP_8BPP_BUFFER_PX_WIDTH   256u // Assumes BM_CTRL 8BPP 256 x 512 shared layout
#define DEVICE_BITMAP_8BPP_BUFFER_PX_HEIGHT  512

#define DEVICE_BITMAP_4BPP_BUFFER_PX_WIDTH   512u // Assumes BM_CTRL 4BPP 512 x 256 shared layout
#define DEVICE_BITMAP_4BPP_BUFFER_PX_HEIGHT  512u

#define DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH   (DEVICE_BITMAP_4BPP_BUFFER_PX_WIDTH / 2u)
#define DEVICE_BITMAP_4BPP_BUFFER_BYTE_HEIGHT  (DEVICE_BITMAP_4BPP_BUFFER_PX_HEIGHT / 2u)

#define DEVICE_BITMAP_4BPP_BUFFER_U16_WIDTH   (DEVICE_BITMAP_4BPP_BUFFER_PX_WIDTH / 2u)
#define DEVICE_BITMAP_4BPP_BUFFER_U16_HEIGHT  (DEVICE_BITMAP_4BPP_BUFFER_PX_HEIGHT / 2u)



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


#define S_8x8         (0 << 2)
#define S_16x16       (1 << 2)
#define S_16x32       (2 << 2)
#define S_32x32       (3 << 2)

#define S_FLIPX       (1 << 6)
#define S_FLIPY       (1 << 7)

#define S_Y_HIBIT     (1 << 1)
#define S_X_HIBIT     (1 << 0)
#define S_Y_HIBIT_ON  (1 << 1)
#define S_X_HIBIT_ON  (1 << 0)
#define S_HIBITS_MASK_OFF (0xFCu)
#define S_HIBITS_ON       (0x03u)

#define S_PAL0        (0 << 4)
#define S_PAL1        (1 << 4)
#define S_PAL2        (2 << 4)
#define S_PAL3        (3 << 4)


// No need for BANKREFS, zero them out
#define BANKREF(x)
#define BANKREF_EXTERN(x)

extern uint16_t sys_time;
extern bool     vbl_done;

typedef struct OAM_item_t {
    uint8_t tile;  //< Sprite tile number VDP.OAM[N].[31..24] 
    uint8_t y;     //< Y Coordinates (lowest 8 bits of 9) of the sprite on screen
    uint8_t prop;  //< OAM Property Flags
    uint8_t x;     //< X Coordinates (lowest 8 bits of 9) of the sprite on screen
} OAM_item_t;

// TODO: WARNING!: Need to use DMA to copy over shadow oam to actual oam
// // extern volatile struct OAM_item_t shadow_OAM[];
// #define shadow_OAM ((OAM_item_t *)VDP.OAM)
extern OAM_item_t shadow_OAM[SHADOW_OAM_MAX_SPRITES];


void enable_interrupt_nmi_vblank();
void disable_interrupt_nmi_vblank();

void enable_interrupt_irq0_vblank();
void disable_interrupt_irq0_vblank();

void enable_interrupt_irq1_vblank();
void disable_interrupt_irq1_vblank();

void INTERRUPT SMALLFUNC isr_nmi_vblank(void);
void INTERRUPT SMALLFUNC isr_irq0_vblank(void);
void INTERRUPT SMALLFUNC isr_irq1_vblank(void);

void shadow_oam_copy_dma(void);
void shadow_oam_copy_cpu(void);

void vsync(void);

// TODO: Docs
inline void set_sprite_tile(uint16_t nb, uint8_t tile) {
    shadow_OAM[nb].tile=tile;
}

inline uint8_t get_sprite_tile(uint16_t nb) {
    return shadow_OAM[nb].tile;
}

inline void set_sprite_prop(uint16_t nb, uint8_t prop) {
    shadow_OAM[nb].prop=prop;
}

inline uint8_t get_sprite_prop(uint16_t nb) {
    return shadow_OAM[nb].prop;
}

// First compiler quirk on gcc wonderful sh1?
// Build fails with unable to find move_sprite() at linking stage.
// Maybe compiler is choosing not to inline and then the header guard
// is preventing it from generating the function again? 
//
// Workaround is to use always_inline to force it.
//
// TODO: Using 8 bit x,y positions for now, but x and y are signed 9 bit (so -256 to 255)
ALWAYS_INLINE inline void move_sprite(uint16_t nb, uint8_t x, uint8_t y) {
    OAM_item_t * itm = &shadow_OAM[nb];
    itm->y=y, itm->x=x;
    itm->prop &= (S_HIBITS_MASK_OFF);  // TODO: Forcing X,Y High bits off for now
}

// TODO: Using 8 bit x,y positions for now, but x and y are signed 9 bit (so -256 to 255)
ALWAYS_INLINE inline void scroll_sprite(uint16_t nb, int8_t x, int8_t y) {
    OAM_item_t * itm = &shadow_OAM[nb];
    itm->y+=y, itm->x+=x;
    itm->prop &= (S_HIBITS_MASK_OFF);  // TODO: Forcing X,Y High bits off for now
}

ALWAYS_INLINE inline void hide_sprite(uint16_t nb) {
    OAM_item_t * itm = &shadow_OAM[nb];
    itm->y = 128;
    itm->prop |= S_Y_HIBIT_ON;  // Hide sprite by setting Y high (signed) bit and low y to give it a negative off-screen location
}

/** Turns on the sprites layers (OBJ0,OBJ1).
*/
#define SHOW_SPRITES \
  VDP.LAYER_CTRL |= (LAYER_ENABLE_OBJ0 | LAYER_ENABLE_OBJ1)

/** Turns off the sprites layers (OBJ0,OBJ1).
    @see hide_sprite, hide_sprites_range
*/
#define HIDE_SPRITES \
  VDP.LAYER_CTRL &= ~(LAYER_ENABLE_OBJ0 | LAYER_ENABLE_OBJ1)


/** Turns on the Screen B meta layer.
*/
#define SHOW_SCREEN_B \
  VDP.SCREENPRIO |= SCREEN_B_ENABLE

/** Turns off the Screen B meta layer.
*/
#define HIDE_SCREEN_B \
  VDP.SCREENPRIO &= ~SCREEN_B_ENABLE

/** Turns on the Screen A meta layer.
*/
#define SHOW_SCREEN_A \
  VDP.SCREENPRIO |= SCREEN_A_ENABLE

/** Turns off the Screen A meta layer.
*/
#define HIDE_SCREEN_A \
  VDP.SCREENPRIO &= ~SCREEN_A_ENABLE


/** Turns on the Background Tilemap 0 layer.
*/
#define SHOW_BKG_0 \
  VDP.LAYER_CTRL |= LAYER_ENABLE_BG0

/** Turns off the Background Tilemap 0 layer.
*/
#define HIDE_BKG_0 \
  VDP.LAYER_CTRL &= ~LAYER_ENABLE_BG0

/** Turns on the Background Tilemap 1 layer.
*/
#define SHOW_BKG_1 \
  VDP.LAYER_CTRL |= LAYER_ENABLE_BG1

/** Turns off the Background Tilemap 1 layer.
*/
#define HIDE_BKG_1 \
  VDP.LAYER_CTRL &= ~LAYER_ENABLE_BG1


typedef uint16_t palette_color_t;

#define COLS_PER_PAL_4BPP  16u
#define COLS_PER_PAL_8BPP 256u

#define RGB888(r, g, b) (((uint16_t)((((r) >> 3) & 0x1f) << 10)) | ((uint16_t)((((g) >> 3) & 0x1f) << 5)) | (((b) >> 3) & 0x1f))
#define RGB8 RGB888


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
void set_bkg_4bpp_data(unsigned int start, unsigned int ntiles, const uint16_t *src);


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


/** Set single tile t on background layer at x,y
    @param x X-coordinate
    @param y Y-coordinate
    @param t tile index

    @return returns the address of tile, so you may use faster set_vram_byte() later
*/
uint16_t * set_bkg_tile_xy(uint16_t x, uint16_t y, uint16_t t);
#define set_tile_xy set_bkg_tile_xy


/** Fills a rectangular region of Tile Map entries for the Background layer with tile.

    @param x      X Start position in Background Map tile coordinates. Range 0 - 31
    @param y      Y Start position in Background Map tile coordinates. Range 0 - 31
    @param w      Width of area to set in tiles. Range 1 - 32
    @param h      Height of area to set in tiles. Range 1 - 32
    @param tile   Fill value
*/
void fill_bkg_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t tile);


/** Sets a rectangular region of a Bitmap layer.

    @param x      X Start position in pixel coordinates. Range 0 - 510. Rounded down to even value. // TODO
    @param y      Y Start position in pixel coordinates. Range 0 - 255
    @param w      Width of area to set in tiles. Range 1 - 512. Rounded down to even value. // TODO
    @param h      Height of area to set in tiles. Range 1 - 32
    @param bitmap Pointer to source bitmap data
*/
void load_bitmap_4bpp(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint8_t *bitmap);


/** Set base memory address used by the tilemap writing functions,
    used for selecting between BG0 and BG1
 
  @param p_tilemap_base_address   Should point to the starting (0,0) position in Background Map tile coordinates

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


/** Set property flag to determine whether next loaded tiles render on Screen A or B that is ORed in when writing tilemap data
 
  @param screen_a_or_b   Select target output Screen `LAYER_SCREEN_A` or `LAYER_SCREEN_B`
 */
void set_bkg_tiles_target_screen_a_or_b(unsigned int screen_a_or_b);

/** Set BGx layer attributes subpal that is ORed in when writing tilemap data 
 
  @param subpal   Range 0-3 (PAL_0, PAL_1, PAL_2, PAL_3)
 */
void set_bkg_tiles_target_subpal(uint16_t subpal);

/** Delays the given number of milliseconds.
    Uses no timers or interrupts, and can be called with
    interrupts disabled
 */
void delay(uint16_t d);

#endif // _GBDK_LOOPY_H