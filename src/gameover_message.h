// game_board_gfx.h

#ifndef GAMEOVER_MESSAGE_H
#define GAMEOVER_MESSAGE_H


extern const uint8_t spr_gameover_chars[];
extern const uint8_t spr_you_lost_chars[];
extern const uint8_t spr_you_won_chars[];

extern const uint8_t * p_gameover_chars;

#define GAMEOVER_MESSAGE_SET(p_msg) (p_gameover_chars = &p_msg[0])
#define GAMEOVER_MESSAGE_CHK(p_msg) (p_gameover_chars == &p_msg[0])
void gameover_message_animate(void);
void gameover_message_reset(void);

#endif // GAMEOVER_MESSAGE_H


