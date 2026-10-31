// intro_screen.h

#ifndef INTRO_SCREEN_H
#define INTRO_SCREEN_H

enum INTRO_MENU_ITEMS {
    INTRO_MENU_ID_MIN = 0,

    INTRO_MENU_ID_PLAY_GAME = 0,
    INTRO_MENU_ID_HIGHSCORES,
    INTRO_MENU_ID_CREDITS,

    INTRO_MENU_ID_COUNT,
    INTRO_MENU_ID_MAX = INTRO_MENU_ID_CREDITS,
    INTRO_MENU_ID_DEFAULT = INTRO_MENU_ID_PLAY_GAME
};


void intro_screen_init(void);
void intro_screen_handle(void);
void intro_screen_cleanup(void);

#endif // INTRO_SCREEN_H