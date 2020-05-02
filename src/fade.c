#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "fade.h"

//#include "Palette.h"
//#include "Math.h"

//UWORD Fading_BPal[32];
//UWORD Fading_SPal[32];


#define FADE_IN_START 5
#define FADE_OUT_START 0
                        //    x    R    G    B
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

// #define PAL_RED(C)   (((C)      ) & 0x1F)
// #define PAL_GREEN(C) (((C) >>  5) & 0x1F)
// #define PAL_BLUE(C)  (((C) >> 10) & 0x1F)

// //RGB defined in cgb.h has a << 0 that kills the compiler
// #define RGB2(r, g, b) (((UINT16)(r)) | (((UINT16)(g)) <<  5) | ((((UINT16)(b)) << 8) << 2))


// // == WHITE FADE ==
// UWORD GetFadeColor(INT8 i, UWORD col) {
//         UINT16 fadedwhite = (0x001F >> (5 - i));
//         return RGB2(PAL_RED(col)   | fadedwhite,    // DespRight(0x1F, 5 - i),
//                     PAL_GREEN(col) | fadedwhite,
//                     PAL_BLUE(col)  | fadedwhite);
// }

// // == WHITE FADE semi optimized ==
// UWORD GetFadeColor(INT8 i, UWORD col) {
//         UINT16 fadedwhite = (0x001F >> (5 - i));
//         return (col | (fadedwhite | fadedwhite << 5 | fadedwhite << 10));
// }

// // == BLACK FADE ==
// UWORD GetFadeColor(INT8 i, UWORD col) {
//         return RGB2(PAL_RED(col)  >> i,
//                     PAL_GREEN(col) >> i,
//                     PAL_BLUE(col)  >> i);
// }


void SetFadePalBkg(UINT8 start_pal, UINT8 num_pal, UWORD * p_SrcPal, UINT8 which_pal) {
    int c;

    for (c = start_pal; c < (start_pal + num_pal); c++) {

        if (which_pal == FADE_PAL_BKG)
            p_FadePalBg[c] = p_SrcPal;
        else
            p_FadePalSprite[c] = p_SrcPal;

        p_SrcPal += NUM_COLORS_PER_PAL;
    }
}


void FadeColor(INT8 fade_dir) {

    INT8 i;
    UINT8 pal, c;
    UWORD palette_bg[4]; // Background Pal
    UWORD palette_sp[4]; // Sprite Pal
    const UWORD * p_bg;
    const UWORD * p_sp;

    i = (fade_dir == FADE_OUT) ? FADE_OUT_START : FADE_IN_START; // Set loop start (5 for fade in, 0 for fade out)

    while ((i != -1) && (i != 6)) {

        for(pal = 0; pal < 8; pal++) {
            // Load pointer to palette from cached values
            p_bg = p_FadePalBg[pal];
            p_sp = p_FadePalSprite[pal];

            // Read colors from palette and get faded version
            for(c = 0; c < 4; ++c) {
                    //palette_bg[c] = GetFadeColor(i, *p_bg);
                    //palette_sp[c] = GetFadeColor(i, *p_sp);
                    palette_bg[c] = (*p_bg | FADE_LUT[i]);
                    palette_sp[c] = (*p_sp | FADE_LUT[i]);
                    p_bg++;
                    p_sp++;
            };
            // Apply current step of faded palette colors
            set_bkg_palette(pal, 1, palette_bg);
            set_sprite_palette(pal, 1, palette_sp);
        }
        // delay(5);
        // delay(10); //  <-- feels a little fast (WHITE)
        // delay(20);
        // delay(80); //  <-- looks ok, but flickers (BLACK)
        // delay(1000);
        i += fade_dir;
    }
}

/*
void FadeInColor() {
    UINT8 i;
    for(i = 0; i != 6; i ++) {
        FadeStepColor(i);
    }
}


void FadeOutColor() {
    UINT8 i;
    for(i = 5; i != 0xFF; -- i) {
        FadeStepColor(i);
    }
}
*/