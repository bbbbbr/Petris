// game_board_gfx.h

#ifndef GAMEOVER_MESSAGE_H
#define GAMEOVER_MESSAGE_H


#define CHR2SPR(chr) (chr -'A' + TILES_FONT_CHARS_START)

extern const INT8 SPR_GAMEOVER_CHARS[];
extern const INT8 SPR_YOU_LOST_CHARS[];
extern const INT8 SPR_YOU_WON_CHARS[];

void gameover_message_animate(const INT8 spr_text_chars[]);
void gameover_message_reset(void);

#endif // GAMEOVER_MESSAGE_H


