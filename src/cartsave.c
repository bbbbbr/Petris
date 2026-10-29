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
#include <stdint.h>

#include "options.h"
#include "highscores.h"

// Note: SRAM on the loopy has an 8 bit bus width
#define SRAM_BASE       (0x02000000u)
#define SRAM_OPTIONS    (SRAM_BASE + 0u)
#define SRAM_HIGHSCORES (SRAM_OPTIONS + sizeof(options_rec))

options_rec    * const p_sram_game_options = (options_rec *)SRAM_OPTIONS;
highscores_rec * const p_sram_highscores   = (highscores_rec *)SRAM_HIGHSCORES;


// Basic checksum on all bytes of the SRAM record except the checksum byte itself
// * p_save_data should be a ** uint8_t pointer **
// * Length should EXCLUDE the checksum, and it should be at the end of the record
static uint16_t cartsave_calc_checksum(uint8_t * p_save_data, size_t length) {

    uint16_t checksum = 0u;

    // Calculate checksum for save, skip past checksum (first byte)
    for (size_t c = 0; c < length; c++) {
        checksum += *p_save_data++;
    }

    return checksum;
}


void cartsave_load_options(void) {

    // Calc checksum on SRAM data, excluding the checksum itself
    // Then load if it matches, or reset options if it doesn't
    uint16_t calc_checksum = cartsave_calc_checksum((uint8_t *)p_sram_game_options, sizeof(game_options) - sizeof(game_options.checksum));

    if (p_sram_game_options->checksum == calc_checksum) {
        game_options = *p_sram_game_options;
        // memcpy((void *)&game_options, (void *)&sram_game_options, sizeof(game_options));
    } else {
        options_reset_to_defaults();
    }
}


void cartsave_save_options(void) {

    // Update checksum for options (excluding checksum itself), then write it to SRAM
    game_options.checksum = cartsave_calc_checksum((uint8_t *)&game_options, sizeof(game_options) - sizeof(game_options.checksum));

    *p_sram_game_options = game_options;
    // memcpy((void *)&sram_options, (void *)&sram_game_options, sizeof(game_options));
}



void cartsave_load_highscores(void) {

    // Calc checksum on SRAM data, excluding the checksum itself
    // Then load if it matches, or reset high-scores if it doesn't
    uint16_t calc_checksum = cartsave_calc_checksum((uint8_t *)p_sram_highscores, sizeof(highscores) - sizeof(highscores.checksum));

    if (p_sram_highscores->checksum == calc_checksum) {
        highscores = *p_sram_highscores;
        // memcpy((void *)&highscores_rec, (void *)&sram_highscores, sizeof(highscores));
    } else {
        highscores_reset_all();
    }
}


void cartsave_save_highscores(void) {

    // Update checksum for high-scores (excluding checksum itself), then write it to SRAM
    highscores.checksum = cartsave_calc_checksum((uint8_t *)&highscores, sizeof(highscores) - sizeof(highscores.checksum));

    *p_sram_highscores = highscores;
    // memcpy((void *)&sram_options, (void *)&sram_highscores, sizeof(highscores));
}


