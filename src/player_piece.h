// player_piece.h

#ifndef PLAYER_PIECE_H
#define PLAYER_PIECE_H

#define PLAYER_PIECE_SHOW 1
#define PLAYER_PIECE_HIDE 0


void  player_piece_update_xy(UINT8);
void  player_piece_reset(void);
void  player_piece_set_on_board(void);
UINT8 player_piece_connect_get();

UINT8 player_piece_move(INT8, INT8);
void  player_piece_update_gfx();
void  player_piece_rotate_apply(UINT8 dir);

#define SPR_PLAYER      0 // Player is sprite "0"


#endif // PLAYER_PIECE_H
