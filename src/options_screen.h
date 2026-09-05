// options_screen.h

#ifndef OPTIONS_SCREEN_H
#define OPTIONS_SCREEN_H



void options_screen_cursor_update(int8_t);
void options_screen_setting_update(int8_t);
void options_screen_setting_draw(int8_t);
void options_screen_draw(void);
void options_screen_exit_cleanup(void);
void options_screen_init(void);
void options_screen_handle(void);
void options_screen_try_gamestart(void);

#endif // OPTIONS_SCREEN_H