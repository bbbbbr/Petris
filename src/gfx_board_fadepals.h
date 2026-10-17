// Copyright 2026 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

#ifndef _GFX_BOARDPALS_H
#define _GFX_BOARDPALS_H

#include "fade2pal.h"

#define FADE_SKY_PALS_RESET 1 // Skip past default on first pass
#define FADE_SKY_PALS_COUNT 9
#define FADE_SKY_PALS_MIN   0
#define FADE_SKY_PALS_MAX   (FADE_SKY_PALS_COUNT - 1)

extern const palette_color_t fade_font_8x16_outline_cols[FADE_SKY_PALS_COUNT];
extern const palette_color_t * fade_sky_pals[FADE_SKY_PALS_COUNT];


#endif