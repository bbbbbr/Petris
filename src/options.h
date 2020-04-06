// options.h

#ifndef OPTIONS_H
#define OPTIONS_H

#define OPTION_SETTING_MIN 0

// Speed breakdown by difficulty level

// FpD:   Frames between Drop
// Inc:   Decrease FpD once every Inc Levels
// Msec:  Msec between Drop
// Diff:  Subjective Difficulty
// % Max: Speed % of max
// EZ:    Easy
// NM:    Normal
// HD:    Hard
// EX:    Expert
// BT:    Beast

// FpD Inc Msec    Diff        % Max   EZ  NM  HD  EX  BT
// 60  1   1000    Slow/Easy   5       1
// 45  1   750     Med Easy    7       10  1
// 30  1   500     Med         10      20  10
// 20  2   333     Med Harder  15      30  20
// 15  2   250     Hard Easy   20      40  30  1
// 10  5   167     Hardish     30          40  10
// 8   5   133     Hard        38              10  1
// 6   5   100     Hard Hard   50              20  10  1
// 5   5   83      Hard Hard   60              40  20  10
// 4   0   67      V HARD      75                  30  20
// 4   0   67      V HARD      75                  40  30
// 3   0   50      V-V HARD    100                     40
// 3   0   50      V-V HARD    100

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
// See the table above
#define SPD_ST_EASY   0
#define SPD_ST_NORMAL 1
#define SPD_ST_HARD   4
#define SPD_ST_EXPERT 6
#define SPD_ST_BEAST  7

#define BONUS_EASY   1
#define BONUS_NORMAL 1
#define BONUS_HARD   2
#define BONUS_EXPERT 5
#define BONUS_BEAST  10

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


