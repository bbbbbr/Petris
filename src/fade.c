#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "fade.h"


// #define PAL_FADE_WHITE
#define PAL_FADE_BLACK
// #define PAL_FADE_NONE

#define FADE_IN_START 5
#define FADE_OUT_START 0
                        //   x    R    G    B
const UWORD FADE_LUT[6] = {0b0000000000000000,  // No white present to mix
                           0b0000010000100001,
                           0b0000110001100011,
                           0b0001110011100111,
                           0b0011110111101111,
                           0b0111111111111111}; // Full white present to mix

// Fade palette pointers needs to be initialized to something
// So just point them at the Fade LUT (which will have a gradient of greys)
const UWORD * p_FadePalBg[8]     = {FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT};
const UWORD * p_FadePalSprite[8] = {FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT, FADE_LUT};

#define NUM_COLORS_PER_PAL 4

UWORD ModPalBg[32];
UWORD ModPalSprite[32];




void fade_set_pal(UINT8 start_pal, UINT8 num_pal, UWORD * p_SrcPal, UINT8 which_pal) {
    int c;

    // **DO NOT** Pass palette update through to GBDK pal function
    // Instead let fade_start() perform all palette updates
    //
    // if (which_pal == FADE_PAL_BKG)
    //     set_bkg_palette(start_pal, num_pal, p_SrcPal);
    // else
    //     set_sprite_palette(start_pal, num_pal, p_SrcPal);

    // Copy palette pointers into fade palette array slots
    // for later reference
    for (c = start_pal; c < (start_pal + num_pal); c++) {

        if (which_pal == FADE_PAL_BKG)
            p_FadePalBg[c] = p_SrcPal;
        else
            p_FadePalSprite[c] = p_SrcPal;

        p_SrcPal += NUM_COLORS_PER_PAL;
    }
}


void fade_start(INT8 fade_dir) {

    INT8 i;
    UINT8 pal, c;
    const UWORD * p_bg;
    const UWORD * p_sp;
    UWORD * p_mod_bg;
    UWORD * p_mod_sp;
    UWORD fade_mask;

    i = (fade_dir == FADE_OUT) ? FADE_OUT_START : FADE_IN_START; // Set loop start (5 for fade in, 0 for fade out)

    while ((i != -1) && (i != 6)) {

        p_mod_bg = &ModPalBg[0];
        p_mod_sp = &ModPalSprite[0];

        for(pal = 0; pal < 8; pal++) {
            // Load pointer to palette from cached values
            p_bg = p_FadePalBg[pal];
            p_sp = p_FadePalSprite[pal];

            // Read colors from palette and get faded version
            for(c = 0; c < 4; ++c) {

                #ifdef PAL_FADE_WHITE
                    // Fade in/out from White
                    fade_mask = FADE_LUT[i];
                    *p_mod_bg = *p_bg | fade_mask;
                    *p_mod_sp = *p_sp | fade_mask;

                #else
                  #ifdef PAL_FADE_BLACK
                    // Fade in/out from Black
                    fade_mask = FADE_LUT[5 - i];
                    *p_mod_bg = ((*p_bg) >> i) & fade_mask;
                    *p_mod_sp = ((*p_sp) >> i) & fade_mask;

                    #else
                    // No Fade
                    *p_mod_bg = *p_bg;
                    *p_mod_sp = *p_sp;
                  #endif
                #endif

                // Move to next source palette color
                p_bg++;
                p_sp++;

                // Move to next color in cached Modified palette
                p_mod_bg++;
                p_mod_sp++;
            };
        }
        // Apply current step of faded palette colors

        // To avoid flickering:
        // Wait until VBlank (mode == 1) and then write palette in one big chunk
        // Doing the opposite and gating with wait_vbl_done() yields mild top-of-screen flicker
        while((STAT_REG & 0x03) != 0x01);
        set_bkg_palette(0, 8, ModPalBg);

        while((STAT_REG & 0x03) != 0x01);
        set_sprite_palette(0, 8, ModPalSprite);

        // Wait a little while before performing the next step
        #ifdef CPU_FAST_ENABLED
            delay(70);
        #else
            delay(40);
        #endif

        i += fade_dir;
    }

}



// Use Bresenham's algorithm to Linear Interpolate (lerp)
// between old->new R/G/B over N Frames
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
// X Axis: Frames, Frame duration should always be longer than 32
// Y Axis: Color Delta, Max abs distance between FROM and TO R/G/B is never > 32 (max cval per channel)
//
// distance x (dx) = FADERGB_FRAME_COUNT
// distance y (dy) = color_from - color_to

#define FADERGB_FRAME_COUNT     60                                        // NEVER set less than 32 (32 is max delta per RGB between FROM and TO colors)
                                                                       //       NOR greater than 64 (8 bit signed wraparound error on small color deltas of 1)
#define FADERGB_ERR_COLORSTEP_SUB (FADERGB_FRAME_COUNT * 2)

// For background palettes only
// Fixed number of palettes to operate on, to reduce memory and increase speed
#define FADERGB_PALNUM_START    4
#define FADERGB_NUM_PALS        4
#define COLORS_PER_PAL  4
#define BYTES_PER_COLOR 3
#define FADERGB_PAL_COLOR_COUNT  (FADERGB_NUM_PALS * COLORS_PER_PAL)
#define FADERGB_ARY_SIZE (FADERGB_NUM_PALS * COLORS_PER_PAL * BYTES_PER_COLOR)
#define PAL_INC_RESET 2

INT8 fadergb_frame_count = 0;

// Arrays
UINT8 fadergb_color_val[FADERGB_ARY_SIZE];
INT8  fadergb_color_val_dir[FADERGB_ARY_SIZE];
INT8  fadergb_err_accum[FADERGB_ARY_SIZE];
INT8  fadergb_err_framestep_add[FADERGB_ARY_SIZE];


// #define PAL_RED(C)   (((C)      ) & 0x1F)
// #define PAL_GREEN(C) (((C) >>  5) & 0x1F)
// #define PAL_BLUE(C)  (((C) >> 10) & 0x



//   x BBBBB GGGGG RRRRR
//   F EDCBA 98765 43210

// NOTE: Background Palettes ONLY
// NOTE: Assumes palettes (FADERGB_NUM_PALS) are contiguous
void fadergb_init(UINT8 * p_dest_pal_gbr) {

    // Extract RGB from current palettes
    UINT16 * p_src_pal = &ModPalBg[FADERGB_PALNUM_START * COLORS_PER_PAL];
    UINT16 t_color = *p_src_pal;
    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;

    for (c = 0; c < FADERGB_ARY_SIZE; c++) {

        // TODO: inline function code here?
        // Initialze current channel of current palette color
        // Need to decode in order of B -> G -> R
        fadergb_calc_entry( (UINT8)(t_color >> (5 * pal_inc)) & 0x1F, // color FROM
                            *p_dest_pal_gbr,       // color TO
                            c);                    // color lerp index

        // Load new palette color every 3rd step
        if (pal_inc) {
            // Load next B/G/R channel
            pal_inc--;
        } else {
            // Load new palette color entry
            // and re-load counter
            p_src_pal++;
            t_color = *p_src_pal;
            pal_inc = PAL_INC_RESET;
        }

        // next entry in color array
        p_dest_pal_gbr++;
    }

    // Start frame counter
    fadergb_frame_count = FADERGB_FRAME_COUNT;

}



void fadergb_calc_entry(UINT8 color_from, UINT8 color_to, UINT8 c) {

    // Extract colors from current color palette here
    INT8 color_delta;

    // Get Y axis color distance
    // and set starting value
    color_delta = color_from - color_to;
    fadergb_color_val[c] = color_from;

    // Set up Y axis direction and stepping
    fadergb_err_framestep_add[c] = (color_delta << 1);
    fadergb_color_val_dir[c] = -1;

    // Is Y Distance Negative? Invert direction
    if( color_delta <= 0 ) {
        fadergb_err_framestep_add[c] *= -1;
        fadergb_color_val_dir[c] *= -1;
    }

    // X Distance (frame count) is always larger than Y Distance (color delta)
    // so... Increment on X axis
    fadergb_err_accum[c] = fadergb_err_framestep_add[c] - FADERGB_FRAME_COUNT;
}



void fadergb_update_entry(void) {

    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;
    UINT16 * p_src_pal = &ModPalBg[FADERGB_PALNUM_START * COLORS_PER_PAL];
    *p_src_pal = 0; // Prepare for first color entry

    // Update all colors within this loop
    for (c = 0; c < FADERGB_ARY_SIZE; c++) {
        if (fadergb_err_accum[c] >= 0 )
        {
            // Update color
            fadergb_err_accum[c] -= FADERGB_ERR_COLORSTEP_SUB;
            fadergb_color_val[c] += fadergb_color_val_dir[c];
        }
        fadergb_err_accum[c] += fadergb_err_framestep_add[c];

        // Update colors
        *p_src_pal |= (UINT16)fadergb_color_val[c];

        // Upshift color component if needed
        // or move to next palette entry every third pass
        if (pal_inc) {
            (*p_src_pal) <<= 5;
            pal_inc--;
        } else {
            // Move to next palette entry and clear it
            p_src_pal++;
            *p_src_pal = 0; // BUG: this will alter one after the last modified intended
            pal_inc = PAL_INC_RESET;
        }

    }




    // Decrement number of remaining frame updates
    fadergb_frame_count--;

    // Update Color Palettes
    // To avoid flickering:
    // Wait until VBlank (mode == 1) and then write palette in one big chunk
    // Doing the opposite and gating with wait_vbl_done() yields mild top-of-screen flicker
    while((STAT_REG & 0x03) != 0x01);
    set_bkg_palette(FADERGB_PALNUM_START,
                    FADERGB_NUM_PALS,
                    &ModPalBg[FADERGB_PALNUM_START * COLORS_PER_PAL]);

}


