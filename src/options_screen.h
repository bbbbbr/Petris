// options_screen.h

#ifndef OPTIONS_SCREEN_H
#define OPTIONS_SCREEN_H

#include "options.h"

// Trailing spaces are to clear out previous option value text
extern const char * options_type[OPTION_GAME_TYPE_COUNT];
extern const char * options_difficulty[OPTION_DIFFICULTY_COUNT];

void options_screen_init(void);
void options_screen_handle(void);

#endif // OPTIONS_SCREEN_H