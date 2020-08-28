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

#ifndef _FADE2PAL_H
#define _FADE2PAL_H

#define FADE2PAL_FRAME_COUNT     60   // This may be best left at 32 since that's max color delta
                                     // If a slower time scale is needed it can be achieved
                                     // by calling fade2pal_update_entry() less often.
                                     // Although, longer time scales may produce more intermediate colors
                                     // * NEVER set less than 32 (32 is max delta per RGB between FROM and TO colors)
                                     // * NOR greater than 64 (8 bit signed wraparound error on small color deltas of 1)
#define FADE2PAL_ERR_COLORSTEP_SUB (FADE2PAL_FRAME_COUNT * 2)

// For background palettes only
// Fixed number of palettes to operate on, to reduce memory and increase speed
#define FADE2PAL_PALNUM_START    4
#define FADE2PAL_NUM_PALS        4
#define COLORS_PER_PAL  4
#define BYTES_PER_COLOR 3
#define FADE2PAL_PAL_COLOR_COUNT  (FADE2PAL_NUM_PALS * COLORS_PER_PAL)
#define FADE2PAL_ARY_SIZE (FADE2PAL_NUM_PALS * COLORS_PER_PAL * BYTES_PER_COLOR)
#define PAL_INC_RESET 2

extern UINT8 fade_sky_pal_idx;

extern INT8 fade2pal_frame_count;

void fade2pal_init(void);
void fade2pal_start_next(void);
void fade2pal_prep_fade(UINT8 * p_dest_pal_gbr);
void fade2pal_calc_entry(UINT8 color_from, UINT8 color_to, UINT8 c);
void fade2pal_update_entry(void);


#endif