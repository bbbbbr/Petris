// game_piece.h

#ifndef GAME_PIECE_H
#define GAME_PIECE_H

#define GAME_PIECE_STASH_NONE 0xFF

void  game_piece_next_reset(void);
void  game_piece_next_generate(void);
uint8_t game_piece_next_get(void);
void  game_piece_next_set(uint8_t);
void  game_piece_next_show(uint8_t);



#endif // GAME_PIECE_H
