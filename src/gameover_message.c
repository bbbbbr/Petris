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

#include <gbdk/platform.h>


#include "common.h"

#include "game_board.h"
#include "game_board_gfx.h"
#include "gfx.h"
#include "gfx_print.h"
#include "player_gfx.h"

#include "gameover_message.h"

#define CHR2SPR(chr) (chr) // TODO FONT SPRITES : ((uint8_t)(chr -'A') + TILES_FONT_CHARS_START)

const uint8_t spr_gameover_chars[] = "GAME OVER";
const uint8_t spr_you_lost_chars[] = "YOU LOST ";
const uint8_t spr_you_won_chars[]  = " YOU WON ";

#define SPR_PAL_PRINT BG_PAL_5

#define SPR_GAMEOVER_MAX_Y ((BRD_ST_Y + 8U) * 8U)
#define SPR_GAMEOVER_START_X (((BRD_ST_X) * 8u) + 4u) // 2 Pixels right of left board edge

#define Y_PLAT_ADJ(Y) (((Y) * 1.5))

// Pre-calculated gravity drop bounce LUT
// For calculation ref commented out version of gameover_message_animate() below
const uint8_t SPR_GAMEOVER_LUT_Y[] = {
    224u, Y_PLAT_ADJ(0x03u), Y_PLAT_ADJ(0x06u), Y_PLAT_ADJ(0x0Au), Y_PLAT_ADJ(0x0Fu), Y_PLAT_ADJ(0x15u), Y_PLAT_ADJ(0x1Cu), Y_PLAT_ADJ(0x24u),
    Y_PLAT_ADJ(0x2Du), Y_PLAT_ADJ(0x37u), Y_PLAT_ADJ(0x38u), Y_PLAT_ADJ(0x31u), Y_PLAT_ADJ(0x2Bu), Y_PLAT_ADJ(0x26u), Y_PLAT_ADJ(0x22u), Y_PLAT_ADJ(0x1Fu),
    Y_PLAT_ADJ(0x1Du), Y_PLAT_ADJ(0x1Cu), Y_PLAT_ADJ(0x1Cu), Y_PLAT_ADJ(0x1Du), Y_PLAT_ADJ(0x1Fu), Y_PLAT_ADJ(0x22u), Y_PLAT_ADJ(0x26u), Y_PLAT_ADJ(0x2Bu),
    Y_PLAT_ADJ(0x31u), Y_PLAT_ADJ(0x38u), Y_PLAT_ADJ(0x40u), Y_PLAT_ADJ(0x3Bu), Y_PLAT_ADJ(0x37u), Y_PLAT_ADJ(0x34u), Y_PLAT_ADJ(0x32u), Y_PLAT_ADJ(0x31u),
    Y_PLAT_ADJ(0x31u), Y_PLAT_ADJ(0x32u), Y_PLAT_ADJ(0x34u), Y_PLAT_ADJ(0x37u), Y_PLAT_ADJ(0x3Bu), Y_PLAT_ADJ(0x40u), Y_PLAT_ADJ(0x3Eu), Y_PLAT_ADJ(0x3Du),
    Y_PLAT_ADJ(0x3Du), Y_PLAT_ADJ(0x3Eu), Y_PLAT_ADJ(0x40u), Y_PLAT_ADJ(0x3Fu), Y_PLAT_ADJ(0x3Fu), Y_PLAT_ADJ(0x40u), Y_PLAT_ADJ(0x40u)};

#define SPR_GAMEOVER_LUT_Y_MAX (ARRAY_LEN(SPR_GAMEOVER_LUT_Y) - 1)

#define GAMEOVER_UPDATE_MASK 0x03 // Only update cursor once every 8 frames
#define SPR_GAMEOVER_GRAVITY 1
#define SPR_GAMEOVER_LANDED  127

uint8_t spr_gameover_y_idx[SPR_GAMEOVER_COUNT];
const uint8_t * p_gameover_chars = NULL;


// Drop "G A M E   O V E R" letters with a bounce, starting from left to right
// Expects font data to already be loaded (see PRINT/etc)
void gameover_message_animate(void) {

    uint8_t c;
    uint8_t min_spr = 0; // Used to slowly exit the loop as pieces land
    uint8_t max_spr = 0; // Used for delay launch left to right

    // Relies on existing loaded font tiles and palettes from board_gfx_init...()

    PRINT_POS(SPR_GAMEOVER_START_X, SPR_GAMEOVER_LUT_Y[ spr_gameover_y_idx[c] ]);
    print_to_sprites(min_spr, REL_PAL_FONT_8x16_YELLOW_OAM_0, p_gameover_chars);

    // Loop exits when all sprites have landed
    while (min_spr != SPR_GAMEOVER_COUNT) {

        // Add some delay
        vsync();

        // Periodic Update
        if ((sys_time & GAMEOVER_UPDATE_MASK) == GAMEOVER_UPDATE_MASK) {

            // Delay launch from left to right
            // by adding one more sprite during each animation pass
            if (max_spr < SPR_GAMEOVER_COUNT)
                max_spr++;

            for (uint8_t spr_id = min_spr; spr_id < max_spr; spr_id++) {

                // Move sprite to next Y LUT position
                spr_gameover_y_idx[spr_id]++;

                // If it's at the end of the LUT then it's landing
                // is complete. Increment the starting sprite past it
                // so that on the next iteration it's excluded
                if (spr_gameover_y_idx[spr_id] >= SPR_GAMEOVER_LUT_Y_MAX) {
                    min_spr++;
                }

                // Write directly to shadow oam
                shadow_OAM[(spr_id * FONT_8x16_TILE_HEIGHT)     ].y = SPR_GAMEOVER_LUT_Y[ spr_gameover_y_idx[spr_id] ];
                shadow_OAM[(spr_id * FONT_8x16_TILE_HEIGHT) + 1u].y = SPR_GAMEOVER_LUT_Y[ spr_gameover_y_idx[spr_id] ] + 8u;
            }
        }
    } // while (min_spr != SPR_GAMEOVER_COUNT)
}

// Hides gameover message sprites
// Expects font data to already be loaded (see PRINT/etc)
void gameover_message_reset(void) {

    uint8_t c;

    for (c = 0; c < SPR_GAMEOVER_COUNT; c++) {
        spr_gameover_y_idx[c] = 0; // Reset Y LUT position
        hide_sprite((c * FONT_8x16_TILE_HEIGHT));
        hide_sprite((c * FONT_8x16_TILE_HEIGHT) + 1u);

        // Sprite tiles will get set just before display
        // in order to set the desired message
    }
}

