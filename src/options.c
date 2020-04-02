// options.c


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "options.h"
#include "options_screen.h"

// TODO: For now these aren't in an array/struct to reduce cost of access. Re-evaluate later
INT8 option_game_type = OPTION_GAME_TYPE_DEFAULT;
INT8 option_game_difficulty = OPTION_DIFFICULTY_DEFAULT;
INT8 option_game_visual_hints = OPTION_VISUAL_HINTS_DEFAULT;
