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

#include <gb/gb.h>
#include <gb/cgb.h>

#include "fade2pal.h"

// Scratch palette array in fade.c
// Reads from this to extract current palette
// Writes to it to apply palette updates
extern UWORD ModPalBg[];


UINT8 fade_sky_pal_idx = 0;


// Use Bresenham's algorithm to Linear Interpolate (lerp)
// between old->new R/G/B over N Frames
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
// X Axis: Frames, Frame duration should always be longer than 32
// Y Axis: Color Delta, Max abs distance between FROM and TO R/G/B is never > 32 (max cval per channel)
//
// distance x (dx) = FADE2PAL_FRAME_COUNT
// distance y (dy) = color_from - color_to


INT8 fade2pal_frame_count = 0;

// Arrays
UINT8 fade2pal_color_val[FADE2PAL_ARY_SIZE];
INT8  fade2pal_color_val_dir[FADE2PAL_ARY_SIZE];
INT8  fade2pal_err_accum[FADE2PAL_ARY_SIZE];
INT8  fade2pal_err_framestep_add[FADE2PAL_ARY_SIZE];

// Color Palette entry bit packing
// UINT16
// Component: x BBBBB GGGGG RRRRR
// Bits used: F EDCBA 98765 43210

// NOTE: Background Palettes ONLY
// NOTE: Assumes palettes (FADE2PAL_NUM_PALS) are contiguous
void fade2pal_init(UINT8 * p_dest_pal_gbr) {

    // Extract RGB from current palettes
    UINT16 * p_src_pal = &ModPalBg[FADE2PAL_PALNUM_START * COLORS_PER_PAL];
    UINT16 t_color = *p_src_pal;
    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;

    for (c = 0; c < FADE2PAL_ARY_SIZE; c++) {

// TODO: inline function code here?
        // Initialze current channel of current palette color
        // Need to decode in order of B -> G -> R
        fade2pal_calc_entry( (UINT8)(t_color >> (5 * pal_inc)) & 0x1F, // color FROM
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
    fade2pal_frame_count = FADE2PAL_FRAME_COUNT;

}



void fade2pal_calc_entry(UINT8 color_from, UINT8 color_to, UINT8 c) {

    // Extract colors from current color palette here

    // Set starting value and Y axis direction
    // Set X axis error step using Y axis color distance x 2
    fade2pal_color_val[c] = color_from;
    fade2pal_color_val_dir[c] = -1;
    fade2pal_err_framestep_add[c] = (color_from - color_to) << 1;

    // Is Y Distance/Step Negative? Invert direction
    if( fade2pal_err_framestep_add[c] <= 0 ) {
        fade2pal_err_framestep_add[c] *= -1;
        fade2pal_color_val_dir[c] *= -1;
    }

    // X Distance (frame count) is always larger than Y Distance (color delta)
    // so the controlling increment is always on the X axis (frame count)
    // Load initial frame error since the first step (current pal FROM) is already loaded
    fade2pal_err_accum[c] = fade2pal_err_framestep_add[c] - FADE2PAL_FRAME_COUNT;
}



void fade2pal_update_entry(void) {

    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;
    UINT16 * p_src_pal = &ModPalBg[FADE2PAL_PALNUM_START * COLORS_PER_PAL];
    *p_src_pal = 0; // Prepare for first color entry

    // Update all colors within this loop
    for (c = 0; c < FADE2PAL_ARY_SIZE; c++) {
        if (fade2pal_err_accum[c] >= 0 )
        {
            // Update color
            fade2pal_err_accum[c] -= FADE2PAL_ERR_COLORSTEP_SUB;
            fade2pal_color_val[c] += fade2pal_color_val_dir[c];
        }
        fade2pal_err_accum[c] += fade2pal_err_framestep_add[c];

        // Update colors
        *p_src_pal |= (UINT16)fade2pal_color_val[c];

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
    fade2pal_frame_count--;

    // Update Color Palettes
    // To avoid flickering:
    // Wait until VBlank (mode == 1) and then write palette in one big chunk
    // Doing the opposite and gating with wait_vbl_done() yields mild top-of-screen flicker
    while((STAT_REG & 0x03) != 0x01);
    set_bkg_palette(FADE2PAL_PALNUM_START,
                    FADE2PAL_NUM_PALS,
                    &ModPalBg[FADE2PAL_PALNUM_START * COLORS_PER_PAL]);

}


