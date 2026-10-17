#include <gbdk/platform.h>

#include "fade.h"

// #define DEBUG_SLOW_FADE

#define PAL_FADE_DONE  (0u)
#define PAL_FADE_STEPS (31u) // Max value for a 5 bit color channel

#define R_MASK (FIXED_POINT_5_5_MASK << 22)
#define G_MASK (FIXED_POINT_5_5_MASK << 11)
#define B_MASK (FIXED_POINT_5_5_MASK << 0)


#define RGB555_R(rgb555) (((rgb555) >> 10) & 0x1Fu)
#define RGB555_G(rgb555) (((rgb555) >> 5) & 0x1Fu)
#define RGB555_B(rgb555) (((rgb555) >> 0) & 0x1Fu)
//        3322222222221111 1111110000000000
//        1098765432109876 5432109876543210
//            |    :     |     :    |     :
//        fedcba9876543210 fedcba9876543210
// RGB555                  xRRRRRGGGGGBBBBB
// RGB5FP RRRRRrrrrrxGGGGG gggggxBBBBBbbbbb (C.c 5.5 fixed point)

#define FIXED_POINT_5_5_MASK                    (0b1111111111u)
#define PALETTE_INCREMENT_MASKOUT_ROLLOVER_BITS (0b11111111110111111111101111111111u)

#define U16RGB555_TO_U32RGBFIXEDPOINT_GAPPED(col) ( ((uint32_t)RGB555_R(col) << 27)  | ((uint32_t)RGB555_G(col) << 16)   | ((uint32_t)RGB555_B(col) << 5) )
#define U32RGBFIXEDPOINT_GAPPED_TO_U16RGB555(col) ( (((col) & (0x1Fu << 27)) >> 17)  | (((col) & (0x1Fu << 16)) >> 11)   | (((col) & (0x1Fu <<  5)) >> 5) )



static volatile bool     palette_fade_active      = false;
static volatile bool     palette_fade_is_to_black = false;
static volatile uint16_t palette_fade_count       = PAL_FADE_DONE;

static uint16_t full_palette_cached[COLS_PER_PAL_8BPP];
static uint16_t full_palette_target[COLS_PER_PAL_8BPP];

static uint32_t palette_increments_fixedpoint[COLS_PER_PAL_8BPP];
static uint32_t palette_cached_fixedpoint[COLS_PER_PAL_8BPP];

static uint16_t palette_debug_slowdown;


static void palette_fade_fixed_point_calc_increments(void);
static void palette_fade_fixed_point_calc_update(void);
static void palette_fade_to_target_finish(void);

// static void palette_fade_calc_update(void);
// static void palette_fade_to_black_calc_update(void);


// Palette fading:
//
// - Runs asynchronously in VBlank (write palette) and VBlank-Ended (calc next fade palette) with control flag
// - Can be waited on using palette_fade_wait_completed()
// - Set fade target palettes with set_fade_4bpp_palette()
//
// - Fade in operates on cached full_palette_cached -> full_palette_target

// Fixed point palette fades:
//
// 0. Writing palette upscales to 32 bits and stores in:  palette_cached_fixedpoint
//     - The u16 RGB55 is loaded into a u32 with 1 bit spacers between each channel to absorb addition/subtraction rollover
//     - RGB555 -> RRRRRrrrrrxGGGGGgggggxBBBBBbbbbb
//       where uppercase is Face value and lowercase is fractional value
// 1. Calculate 5 bit increment per channel (upscaled to 32 bits)  uint32_t palette_increments[256];
// 2.A. Per update add bits + mask                                 uint32_t palette_cached_fixedpoint[256];
// 2.B. Mask down to RGB555 and write to cached
// 3. Copy final palette at end?


// Sets cached and target palettes to all black
void palettes_init(void) {

    palette_fade_count       = PAL_FADE_DONE;
    palette_fade_active      = false;
    palette_fade_is_to_black = false;

    for (uint16_t color = 0; color < COLS_PER_PAL_8BPP; color++) {
        full_palette_cached[color] = RGB888(0,0,0);
        full_palette_target[color] = RGB888(0,0,0);
    }
}


// Waits for an active palette fade to complete
void palette_fade_wait_completed(void) {

    while(palette_fade_active) {
        vsync();
    }
}


// Waits for an active palette fade to complete
bool palette_fade_is_running(void) {

    return (palette_fade_active);
}


// Starts a fade toward black or full_palette_target[]
// Adds interrupt handlers to VBlank and VBlank-ended
void palette_fade_start(bool is_FADE_TO_BLACK) {

    // Critical section since the control vars may be modified by interrupts
    // This seems to have fixed an occasional glitch between fade out->in
    volatile uint32_t status_register_saved = disable_interrupts();

        palette_fade_count       = PAL_FADE_STEPS;
        palette_fade_active      = true;
        palette_debug_slowdown   = 0u;
        palette_fade_is_to_black = is_FADE_TO_BLACK;

        // Register the ISR handlers
        add_vbl(palette_fade_vlb_handler);
        add_vbl_ended(palette_fade_vlb_ended_handler);

    enable_interrupts(status_register_saved);

    palette_fade_fixed_point_calc_increments();
}


// Ends a fade and removes interrupt handlers
static void palette_fade_to_target_end(void) {

    // Critical section
    volatile uint32_t status_register_saved = disable_interrupts();

        // Clear state flag and de-register the ISR handlers
        palette_fade_active = false;
        remove_vbl();
        remove_vbl_ended();

    enable_interrupts(status_register_saved);
}


void palette_fade_vlb_handler(void) {
    
    // If palette fade is active then copy the cached palette to the hardware vdp palette
    if (palette_fade_active) {
        // Brute version, fade all colors instead of flagged sub-palettes // TODO: operate on sub-palettes of 16 instead
        set_bkg_4bpp_palette(PAL_0, NUM_4BPP_PALS, full_palette_cached);
    }
}


// Sets a fade target 4bpp sub-palette (does not apply until a fade start is triggered)
void set_fade_target_4bpp_palette(unsigned int first_palette, unsigned int nb_palettes, const palette_color_t *rgb_data) {

    uint16_t * p_pal = &full_palette_target[first_palette * COLS_PER_PAL_4BPP];
    DMA1_U16_MEMCOPY_AND_WAIT_DONE(p_pal, rgb_data, (nb_palettes * COLS_PER_PAL_4BPP));
}


// Set a single color entry in the fade target 4bpp sub-palette (does not apply until a fade start is triggered)
void set_fade_target_4bpp_palette_entry(unsigned int palette, unsigned int entry, palette_color_t rgb_data) {
    full_palette_target[(palette * COLS_PER_PAL_4BPP) + entry] = rgb_data;
}


// Called from simulated end of VBlank interrupt
void palette_fade_vlb_ended_handler(void) {

    // TODO: DEBUG TEST: SLOW DAWN FADES A LOT
    #ifdef DEBUG_SLOW_FADE
        palette_debug_slowdown++;
        if ((palette_debug_slowdown & 0x000Fu) != 0u) return;
    #endif

    if (palette_fade_active) {
        palette_fade_fixed_point_calc_update();
        /*
            if (palette_fade_is_to_black) palette_fade_to_black_calc_update();
            else                          palette_fade_calc_update();
        */
    }
}


static void palette_fade_fixed_point_calc_update(void) {

    if (palette_fade_count == PAL_FADE_DONE) {
        palette_fade_to_target_end();
    }
    else {
        palette_fade_count--;

        // On final fade step just copy the target palette to cached to ensure
        // fixed point rounding errors don't prevent perfectly matching expected colors
        if (palette_fade_count == PAL_FADE_DONE) {
            if (palette_fade_is_to_black) {
                // Source is "volatile" to ensure the compiler doesn't optimize it (and it's address) out!
                volatile palette_color_t black = RGB555(0u, 0u, 0u);
                DMA1_U16_MEMSET_FROM_VAR_AND_WAIT_DONE(full_palette_cached, &black, (NUM_4BPP_PALS * COLS_PER_PAL_4BPP));
            } else {
                DMA1_U16_MEMCOPY_AND_WAIT_DONE(full_palette_cached, full_palette_target, (NUM_4BPP_PALS * COLS_PER_PAL_4BPP));
            }
        }
        else {
            for (uint16_t c = 0; c < COLS_PER_PAL_8BPP; c++) {

                // palette_cached_fixedpoint[c] =
                //     (((palette_cached_fixedpoint[c] + (palette_increments_fixedpoint[c] & R_MASK)) & R_MASK)) |
                //     (((palette_cached_fixedpoint[c] + (palette_increments_fixedpoint[c] & G_MASK)) & G_MASK)) |
                //     (((palette_cached_fixedpoint[c] + (palette_increments_fixedpoint[c] & B_MASK)) & B_MASK));

                // Simpler version of the above calc (it works!)
                palette_cached_fixedpoint[c] += palette_increments_fixedpoint[c];
                // Mask out any bits that rolled over between the channels
                palette_cached_fixedpoint[c] &= PALETTE_INCREMENT_MASKOUT_ROLLOVER_BITS;
                full_palette_cached[c] = (uint16_t)U32RGBFIXEDPOINT_GAPPED_TO_U16RGB555(palette_cached_fixedpoint[c]);
            }
        }
    }
}


static void palette_fade_fixed_point_calc_increments(void) {

    for (uint16_t c = 0; c < COLS_PER_PAL_8BPP; c++) {
        // Upscale current cached palette to 5.5 fixed point rgb
        palette_cached_fixedpoint[c] = U16RGB555_TO_U32RGBFIXEDPOINT_GAPPED(full_palette_cached[c]);

        uint32_t r_inc, g_inc, b_inc;

        // Calculate fixed point increments based on the target - cached difference
        // The result of the calculation is the X.5 portion (non-upshifted) of 5.5 fixed point increment value
        // The mask is to limit the 5.5 fixed point unsigned negative value wraparound to 10 bits
        if (palette_fade_is_to_black) {
            r_inc = ((0u - RGB555_R(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
            g_inc = ((0u - RGB555_G(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
            b_inc = ((0u - RGB555_B(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
        } else {
            r_inc = ((RGB555_R(full_palette_target[c]) - RGB555_R(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
            g_inc = ((RGB555_G(full_palette_target[c]) - RGB555_G(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
            b_inc = ((RGB555_B(full_palette_target[c]) - RGB555_B(full_palette_cached[c])) & FIXED_POINT_5_5_MASK);
        }

        // The final 5.5 version looks like this: RRRRRrrrrrxGGGGGgggggxBBBBBbbbbb
        // with the increment values being mostly in the fractional (lowercase) bits
        palette_increments_fixedpoint[c] = (r_inc << 22) | (g_inc << 11) | (b_inc);
    }
}


/*
// Naive fade implementation

// Calculate the next palette during a fade
static void palette_fade_calc_update(void) {    

    if (palette_fade_count == PAL_FADE_DONE) {
        palette_fade_active = false;
    }
    else {
        // Brute version, fade all colors instead of flagged sub-palettes // TODO: operate on sub-palettes of 16 instead?
        uint16_t * p_color_cached = full_palette_cached;
        uint16_t * p_color_target = full_palette_target;
        for (uint16_t color = 0; color < COLS_PER_PAL_8BPP; color++) {

            uint16_t color_cached = *p_color_cached;
            uint16_t color_target = *p_color_target;

            if      ((color_cached & PAL_RED_MASK)   > (color_target & PAL_RED_MASK))   color_cached -= PAL_RED_LSBIT;
            else if ((color_cached & PAL_RED_MASK)   < (color_target & PAL_RED_MASK))   color_cached += PAL_RED_LSBIT;

            if      ((color_cached & PAL_GREEN_MASK) > (color_target & PAL_GREEN_MASK)) color_cached -= PAL_GREEN_LSBIT;
            else if ((color_cached & PAL_GREEN_MASK) < (color_target & PAL_GREEN_MASK)) color_cached += PAL_GREEN_LSBIT;

            if      ((color_cached & PAL_BLUE_MASK)  > (color_target & PAL_BLUE_MASK))  color_cached -= PAL_BLUE_LSBIT;
            else if ((color_cached & PAL_BLUE_MASK)  < (color_target & PAL_BLUE_MASK))  color_cached += PAL_BLUE_LSBIT;

            *p_color_cached++ = color_cached;
            p_color_target++;
        }        

        palette_fade_count--;
    }
}


// Calculate the next palette during a fade to black
// Does NOT modify target palette so that it can be
// faded in/out while retaining it's values
static void palette_fade_to_black_calc_update(void) {    

    if (palette_fade_count == PAL_FADE_DONE) {
        palette_fade_active = false;
    }
    else {
        
        // Brute version, fade all colors instead of flagged sub-palettes // TODO: operate on sub-palettes of 16 instead?
        uint16_t * p_color_cached = full_palette_cached;
        for (uint16_t color = 0; color < COLS_PER_PAL_8BPP; color++) {

            uint16_t color_cached = *p_color_cached;

            if (color_cached & PAL_RED_MASK)   color_cached -= PAL_RED_LSBIT;
            if (color_cached & PAL_GREEN_MASK) color_cached -= PAL_GREEN_LSBIT;
            if (color_cached & PAL_BLUE_MASK)  color_cached -= PAL_BLUE_LSBIT;
            *p_color_cached++ = color_cached;
        }

        palette_fade_count--;
    }
}
*/
