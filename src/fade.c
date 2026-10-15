#include <gbdk/platform.h>

#include "fade.h"


#define PAL_FADE_DONE  (0u)
#define PAL_FADE_STEPS (31u) // Max value for a 5 bit color channel

// #define DEBUG_SLOW_FADE


bool palette_fade_active = false;

static bool     palette_fade_is_to_black = false;
static uint16_t palette_fade_count       = PAL_FADE_DONE;

static uint16_t full_palette_cached[COLS_PER_PAL_8BPP];
static uint16_t full_palette_target[COLS_PER_PAL_8BPP];

static uint16_t palette_debug_slowdown;

static void palette_fade_calc_update(void);
static void palette_fade_to_black_calc_update(void);


// Palette fading:
// - Call
// - Runs asynchronously in VBlank (write palette) and VBlank-Ended (calc next fade palette) with control flag
// - Can be waited on using palette_fade_wait_completed()
// - Set fade target palettes with set_fade_4bpp_palette()
//
// - Fade in operates on cached full_palette_cached -> full_palette_target


// Sets cached and target palettes to all black
void palettes_init(void) {

    palette_fade_count       = PAL_FADE_DONE;
    palette_fade_active      = false;
    palette_fade_is_to_black = false;

    for (uint16_t color = 0; color < COLS_PER_PAL_8BPP; color++) {
        full_palette_cached[color] = RGB888(0,0,0);
        full_palette_target[color] = RGB888(0,0,0);
    }

    // Register the ISR handlers
    add_vbl(palette_fade_vlb_handler);
    add_vbl_ended(palette_fade_vlb_ended_handler);
}


// Waits for an active palette fade to complete
void palette_fade_wait_completed(void) {

    while(palette_fade_active) {
        vsync();
    }
}


// Starts a fadeout to black
void palette_fade_to_black_start(void) {
    
    // Critical section since the control vars may be modified by interrupts
    volatile uint32_t stauts_register_saved = disable_interrupts();

        palette_fade_count       = PAL_FADE_STEPS;
        palette_fade_active      = true;
        palette_fade_is_to_black = true;
        palette_debug_slowdown = 0;

    enable_interrupts(stauts_register_saved);
}


// Starts a fade toward full_palette_target[]
void palette_fade_to_target_start(void) {

    // Critical section since the control vars may be modified by interrupts
    volatile uint32_t stauts_register_saved = disable_interrupts();

        palette_fade_count       = PAL_FADE_STEPS;
        palette_fade_active      = true;
        palette_fade_is_to_black = false;

    enable_interrupts(stauts_register_saved);
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


// Called from simulated end of VBlank interrupt
void palette_fade_vlb_ended_handler(void) {

    // TODO: DEBUG TEST: SLOW DAWN FADES A LOT
    #ifdef DEBUG_SLOW_FADE
        palette_debug_slowdown++;
        if ((palette_debug_slowdown & 0x000Fu) != 0u) return;
    #endif

    if (palette_fade_active) {
        if (palette_fade_is_to_black) palette_fade_to_black_calc_update(); 
        else                          palette_fade_calc_update();
    }
}


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
