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

// options.c


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"

#include "options.h"
#include "options_screen.h"

// TODO: rename "option_game_" -> "game_setting" , rename "options.c/h" -> "game_settings.c/h"

// TODO: For now these aren't in an array/struct to reduce cost of access. Re-evaluate later
INT8 option_game_type          = OPTION_GAME_TYPE_DEFAULT;
INT8 option_game_difficulty    = OPTION_DIFFICULTY_DEFAULT;
INT8 option_game_visual_hints  = OPTION_VISUAL_HINTS_DEFAULT;
INT8 option_game_high_contrast = OPTION_HIGH_CONTRAST_DEFAULT;
INT8 option_game_music         = OPTION_MUSIC_DEFAULT;
INT8 option_game_preview_next  = NEXT_PREV_DEFAULT;
INT8 option_game_link2p        = OPTION_LINK2P_DEFAULT;

// See spreadsheet for more details
// Speed increase per level bracket roughly doubles
const UINT8 frames_per_drop_LUT[]          = {60,45,30,20,15,10,8,7,6,5,4,4,3};
const UINT8 levels_per_fpd_decrement_LUT[] = { 1, 1, 1, 2, 2, 5,0,0,0,0,0,0,0};

const char * options_difficulty_abrv[]  = {"EZ ", "NM", "HD", "XP", "BT"}; // Must match : option_difficulty_entries

// Presets per difficulty level
const settings_data settings_LUT[] = {
    {SPD_ST_EASY,   NEXT_PREV_ON,  BONUS_EASY,   BOMB_THR_EASY,   MERGE_THR_EASY},
    {SPD_ST_NORMAL, NEXT_PREV_ON,  BONUS_NORMAL, BOMB_THR_NORMAL, MERGE_THR_NORMAL},
    {SPD_ST_HARD,   NEXT_PREV_ON,  BONUS_HARD,   BOMB_THR_HARD,   MERGE_THR_HARD},
    {SPD_ST_EXPERT, NEXT_PREV_ON,  BONUS_EXPERT, BOMB_THR_EXPERT, MERGE_THR_EXPERT},
    {SPD_ST_BEAST,  NEXT_PREV_OFF, BONUS_BEAST,  BOMB_THR_BEAST,  MERGE_THR_BEAST}
};

const settings_data * p_game_settings;


const char * options_difficulty_abbrev_text_get(void) {
    return options_difficulty_abrv[option_game_difficulty];
}


// TODO: ?? use accessor instead of directly accessing option_game_preview_next?
//UINT8 options_preview_next_enabled_get(UINT8 level) {
//     option_game_preview_next      = settings_LUT[ option_game_difficulty ].preview_next_enabled;
//}


// TODO: NOTE: level must *not* be zero <-- no longer true?
UINT8 options_frames_per_drop_get(UINT8 level) {

    UINT8 index;
    UINT8 sub_decrement = 0;

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

            // TODO: OPTIMIZE: there is probably a more efficient way of doing this
            // Increase in drop speed between tiers is based on
            // levels elapsed since tier base divided by a scaling factor
            sub_decrement = (level % FPD_LUT_LEVELS_PER_TIER) / levels_per_fpd_decrement_LUT[index];
        }
    }

    // Return new frames-per-drop,
    // factoring in per level sub-decrement
    return (frames_per_drop_LUT[ index ] - sub_decrement);
 }



void options_player_settings_apply(void) {

    p_game_settings = &settings_LUT[ option_game_difficulty ];

    option_game_preview_next = p_game_settings->preview_next_enabled;
}