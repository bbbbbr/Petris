// options.h

#ifndef OPTIONS_H
#define OPTIONS_H

extern INT8 option_game_difficulty;
extern INT8 option_game_visual_hints;
extern INT8 option_game_type;

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

    OPTION_DIFFICULTY_DEFAULT = OPTION_DIFFICULTY_EASY
} option_difficulty_entries;


// Visual Hinting
enum {
    OPTION_VISUAL_HINTS_ON,// = OPTION_VISUAL_HINTS_MIN,
    OPTION_VISUAL_HINTS_OFF,

    OPTION_VISUAL_HINTS_DEFAULT = OPTION_VISUAL_HINTS_ON
} option_visual_hints_entries;



#endif // OPTIONS_H


