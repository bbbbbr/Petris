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

// options.c


#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

#include "common.h"

#include "options.h"
#include "options_screen.h"


options_rec game_options;


void options_reset_to_defaults(void) {

    game_options.type          = OPTION_GAME_TYPE_DEFAULT;
    game_options.difficulty    = OPTION_DIFFICULTY_DEFAULT;
    game_options.visual_hints  = OPTION_VISUAL_HINTS_DEFAULT;
    game_options.high_contrast = OPTION_HIGH_CONTRAST_DEFAULT;
    game_options.music         = OPTION_MUSIC_DEFAULT;
    game_options.preview_next  = NEXT_PREV_DEFAULT;
    game_options.link2p        = OPTION_LINK2P_DEFAULT;
    game_options.hard_drop     = OPTION_HARD_DROP_DEFAULT;
    game_options.filter_diagonals = OPTION_FILTER_DIAGONALS_DEFAULT;
}


// See spreadsheet for more details
// Speed increase per level bracket roughly doubles
const uint8_t frames_per_drop_LUT[]          = {60,45,30,20,15,10,8,7,6,5,4,4,3};
const uint8_t levels_per_fpd_decrement_LUT[] = { 1, 1, 1, 2, 2, 5,0,0,0,0,0,0,0};

const char * options_difficulty_abrv[]  = {"EZ ", "NM", "HD", "XP", "BT"}; // Must match : option_difficulty_entries

// Presets per difficulty level
const settings_data settings_LUT[] = {
    {SPD_ST_EASY,   NEXT_PREV_ON,  BONUS_EASY,   BOMB_THR_EASY,   MERGE_THR_EASY},
    {SPD_ST_NORMAL, NEXT_PREV_ON,  BONUS_NORMAL, BOMB_THR_NORMAL, MERGE_THR_NORMAL},
    {SPD_ST_HARD,   NEXT_PREV_ON,  BONUS_HARD,   BOMB_THR_HARD,   MERGE_THR_HARD},
    {SPD_ST_EXPERT, NEXT_PREV_ON,  BONUS_EXPERT, BOMB_THR_EXPERT, MERGE_THR_EXPERT},
    {SPD_ST_BEAST,  NEXT_PREV_OFF, BONUS_BEAST,  BOMB_THR_BEAST,  MERGE_THR_BEAST}
};

const settings_data * p_game_settings = NULL;


const char * options_difficulty_abbrev_text_get(void) {
    return options_difficulty_abrv[game_options.difficulty];
}


// TODO: NOTE: level must *not* be zero <-- no longer true?
uint8_t options_frames_per_drop_get(uint8_t level) {

    uint8_t index;
    uint8_t sub_decrement = 0;

    // First get level index into frames-per-drop LUT
    index = (level / FPD_LUT_LEVELS_PER_TIER);

    // Make sure Level doesn't exceed max number of tiers
    // (increasing speed per level caps out after a certain range)
    if (index >= FPD_TIERS_MAX) {
        index = FPD_TIERS_MAX;
    }

    // Add offset based on difficulty setting
    index += p_game_settings->LUT_speed_offset;

    // Bounds check level LUT array
    if (index >= ARRAY_LEN(frames_per_drop_LUT)) {
        index = ARRAY_LEN(frames_per_drop_LUT);
    }

    // Only factor in sub-tier per-level decrement if level isn't maxed out
    if (level < FPD_LEVEL_MAX) {

        // Bounds check sub-level LUT array
        if (index < ARRAY_LEN(levels_per_fpd_decrement_LUT)) {

            // Increase in drop speed between tiers is based on
            // levels elapsed since tier base divided by a scaling factor
            // Optional: could optimize this
            sub_decrement = (level % FPD_LUT_LEVELS_PER_TIER) / levels_per_fpd_decrement_LUT[index];
        }
    }

    // Return new frames-per-drop,
    // factoring in per level sub-decrement
    return (frames_per_drop_LUT[ index ] - sub_decrement);
 }



void options_player_settings_apply(void) {

    p_game_settings = &settings_LUT[ game_options.difficulty ];

    game_options.preview_next = p_game_settings->preview_next_enabled;
}
