#include "loopy.h"
#include "loopy_helpers.h"
#include <gbdk/platform.h>

#include "intro_screen_out.h"
#include "intro_screen_out_pal2.h"
#include "intro_screen_out_pal3.h"
#include "intro_screen_out_pal4.h"
#include "intro_screen_out_pal5.h"
#include "intro_screen_out_pal6.h"
#include "intro_screen_out_pal7.h"
#include "intro_screen_out_pal8.h"
#include "intro_screen_out_pal9.h"




#include "gfx_board_fadepals.h"

const palette_color_t fade_font_8x16_outline_cols[FADE_SKY_PALS_COUNT] = {
    // Default blue sky
    RGB888(114,187,203),
    // Pale blue green sky
    RGB888(125,184,159),
    // Yellowy Orangey Sky
    RGB888(184,150,103),
    // Peachish Sky
    RGB888(184,132,103),
    // Pinkish Sky
    RGB888(184,106,145),
    // Grey Blue sky night time
    RGB888( 75,108,156), // RGB888( 77,120,184),
    // Deeper blue sky night time
    RGB888( 53, 90,184),
    // Rising Purple sky
    RGB888(110, 89,179), // RGB888(115, 75,184),
    // Morning light lavender sky    
    RGB888( 112,120,196), // RGB888( 99,110,184),
};

const palette_color_t * fade_sky_pals[FADE_SKY_PALS_COUNT] = {
    // Default blue sky
    intro_screen_out_palettes,

    // Pale blue green sky
    intro_screen_out_pal2_palettes,

    // Yellowy Orangey Sky
    intro_screen_out_pal3_palettes,

    // Peachish Sky
    intro_screen_out_pal4_palettes,

    // Pinkish Sky
    intro_screen_out_pal5_palettes,

    // Grey Blue sky night time
    intro_screen_out_pal6_palettes,

    // Deeper blue sky night time
    intro_screen_out_pal7_palettes,

    // Rising Purple sky
    intro_screen_out_pal8_palettes,

    // Morning light lavender sky
    intro_screen_out_pal9_palettes,
};
