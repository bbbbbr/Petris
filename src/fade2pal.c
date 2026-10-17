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

#include "loopy.h"
#include "loopy_helpers.h"
#include <gbdk/platform.h>

#include "gfx.h"

#include "fade.h"
#include "fade2pal.h"
#include "gfx_board_fadepals.h"

#include "intro_screen_out.h"

#define PRINT_PAL_FONT_8x16_OUTLINE_COLOR (1u) // Kind of hackish, outline is color [1], to make font outline match level change themed palettes/fades (they look bad without it)

uint16_t fade_sky_pal_idx = FADE_SKY_PALS_MIN;

#define FONT_8x16_BORDER_COLOR_WHITE (RGB888(255u,255u,255u))

// This should be called after 
void fade2pal_init(void) {

    fade_sky_pal_idx = FADE_SKY_PALS_RESET;

    // Font Sprite palettes: Set border to a white outline on startup
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_0, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_1, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_2, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);
}

// All palettes are variations of the same indexed color image
void fade2pal_start_next(void) {

    // Set the outline color for the 8x16 font oam/sprite palettes to try and match the fade palette theme
    // since it looks better than a grey outline and better than keeping the default blue outline

    // Font Sprite palettes: Set border to a white outline
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_0, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_1, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_OBJ_2, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, FONT_8x16_BORDER_COLOR_WHITE);

    // Font BG Tilemap palettes (dedicated border colors to match each sky palette)
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_BG0_0, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, fade_font_8x16_outline_cols[fade_sky_pal_idx]);
    set_fade_target_4bpp_palette_entry(PAL_ASSIGN_BG0_1, PRINT_PAL_FONT_8x16_OUTLINE_COLOR, fade_font_8x16_outline_cols[fade_sky_pal_idx]);

    // Main background palette
    set_fade_target_4bpp_palette(PAL_ASSIGN_BG1_0, intro_screen_out_PALETTE_COUNT, fade_sky_pals[fade_sky_pal_idx]);
    palette_fade_start(FADE_TO_TARGET);

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

