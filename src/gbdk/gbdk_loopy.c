
#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#include "gbdk/platform.h"

static uint16_t * _bg_tilemap_base_address =  BG0_MAP_START();
static uint8_t  * _4bpp_tile_patterns_base_address = 0;
static uint16_t   _tilemap_screen_ab_prop = 0;
static uint16_t   _tilemap_subpal_prop = 0;
static uint16_t   _tilemap_cached_props = 0;

uint16_t sys_time = 0;
bool     vbl_done = false;
OAM_item_t shadow_OAM[SHADOW_OAM_MAX_SPRITES];


#define NO_BIOS_VSYNC_BEFORE_WRITES

// Turn bios vsync timing on/off for writes
#ifdef NO_BIOS_VSYNC_BEFORE_WRITES
    #define OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES()
#else
    #define OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES()  bios_vsync();  // TODO: FIXME With safe (?) VDP write timing, to at least reduce tearing  
#endif


// Copy shadow oam via DMA
//
// WARNING: DMA CANNOT be used when VDP NMI VBlank is enabled.
//          Will fail to start with DMA_DMAOR_NMIF_BLOCKED_BY_NMI set in DMAOR
void shadow_oam_copy_dma(void) {

    DMAC_DMAOR = DMA_DMAOR_AE_NO_ERROR | DMA_DMAOR_NMIF_NO_ERROR | DMA_DMAOR_DME_ENABLE;  // Enable and clear any previously set error flags

    // DMA using channel 0
    DMAC_SAR0  = (uint32_t)shadow_OAM; // Src
    DMAC_DAR0  = (uint32_t)VDP.OAM;                // Dest     // 0x0C050000
    DMAC_TCR0  = SHADOW_OAM_MAX_SPRITES * 2u; // Transfer count (size in u16, which is VRAM access size, each OAM entry is 32 bits, so 2 per entry)
    DMAC_CHCR0 = (DMA_CHCR_DM_DEST_INCREMENT | DMA_CHCR_SM_SRC_INCREMENT | DMA_CHCR_RS_AUTO_CONF | DMA_CHCR_TM_BUS_BURST | DMA_CHCR_TS_XFER_WORD_U16 | DMA_CHCR_DE_XFER_ENABLE);

    // DMA is fast enough when working that this doesn't even decrement once
    uint16_t timeout = 60000u;
    while(!(DMAC_CHCR0 & DMA_CHCR_TE_XFER_IS_DONE) && timeout != 0u) {
        timeout--;
    }
}


// Copy shadow oam via partially unrolled CPU loop
void shadow_oam_copy(void) {

    volatile uint32_t * p_OAM = VDP.OAM;
    volatile uint32_t * p_src = (uint32_t *)shadow_OAM;

    for (uint16_t c = 0; c < (SHADOW_OAM_MAX_SPRITES / VSYNC_OAM_COPY_SZ); c++) {
        // Number of unrolled writes here should match VSYNC_OAM_COPY_SZ
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;

        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
        *p_OAM++ = *p_src++;
    }
}


void enable_interrupt_nmi_vblank() {
    VDP.IRQ0_NMI_CTRL |= NMI_ENABLE;
}


void disable_interrupt_nmi_vblank() {
    VDP.IRQ0_NMI_CTRL &= ~VDP.IRQ0_NMI_CTRL;
}


void enable_interrupt_irq0_vblank() {
    #define IRQ_PRIORITY_LOWEST_OFF    0x0u
    #define IRQ_PRIORITY_14            0xEu
    #define IRQ_PRIORITY_15_HIGHEST    0xFu

    #define VERT_SCANLINE_VBLANK_FIRST -39 // First VBlank Scanline (-39 -> 0 -> 224)
    #define HORIZ_PIXEL_HBLANK_FIRST   -84 // First HBlank Pixel    (-84 -> 0 -> 257)

    VDP.IRQ0_VCMP = VERT_SCANLINE_VBLANK_FIRST; // First VBlank Scanline (-39 -> 0 -> 224)
    VDP.IRQ0_HCMP = HORIZ_PIXEL_HBLANK_FIRST; // First HBlank Pixel    (-84 -> 0 -> 257)
    VDP.IRQ0_NMI_CTRL |= (IRQ0_ENABLE | IRQ0_VCMP_ENABLE);
    sys_setInterruptPriority(INT_PRIO_IRQ0, IRQ_PRIORITY_15_HIGHEST);
    sys_setInterruptMask(IRQ_PRIORITY_14); // Set Global interrupt priority mask level to be 1 below the level configured above
}


void disable_interrupt_irq0_vblank() {
    VDP.IRQ0_NMI_CTRL &= ~IRQ0_ENABLE;
}

// Hardware/Emulator status:
//
// - Hardware: Gets called, works
// - LoopyMSE: Crash
// - CLoopy:   Does not appear to get called
//
void INTERRUPT SMALLFUNC isr_nmi_vblank(void) {
}


void INTERRUPT SMALLFUNC isr_irq0_vblank(void) {
    shadow_oam_copy_dma();
    sys_time++;
    vbl_done = true;
}


// Note: Loopy bios vsync also polls controller(s)
// #define vsync  bios_vsync  
void vsync(void) {
    bios_vsync();
}

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
void set_bkg_4bpp_palette(unsigned int first_palette, unsigned int nb_palettes, const palette_color_t *rgb_data) {

    uint16_t * p_pal = &VDP.PALETTE[first_palette * COLS_PER_PAL_4BPP];

    for (unsigned int pal = 0; pal < nb_palettes; pal++) {
        for (unsigned int col = 0; col < COLS_PER_PAL_4BPP; col++) {
            *p_pal++ = *rgb_data++;
        }
    }
}



// TODO: convert to u16 to reduce number of writes
/** Sets VRAM Tile Pattern data in the 4bpp format

    @param first_tile  Index of the first tile to write (0 - 511)
    @param nb_tiles    Number of tiles to write
    @param data        Pointer to source Tile Pattern data.
 */
void set_bkg_4bpp_data(unsigned int start, unsigned int ntiles, const uint16_t *src) {

    // Important! Writes to Tile VRAM *MUST* be 16 bit, 8 bit writes
    // on real hardware will result in every other byte of tile pattern
    // data being corrupted, yielding vertical lines on the screen (palette dependent).
    
    // Offset into start of 4bpp tile pattern data based on 
    uint16_t * p_dest = (start * U16_WORDS_PER_4BPP_TILE) + (uint16_t *)_4bpp_tile_patterns_base_address;

    // TODO: Use DMA (make a vmemcpy shim?)
    // Tile VRAM is not dual-ported, so requires safe access timing, unlike bitmap vram
    // TODO: This is the shoddiest safe access timing...
    OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES();
    size_t copybytes = ntiles;
    while (copybytes--) {
        // Write one 8x8 32 byte tile entry as a block
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;

        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
        *p_dest++ = *src++;
    }
}


void set_bkg_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles) {
    
          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES();
    while (height--) {
        uint16_t row_len = width;
        uint16_t row_wrap = DEVICE_SCREEN_BUFFER_WIDTH - x;
        while (row_len--) {
            *p_dest++ = *tiles++ | _tilemap_cached_props;
            // Check for wraparound from right edge -> left.
            // In that case, preserve current row instead of letting it step down to next
            row_wrap--;
            if ((row_wrap == 0) && (row_len != 0)) {
                p_dest -= DEVICE_SCREEN_BUFFER_WIDTH;
                row_wrap = DEVICE_SCREEN_BUFFER_WIDTH;
            }
        }
        p_dest += row_stride;
    }
}


void set_bkg_based_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles, unsigned int base_tile) {

          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES();
    while (height--) {
        uint16_t row_len = width;
        uint16_t row_wrap = DEVICE_SCREEN_BUFFER_WIDTH - x;
        while (row_len--) {
            // Mask out tile ID then OR in isolated tile ID + offset, OR in properties
            *p_dest++ = (*tiles & ~BG_TILEMAP_CHRNUM_MASK) | ((*tiles & BG_TILEMAP_CHRNUM_MASK) + base_tile) |  _tilemap_cached_props;
            tiles++;
            // Check for wraparound from right edge -> left.
            // In that case, preserve current row instead of letting it step down to next
            row_wrap--;
            if ((row_wrap == 0) && (row_len != 0)) {
                p_dest -= DEVICE_SCREEN_BUFFER_WIDTH;
                row_wrap = DEVICE_SCREEN_BUFFER_WIDTH;
            }
        }
        p_dest += row_stride;
    }
}


uint16_t * set_bkg_tile_xy(uint16_t x, uint16_t y, uint16_t tile) {

    uint16_t * p_dest = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    *p_dest = tile | _tilemap_cached_props;

    return p_dest;
}


void fill_bkg_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t tile) {

          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES();
    while (height--) {
        uint16_t row_len = width;
        uint16_t row_wrap = DEVICE_SCREEN_BUFFER_WIDTH - x;
        while (row_len--) {
            *p_dest++ = tile | _tilemap_cached_props;
            // Check for wraparound from right edge -> left.
            // In that case, preserve current row instead of letting it step down to next
            row_wrap--;
            if ((row_wrap == 0) && (row_len != 0)) {
                p_dest -= DEVICE_SCREEN_BUFFER_WIDTH;
                row_wrap = DEVICE_SCREEN_BUFFER_WIDTH;
            }
        }
        p_dest += row_stride;
    }
}


// TODO: this could be a DMA
// Only operates on width multiples of 2, X is rounded down to multiple of 2
void load_bitmap_4bpp(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint8_t *bitmap) {

        if (width & 0x0001) return; // TODO: handle odd numbered widths and start x (requires splitting and shifting all bytes)

          x /= 2;      // 4BPP packs 2 pixels into 1 byte
          width /= 2;  // 4BPP packs 2 pixels into 1 byte

           uint8_t * p_dest     = VDP.BITMAP_VRAM_8BIT + ((y * DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH) + x);
    const uint32_t   row_stride = DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH - width;

    OPTIONAL_IF_ENABLED_BIOS_VSYNC_BEFORE_VDP_WRITES();
    while (height--) {
        uint16_t row_len = width;
        uint16_t row_wrap = DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH - x;
        while (row_len--) {
            *p_dest++ = *bitmap++;
            // Check for wraparound from right edge -> left.
            // In that case, preserve current row instead of letting it step down to next
            row_wrap--;
            if ((row_wrap == 0) && (row_len != 0)) {
                p_dest -= DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH;
                row_wrap = DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH - x;
            }
        }
        p_dest += row_stride;
    }
}


void set_bkg_tilemap_base_address(uint16_t * p_tilemap_base_address) {
    _bg_tilemap_base_address = p_tilemap_base_address;
}

void set_4bpp_tile_patterns_base_address(uint8_t * p_tile_patterns_base_address) {
    _4bpp_tile_patterns_base_address = p_tile_patterns_base_address;
}

void set_bkg_tiles_target_screen_a_or_b(unsigned int screen_a_or_b) {
    if (screen_a_or_b & LAYER_SCREEN_B)
        _tilemap_screen_ab_prop = BG_TILEMAP_SCREEN_B;
    else
        _tilemap_screen_ab_prop = 0;
    // Update merged, cached props
    _tilemap_cached_props = _tilemap_subpal_prop | _tilemap_screen_ab_prop;
}

void set_bkg_tiles_target_subpal(uint16_t subpal) {
    _tilemap_subpal_prop = BG_PAL(subpal & 0x03u);
    // Update merged, cached props
    _tilemap_cached_props = _tilemap_subpal_prop | _tilemap_screen_ab_prop;

}


// TODO: rewrite in ASM, count actual cycles
//
// 1000 msec ÷ 59.8261 hz = 16.715 FPS
// 16.715 FPS  / 263 lines = 0.06355 msec per line
// 1 msec / 0.06355 msec per line = 15.73 lines per msec
void delay(uint16_t msecs_delay) {
    for (volatile uint16_t c = 0u; c < msecs_delay; c++) {
        for (volatile uint16_t one_msec_loop = 0u; one_msec_loop < 0x01A0u; one_msec_loop++) {
                __asm__ volatile("nop");
        }
    }
}



// void set_native_tile_data(uint16_t start, uint16_t ntiles, const void *src) PRESERVES_REGS(iyh, iyl);
// void set_bkg_4bpp_data(uint16_t start, uint16_t ntiles, const void *src) PRESERVES_REGS(iyh, iyl);
// void set_bkg_native_data(uint16_t start, uint16_t ntiles, const void *src) PRESERVES_REGS(iyh, iyl);


// void set_tile_map(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles) Z88DK_CALLEE;
// void set_tile_map_compat(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles) Z88DK_CALLEE;
// #define set_bkg_tiles set_tile_map_compat
// #define set_win_tiles set_tile_map_compat



/*
// gbdk [convert]
initrand()
rand()

add_LCD()
add_VBL()
disable_interrupts()
enable_interrupts()
init_interrupts()
set_interrupts()

vsync()
delay()

move_sprite()
set_bkg_data()
x set_bkg_palette()
set_bkg_tiles()
set_sprite_data()
set_sprite_palette()
set_sprite_prop()
set_sprite_tile()

~some kind of shadow oam/copy
*/