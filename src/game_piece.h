// game_piece.h

#ifndef GAME_PIECE_H
#define GAME_PIECE_H

#define GAME_PIECE_NEXT_OVERRIDE_NONE 0xFF

void  game_piece_next_generate(void);
UINT8 game_piece_next_get(void);
void  game_piece_next_set(UINT8);
void  game_piece_next_show(UINT8);



#endif // GAME_PIECE_H
