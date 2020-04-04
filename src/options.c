// options.c


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"

#include "options.h"
#include "options_screen.h"

// TODO: rename "option_game_" -> "game_setting" , rename "options.c/h" -> "game_settings.c/h"

// TODO: For now these aren't in an array/struct to reduce cost of access. Re-evaluate later
INT8 option_game_type = OPTION_GAME_TYPE_DEFAULT;
INT8 option_game_difficulty = OPTION_DIFFICULTY_DEFAULT;
INT8 option_game_visual_hints = OPTION_VISUAL_HINTS_DEFAULT;

// See spreadsheet for more details
// Speed increase per level bracket roughly doubles
const UINT8 frames_per_drop_LUT[] = {60,45,30,20,15,10,8,6,5,4,4,3,3};

// Presets per difficulty level
const settings_data settings_LUT[] = {
    {SPD_ST_EASY,   NEXT_PREV_ON,  BONUS_EASY,   BOMB_THR_EASY,   MERGE_THR_EASY},
    {SPD_ST_NORMAL, NEXT_PREV_ON,  BONUS_NORMAL, BOMB_THR_NORMAL, MERGE_THR_NORMAL},
    {SPD_ST_HARD,   NEXT_PREV_ON,  BONUS_HARD,   BOMB_THR_HARD,   MERGE_THR_HARD},
    {SPD_ST_EXPERT, NEXT_PREV_ON,  BONUS_EXPERT, BOMB_THR_EXPERT, MERGE_THR_EXPERT},
    {SPD_ST_BEAST,  NEXT_PREV_OFF, BONUS_BEAST,  BOMB_THR_BEAST,  MERGE_THR_BEAST}
};

const settings_data * p_game_settings;



// NOTE: level must *not* be zero
UINT8 options_get_frames_per_drop(UINT8 level) {

    UINT8 index;

    // game level is 1 based, so offset by -1 for array indexing
    index = (level - 1) + p_game_settings->LUT_speed_offset;

    // Bounds check
    if (index >= ARRAY_LEN(frames_per_drop_LUT)) {
        index = ARRAY_LEN(frames_per_drop_LUT);
    }
    // frames_per_drop_LUT[ p_game_settings.LUT_speed_offset ];
    return ( frames_per_drop_LUT[ index ] );
 }



void options_player_settings_apply(void) {

    p_game_settings = &settings_LUT[ option_game_difficulty ];

    // level / 10
    // speed_LUT_offset = level /   player_settings.LUT_speed_offset
    // frames_per_drop_LUT[ p_game_settings.LUT_speed_offset ];


//     option_game_preview_next      = settings_LUT[ option_game_difficulty ].preview_next_enabled;
// //     frames_per_drop               = frames_per_drop_LUT[ settings_LUT[ option_game_difficulty ].LUT_speed_offset ];
//     score_bonus                   = settings_LUT[ option_game_difficulty ].score_bonus;

//     piece_special_threshold_bomb  = settings_LUT[ option_game_difficulty ].spec_bomb_threshold_pettiles;
//     piece_special_threshold_merge = settings_LUT[ option_game_difficulty ].spec_merge_threshold_pieces;
}