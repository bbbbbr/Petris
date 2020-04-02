// options_screen.h

#ifndef OPTIONS_SCREEN_H
#define OPTIONS_SCREEN_H

void options_screen_cursor_update(INT8);
void options_screen_setting_update(INT8);
void options_screen_setting_draw(INT8);
void options_screen_draw(void);
void options_screen_exit_cleanup(void);
void options_screen_init(void);
void options_screen_handle(void);

#endif // OPTIONS_SCREEN_H