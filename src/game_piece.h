// game_piece.h

#ifndef GAME_PIECE_H
#define GAME_PIECE_H

// TODO: handle offset?
#define GAME_PIECE_NEXT_PREVIEW_X ((4 + 1) * 8)
#define GAME_PIECE_NEXT_PREVIEW_Y ((10 + 1) * 8)

#define SPR_PLAYER_NEXT      1 // Next piece preview is "1"

#define GAME_PIECE_NEXT_OVERRIDE_NONE 0xFF

void  game_piece_next_generate(void);
UINT8 game_piece_next_get(void);
void  game_piece_next_set(UINT8);
void  game_piece_next_show(UINT8);



#endif // GAME_PIECE_H
