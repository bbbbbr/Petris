// options.h

#ifndef OPTIONS_H
#define OPTIONS_H

#define OPTION_SETTING_MIN 0

// Game Type Level
enum {
    OPTION_GAME_TYPE_MARATHON, // = OPTION_GAME_TYPE_MIN,
    OPTION_GAME_TYPE_LEVEL_UP,
    OPTION_GAME_TYPE_GET_THE_PETS,
    OPTION_GAME_TYPE_COLLECT_PETS,

    OPTION_GAME_TYPE_DEFAULT = OPTION_GAME_TYPE_MARATHON
} option_game_type_entries;


// Game Difficulty Level
enum {
    OPTION_DIFFICULTY_EASY, // = OPTION_DIFFICULTY_MIN,
    OPTION_DIFFICULTY_NORMAL,
    OPTION_DIFFICULTY_HARD,
    OPTION_DIFFICULTY_EXPERT,
    OPTION_DIFFICULTY_BEAST,

    OPTION_DIFFICULTY_DEFAULT = OPTION_DIFFICULTY_EASY
} option_difficulty_entries;


// Visual Hinting
enum {
    OPTION_VISUAL_HINTS_ON,// = OPTION_VISUAL_HINTS_MIN,
    OPTION_VISUAL_HINTS_OFF,

    OPTION_VISUAL_HINTS_DEFAULT = OPTION_VISUAL_HINTS_ON
} option_visual_hints_entries;


#define NEXT_PREV_OFF  0
#define NEXT_PREV_ON  1
#define NEXT_PREV_DEFAULT NEXT_PREV_ON

// These are offsets for where to start in frames_per_drop_LUT
// The index for them is option_difficulty_entries
#define SPD_ST_EASY   0
#define SPD_ST_NORMAL 1
#define SPD_ST_HARD   4
#define SPD_ST_EXPERT 6
#define SPD_ST_BEAST  7

#define BONUS_EASY   0
#define BONUS_NORMAL 1
#define BONUS_HARD   2
#define BONUS_EXPERT 3
#define BONUS_BEAST  4

// Number of tiles in a pet required to get a special-bomb
// See: spec_bomb_threshold_pettiles
#define BOMB_THR_EASY   5
#define BOMB_THR_NORMAL 6
#define BOMB_THR_HARD   8
#define BOMB_THR_EXPERT 8
#define BOMB_THR_BEAST  8

// TODO: This should have a randomness factor to make it less predictable
//       I.E.: pieces_until_special_merge = N1 + random(N2)
// Number of pieces issued until delivering a special-merge
// See: spec_merge_threshold_pieces
#define MERGE_THR_EASY   0x001F // once every 32
#define MERGE_THR_NORMAL 0x003F // once every 64
#define MERGE_THR_HARD   0x003F // once every 64
#define MERGE_THR_EXPERT 0x007F // once every 128
#define MERGE_THR_BEAST  0x007F // once every 128


typedef struct settings_rec {
    UINT8  LUT_speed_offset;
    UINT8  preview_next_enabled;
    UINT8  score_bonus;
    UINT8  spec_bomb_threshold_pettiles;
    UINT16 spec_merge_threshold_pieces;
} settings_data;

extern const settings_data * p_game_settings;

extern INT8 option_game_difficulty;
extern INT8 option_game_visual_hints;
extern INT8 option_game_type;
extern INT8 option_game_preview_next;

UINT8 options_frames_per_drop_get(UINT8);
void options_player_settings_apply(void);

#endif // OPTIONS_H


