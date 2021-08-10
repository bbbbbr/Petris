#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "fade.h"


// #define PAL_FADE_WHITE
#define PAL_FADE_BLACK
// #define PAL_FADE_NONE

#define FADE_IN_START 5
#define FADE_OUT_START 0

#define FADE_EXCLUDE_LSBITS 0b0111101111011110
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

                    if (fade_dir == FADE_OUT) {
                        // If fading out, just use currently cached/loaded palette
                        // This makes it compatible with fade2pal adjustments
                        *p_mod_bg = *p_mod_bg | fade_mask;
                        *p_mod_sp = *p_mod_sp | fade_mask;
                    } else {
                        *p_mod_bg = *p_bg | fade_mask;
                        *p_mod_sp = *p_sp | fade_mask;
                    }

                #else
                  #ifdef PAL_FADE_BLACK

                    if (fade_dir == FADE_OUT) {
                        // If fading out, just use currently cached/loaded palette
                        // This makes it compatible with fade2pal adjustments

                        // Mask lowest bits then downshift all channels by 1
                        *p_mod_bg = (*p_mod_bg & FADE_EXCLUDE_LSBITS) >> 1;
                        *p_mod_sp = (*p_mod_sp & FADE_EXCLUDE_LSBITS) >> 1;

                    } else {
                        // Fade in/out from Black
                        fade_mask = FADE_LUT[5 - i];

                        *p_mod_bg = ((*p_bg) >> i) & fade_mask;
                        *p_mod_sp = ((*p_sp) >> i) & fade_mask;
                    }

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
        while((STAT_REG & (STATF_HBL | STATF_VBL | STATF_OAM | STATF_LCD)) != STATF_VBL);
        set_bkg_palette(0, 8, ModPalBg);

        while((STAT_REG & (STATF_HBL | STATF_VBL | STATF_OAM | STATF_LCD)) != STATF_VBL);
        set_sprite_palette(0, 8, ModPalSprite);

        // Wait a little while before performing the next step
        delay(40);

        i += fade_dir;
    }

}
