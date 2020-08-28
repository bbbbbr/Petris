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
#include "gfx_board_fadepals.h"

// Scratch palette array in fade.c
// Reads from this to extract current palette
// Writes to it to apply palette updates
extern UWORD ModPalBg[];


UINT8 fade_sky_pal_idx;


// Use Bresenham's algorithm to Linear Interpolate (lerp)
// between old->new R/G/B over N Frames
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
// X Axis: Frames, Frame duration should always be longer than 32
// Y Axis: Color Delta, Max abs distance between FROM and TO R/G/B is never > 32 (max cval per channel)
//
// distance x (dx) = FADE2PAL_FRAME_COUNT
// distance y (dy) = color_from - color_to

INT8 fade2pal_frame_count;

// Arrays
UINT8 fade2pal_color_val[FADE2PAL_ARY_SIZE];
INT8  fade2pal_color_val_dir[FADE2PAL_ARY_SIZE];
INT8  fade2pal_err_accum[FADE2PAL_ARY_SIZE];
INT8  fade2pal_err_framestep_add[FADE2PAL_ARY_SIZE];


void fade2pal_init(void) {

    fade_sky_pal_idx = FADE_SKY_PALS_RESET;
    fade2pal_frame_count = 0;
}


void fade2pal_start_next(void) {

    // Calculate fade to the currently queued palette
    fade2pal_prep_fade(&fade_sky_pals[fade_sky_pal_idx][0]);

    // Loop through transitions until done
    while (fade2pal_frame_count) {

        wait_vbl_done();

        // Optional: fade can be stretched over more frames
        // if (sys_time & 0x01)
            fade2pal_update_entry();
    }

    // Queue up next palette
    if (fade_sky_pal_idx < FADE_SKY_PALS_MAX) {
        fade_sky_pal_idx++;
    }
    else {
        // Wrap around to first (default) palette
        // Use MIN, not RESET which skips initial default palette
        fade_sky_pal_idx = FADE_SKY_PALS_MIN;
    }

}


// Color Palette entry bit packing
// UINT16
// Component: x BBBBB GGGGG RRRRR
// Bits used: F EDCBA 98765 43210

// NOTE: Background Palettes ONLY
// NOTE: Assumes palettes (FADE2PAL_NUM_PALS) are contiguous
void fade2pal_prep_fade(UINT8 * p_dest_pal_gbr) {

    // Extract RGB from current palettes
    UINT16 * p_src_pal = &ModPalBg[FADE2PAL_PALNUM_START * COLORS_PER_PAL];
    UINT16 t_color = *p_src_pal;
    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;

    for (c = 0; c < FADE2PAL_ARY_SIZE; c++) {


        // Initialze current channel of current palette color
        // Need to decode in order of B -> G -> R
        // Note: function could be inlined here
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



// This can also be called from a vlank/etc ISR
void fade2pal_update_entry(void) {

    UINT8  pal_inc = PAL_INC_RESET;
    UINT8  c;
    UINT16 * p_src_pal = &ModPalBg[FADE2PAL_PALNUM_START * COLORS_PER_PAL];
    *p_src_pal = 0; // Clear the first palette entry

    // Update all the separated B,G,R color components
    for (c = 0; c < FADE2PAL_ARY_SIZE; c++) {

        // If there is enough accumulated error then update the color component
        if (fade2pal_err_accum[c] >= 0 )
        {
            // Roll back the error accumulator by one Color step
            fade2pal_err_accum[c] -= FADE2PAL_ERR_COLORSTEP_SUB;
            // Update the color component (R,G or B) by it's step dir/amount
            fade2pal_color_val[c] += fade2pal_color_val_dir[c];
        }
        // Increment error accumulator by one Frame step
        fade2pal_err_accum[c] += fade2pal_err_framestep_add[c];

        // Merge color component into BGRu16 palette entry
        *p_src_pal |= (UINT16)fade2pal_color_val[c];

        // Upshift color component if needed
        // or move to next palette entry every third pass
        if (pal_inc) {
            (*p_src_pal) <<= 5;
            pal_inc--;
        } else {
            // Move to next palette entry
            p_src_pal++;
            // Only clear if this is NOT the last pass
            // (i.e. don't write beyond palette array bounds)
            if (c != (FADE2PAL_ARY_SIZE - 1))
                *p_src_pal = 0;
            // Reset color component counter
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


