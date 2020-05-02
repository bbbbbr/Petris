#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "fade.h"


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

#define NUM_COLORS_PER_PAL 4 // (sizeof(UWORD) * 4)


void fade_set_pal(UINT8 start_pal, UINT8 num_pal, UWORD * p_SrcPal, UINT8 which_pal) {
    int c;

    // // Pass palette update through to GBDK pal function
    // if (which_pal == FADE_PAL_BKG)
    //     set_bkg_palette(start_pal, num_pal, p_SrcPal);
    // else
    //     set_sprite_palette(start_pal, num_pal, p_SrcPal);

    // Now copy palette pointers into fade palette array slots
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
    UWORD palette_bg[4]; // Background Pal
    UWORD palette_sp[4]; // Sprite Pal
    const UWORD * p_bg;
    const UWORD * p_sp;

    i = (fade_dir == FADE_OUT) ? FADE_OUT_START : FADE_IN_START; // Set loop start (5 for fade in, 0 for fade out)

    while ((i != -1) && (i != 6)) {

        // TODO: There is some flickering since palette rewrite timing is not locked to Vblank (and seems to take too long to compelte)
        // while((STAT_REG & 0x03) != 0x01);
        for(pal = 0; pal < 8; pal++) {
            // Load pointer to palette from cached values
            p_bg = p_FadePalBg[pal];
            p_sp = p_FadePalSprite[pal];

            // Read colors from palette and get faded version
            for(c = 0; c < 4; ++c) {
                    // Fade in/out from White
                    palette_bg[c] = (*p_bg | FADE_LUT[i]);
                    palette_sp[c] = (*p_sp | FADE_LUT[i]);

                    // // Fade in/out from Black (if enabled, remove: = {FADE_LUT)
                    // palette_bg[c] = ((*p_bg) >> i) & FADE_LUT[5 - i];
                    // palette_sp[c] = ((*p_sp) >> i) & FADE_LUT[5 - i];

                    p_bg++;
                    p_sp++;
            };
            // Apply current step of faded palette colors
            set_bkg_palette(pal, 1, palette_bg);
            set_sprite_palette(pal, 1, palette_sp);
        }
        delay(10);
        i += fade_dir;
    }
}
