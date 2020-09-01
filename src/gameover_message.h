// game_board_gfx.h

#ifndef GAMEOVER_MESSAGE_H
#define GAMEOVER_MESSAGE_H


extern const UINT8 SPR_GAMEOVER_CHARS[];
extern const UINT8 SPR_YOU_LOST_CHARS[];
extern const UINT8 SPR_YOU_WON_CHARS[];

extern const UINT8 * p_gameover_chars;

#define GAMEOVER_MESSAGE_SET(p_msg) (p_gameover_chars = &p_msg[0])
void gameover_message_animate(void);
void gameover_message_reset(void);

#endif // GAMEOVER_MESSAGE_H


