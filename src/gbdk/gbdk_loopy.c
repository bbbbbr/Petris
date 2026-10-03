
#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#include "gbdk/platform.h"


#define IRQ_PRIORITY_LOWEST_OFF    0x0u
#define IRQ_PRIORITY_14            0xEu
#define IRQ_PRIORITY_15_HIGHEST    0xFu

#define VERT_SCANLINE_VBLANK_FIRST -39       // First VBlank Scanline (-39 -> 0 -> 224)
#define HORIZ_PIXEL_HBLANK_FIRST   -84       // First HBlank Pixel    (-84 -> 0 -> 257)

#define  U16_COUNT_PER_OAM_ENTRY  (2u)
#define NO_BIOS_VSYNC_BEFORE_WRITES

// Turn bios vsync timing on/off for writes
#ifdef NO_BIOS_VSYNC_BEFORE_WRITES
#else
#endif

#define CACHED_VCOUNT_INVALIDATED  0u
#define VCOUNT_SIGN_BIT  (1u << 8)

// Leave at least N scanline of safe buffer
#define WAIT_SAFE_VSYNC_LAST_SAFE_THRESHOLD 256u // -1 signed 9 bit as uint16, since it goes from 224 -> -39 (256 + 39)
#define WAIT_SAFE_VSYNC   while (cached_vdp_vcount < WAIT_SAFE_VSYNC_LAST_SAFE_THRESHOLD)

// Channel 1 DMA copy
#define DMA1_U16_MEMCOPY(dest, src, u16_count) \
    DMAC_DMAOR = DMA_DMAOR_AE_NO_ERROR | DMA_DMAOR_NMIF_NO_ERROR | DMA_DMAOR_DME_ENABLE; \
    DMAC_SAR1  = (uint32_t)(src);   \
    DMAC_DAR1  = (uint32_t)(dest);  \
    DMAC_TCR1  = (u16_count);       \
    DMAC_CHCR1 = (DMA_CHCR_DM_DEST_INCREMENT | DMA_CHCR_SM_SRC_INCREMENT | DMA_CHCR_RS_AUTO_CONF | DMA_CHCR_TM_BUS_BURST | DMA_CHCR_TS_XFER_WORD_U16 | DMA_CHCR_DE_XFER_ENABLE);

// When DMA is working this doesn't even decrement once
#define DMA1_WAIT_DONE() \
    volatile uint16_t timeout = 60000u; \
    while(!(DMAC_CHCR1 & DMA_CHCR_TE_XFER_IS_DONE) && timeout != 0u) { \
        timeout--; \
    } \
    cached_vdp_vcount = CACHED_VCOUNT_INVALIDATED; // Invalidate the vcount cache in case the dma blocked an update interrupt

#define DMA1_WAIT_DONE_NO_VCOUNT_INVALIDATE() \
    volatile uint16_t timeout = 60000u; \
    while(!(DMAC_CHCR1 & DMA_CHCR_TE_XFER_IS_DONE) && timeout != 0u) { \
        timeout--; \
    }


    // cached_vdp_vcount = VDP.VCOUNT; // Ooof, maybe can't do this because the dma to vram may have blocked the vdp from updating VCount?

static uint16_t * _bg_tilemap_base_address =  BG0_MAP_START();
static uint8_t  * _4bpp_tile_patterns_base_address = 0;
static uint16_t   _tilemap_screen_ab_prop = 0;
static uint16_t   _tilemap_subpal_prop = 0;
static uint16_t   _tilemap_cached_props = 0;

volatile uint16_t sys_time = 0;
volatile bool     vbl_done = false;
volatile int16_t  simulated_vdp_vcount = VERT_SCANLINE_VBLANK_FIRST;
volatile uint16_t cached_vdp_vcount;  // Unsigned, stores raw 9 bit VDP.VCount without translating it to uint16_t
volatile uint16_t vdp_vcount_s16;     // VDP.VCount translated from 9 bit signed to 16 bit signed


OAM_item_t shadow_OAM[SHADOW_OAM_MAX_SPRITES];
void (* registered_irq0_handler)(void);

#define VRAM_XFER_BUF_SZ    512u
uint16_t vram_xfer_buf[VRAM_XFER_BUF_SZ];


/*
void dma1_u16_memcopy_vram_safe(uint16_t * dest, uint16_t * src, uint16_t u16_count) {

    // Prepare the transfer
    DMAC_DMAOR = DMA_DMAOR_AE_NO_ERROR | DMA_DMAOR_NMIF_NO_ERROR | DMA_DMAOR_DME_ENABLE; \
    DMAC_SAR1  = (uint32_t)(src);
    DMAC_DAR1  = (uint32_t)(dest);
    DMAC_TCR1  = (u16_count);

    // Wait for safe vram access
    while (simulated_vdp_vcount >= (WAIT_SAFE_VSYNC_LAST_SAFE_THRESHOLD));
    // Start the transfer
    DMAC_CHCR1 = (DMA_CHCR_DM_DEST_INCREMENT | DMA_CHCR_SM_SRC_INCREMENT | DMA_CHCR_RS_AUTO_CONF | DMA_CHCR_TM_BUS_BURST | DMA_CHCR_TS_XFER_WORD_U16 | DMA_CHCR_DE_XFER_ENABLE);

    // Wait for done
    volatile uint16_t timeout = 60000u;
    while(!(DMAC_CHCR1 & DMA_CHCR_TE_XFER_IS_DONE) && timeout != 0u) {
        timeout--;
    }
}
*/

// Copy shadow oam via DMA
//
// WARNING: DMA CANNOT be used when VDP NMI VBlank is enabled.
//          Will fail to start with DMA_DMAOR_NMIF_BLOCKED_BY_NMI set in DMAOR
void shadow_oam_copy_dma(void) {

    // Enable and clear any previously set error flags
    DMAC_DMAOR = DMA_DMAOR_AE_NO_ERROR | DMA_DMAOR_NMIF_NO_ERROR | DMA_DMAOR_DME_ENABLE;

    // DMA using channel 0
    // Transfer count (size in u16, which is VRAM access size, each OAM entry is 32 bits, so 2 per entry)
    DMAC_SAR0  = (uint32_t)shadow_OAM;        // Src
    DMAC_DAR0  = (uint32_t)VDP.OAM;           // Dest
    DMAC_TCR0  = SHADOW_OAM_MAX_SPRITES * U16_COUNT_PER_OAM_ENTRY;
    DMAC_CHCR0 = (DMA_CHCR_DM_DEST_INCREMENT | DMA_CHCR_SM_SRC_INCREMENT | DMA_CHCR_RS_AUTO_CONF | DMA_CHCR_TM_BUS_BURST | DMA_CHCR_TS_XFER_WORD_U16 | DMA_CHCR_DE_XFER_ENABLE);

    // When DMA is working this doesn't even decrement once
    uint16_t timeout = 60000u;
    while(!(DMAC_CHCR0 & DMA_CHCR_TE_XFER_IS_DONE) && timeout != 0u) {
        timeout--;
    }
}


// Copy shadow oam via partially unrolled CPU loop
void shadow_oam_copy_cpu(void) {

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


// WARNING: DMA CANNOT be used when VDP NMI VBlank is enabled.
//          Will fail to start with DMA_DMAOR_NMIF_BLOCKED_BY_NMI set in DMAOR
void enable_interrupt_nmi_vblank(void) {
    VDP.IRQ0_NMI_CTRL |= NMI_ENABLE;
}


void disable_interrupt_nmi_vblank(void) {
    VDP.IRQ0_NMI_CTRL &= ~NMI_ENABLE;
}


void enable_interrupt_irq0_vblank(void) {

    VDP.IRQ0_VCMP = VERT_SCANLINE_VBLANK_FIRST;  // First VBlank Scanline (-39 -> 0 -> 224)
    VDP.IRQ0_HCMP = HORIZ_PIXEL_HBLANK_FIRST;    // First HBlank Pixel    (-84 -> 0 -> 257)
    VDP.IRQ0_NMI_CTRL |= (IRQ0_ENABLE | IRQ0_VCMP_ENABLE);
    sys_setInterruptPriority(INT_PRIO_IRQ0, IRQ_PRIORITY_15_HIGHEST);
    // Set Global interrupt priority mask level to be 1 below the level configured above
    sys_setInterruptMask(IRQ_PRIORITY_14);
}

void enable_interrupt_irq0_hblank(void) {

    VDP.IRQ0_VCMP = VERT_SCANLINE_VBLANK_FIRST;  // First VBlank Scanline (-39 -> 0 -> 224)
    VDP.IRQ0_HCMP = HORIZ_PIXEL_HBLANK_FIRST;    // First HBlank Pixel    (-84 -> 0 -> 257)
    VDP.IRQ0_NMI_CTRL &= ~IRQ0_VCMP_ENABLE;      // Select HBlank mode by turning off VBlank mode bit
    VDP.IRQ0_NMI_CTRL |= IRQ0_ENABLE;
    sys_setInterruptPriority(INT_PRIO_IRQ0, IRQ_PRIORITY_15_HIGHEST);
    // Set Global interrupt priority mask level to be 1 below the level configured above
    sys_setInterruptMask(IRQ_PRIORITY_14);
}


void disable_interrupt_irq0_vblank(void) {
    VDP.IRQ0_NMI_CTRL &= ~IRQ0_ENABLE;
}


void enable_interrupt_irq1_vblank(void) {
    // Enable IRQ1 VBlank in VDP with VBlank mode
    VDP.SYNC_IRQ_CTRL = (IRQ1_ENABLE | IRQ1_SRC_VSYNC);

    // Set trigger to Falling edge to match VDP output behavior. If this isn't set it will trigger repeatedly during vblank
    INTC_ICR  = (INTC_ICR & ~INTC_ICR_IRQ1S_MASK) | INTC_ICR_IRQ1S_TRIG_FALLING_EDGE;
    PFC_PACR1 = (PFC_PACR1 & ~PA13_MD10_MODE_MASK) | PA13_MD10_MODE_IRQ1;

    sys_setInterruptPriority(INT_PRIO_IRQ1, IRQ_PRIORITY_15_HIGHEST);
    // Set Global interrupt priority mask level to be 1 below the level configured above
    sys_setInterruptMask(IRQ_PRIORITY_14);
}


void disable_interrupt_irq1_vblank(void) {
    VDP.SYNC_IRQ_CTRL &= ~IRQ1_ENABLE;
}


void add_irq0(void (* handler)(void)) {
    registered_irq0_handler = handler;
}


void remove_irq0(void) {
    registered_irq0_handler = NULL;
}


// Vblank and HBlank Interrupt Source Comparison
//
// "++" Means trigger line/column is configurable
//
//        VBlank    HBlank   Notes
// NMI    Y         N        * DMA CANNOT be used when VDP NMI VBlank is enabled
//
// IRQ0   Y++       Y++
//
// IRQ1   Y         Y        * Can be used to trigger DMA instead of an interrupt
//


void INTERRUPT SMALLFUNC isr_nmi_vblank(void) {
}


void INTERRUPT SMALLFUNC isr_irq0_vblank_hblank(void) {
    simulated_vdp_vcount++;  // Part of the problem might be this breaking down when DMA transfers happen? (i.e. skipped hcounts never get restored?)
    cached_vdp_vcount = VDP.VCOUNT;

    if (registered_irq0_handler) registered_irq0_handler();
}


void INTERRUPT SMALLFUNC isr_irq1_vblank(void) {
    shadow_oam_copy_dma();
    sys_time++;
    vbl_done = true;

    // Hardwiring to -39 since there seems to be jitter in reading HCOUNT (maybe a clash with the HBlank ISR?)
    simulated_vdp_vcount = -39; // (uint16_t)VDP.HCOUNT;
    cached_vdp_vcount = VDP.VCOUNT;
}


// Note: Loopy bios vsync also polls controller(s)
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

    WAIT_SAFE_VSYNC;
    DMA1_U16_MEMCOPY(p_pal, rgb_data, (nb_palettes * COLS_PER_PAL_4BPP));
    DMA1_WAIT_DONE();

    /* 
    // NON-DMA style    
    for (unsigned int pal = 0; pal < nb_palettes; pal++) {
        for (unsigned int col = 0; col < COLS_PER_PAL_4BPP; col++) {
            *p_pal++ = *rgb_data++;
        }
    }
    */
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

    size_t copytiles = ntiles;
    while (copytiles--) {

        WAIT_SAFE_VSYNC;
        DMA1_U16_MEMCOPY(p_dest, src, U16_WORDS_PER_4BPP_TILE);
        DMA1_WAIT_DONE();
        src += U16_WORDS_PER_4BPP_TILE;
        p_dest += U16_WORDS_PER_4BPP_TILE;

        /*         
        // NON-DMA style    
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
        */
    }
}


void set_bkg_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles) {

          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    while (height--) {
        uint16_t row_len = width;

        uint16_t * p_xfer = vram_xfer_buf;
        while (row_len--) {
            *p_xfer++ = *tiles++ | _tilemap_cached_props;
        }
        WAIT_SAFE_VSYNC;
        DMA1_U16_MEMCOPY(p_dest, vram_xfer_buf, width);
        DMA1_WAIT_DONE();
        p_dest += width + row_stride;        

        /* 
        // NON-DMA style    
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
        */        

    }
}


void set_bkg_based_tiles(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t *tiles, unsigned int base_tile) {

          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    while (height--) {
        uint16_t row_len = width;

        uint16_t * p_xfer = vram_xfer_buf;
        while (row_len--) {
            *p_xfer++ = (*tiles & ~BG_TILEMAP_CHRNUM_MASK) | ((*tiles & BG_TILEMAP_CHRNUM_MASK) + base_tile) |  _tilemap_cached_props;
            tiles++;            
        }
        // dma1_u16_memcopy_vram_safe(p_dest, vram_xfer_buf, width);
        WAIT_SAFE_VSYNC;
        DMA1_U16_MEMCOPY(p_dest, vram_xfer_buf, width);
        DMA1_WAIT_DONE();
        p_dest += width + row_stride;

        /* 
        // NON-DMA style    
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
        */
    }
}


uint16_t * set_bkg_tile_xy(uint16_t x, uint16_t y, uint16_t tile) {

    uint16_t * p_dest = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    WAIT_SAFE_VSYNC;
    *p_dest = tile | _tilemap_cached_props;

    return p_dest;
}


void fill_bkg_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint16_t tile) {

          uint16_t * p_dest     = _bg_tilemap_base_address + (y * DEVICE_SCREEN_BUFFER_WIDTH) + x;
    const uint32_t   row_stride = DEVICE_SCREEN_BUFFER_WIDTH - width;

    // Prepare a row to repeatedly copy
    uint16_t * p_xfer = vram_xfer_buf;
    uint16_t row_len = width;
    while (row_len--) {
        *p_xfer++ = tile | _tilemap_cached_props;
    }        

    while (height--) {

        WAIT_SAFE_VSYNC;
        DMA1_U16_MEMCOPY(p_dest, vram_xfer_buf, width);
        DMA1_WAIT_DONE();
        p_dest += width + row_stride;

        /* 
        // NON-DMA style
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
        */
    }
}


// TODO: this could be a DMA
// Only operates on width multiples of 2, X is rounded down to multiple of 2
void load_bitmap_4bpp(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const uint8_t *bitmap) {

        if (width & 0x0001) return; // TODO: handle odd numbered widths and start x (requires splitting and shifting all bytes)

        if ((width & 0x0003) != 0u) return; // Note: Handle u16 DMA limitation, only operates on 4 pixel widths at a time (one u16).

        x /= 2;      // 4BPP packs 2 pixels into 1 byte
        width /= 2;  // 4BPP packs 2 pixels into 1 byte

           uint8_t * p_dest     = VDP.BITMAP_VRAM_8BIT + ((y * DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH) + x);
    const uint32_t   row_stride = DEVICE_BITMAP_4BPP_BUFFER_BYTE_WIDTH - width;

    while (height--) {

        // Bitmap vram is dual ported, safe access timing is only needed if you want to avoid tearing
        // WAIT_SAFE_VSYNC;
        DMA1_U16_MEMCOPY((uint16_t *)p_dest, (uint16_t *)bitmap, width / 2u);  // Transfer size is (/ 2) for 2 bytes per u16
        DMA1_WAIT_DONE_NO_VCOUNT_INVALIDATE();
        p_dest += width + row_stride;
        bitmap += width;

        /*
        // NON-DMA style
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
        */
        
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