// player_piece.h

#ifndef PLAYER_PIECE_H
#define PLAYER_PIECE_H

#define PLAYER_PIECE_SHOW 1
#define PLAYER_PIECE_HIDE 0


void  player_piece_update_xy(UINT8);
UINT8 player_piece_try_reload(void);
void  player_piece_set_on_board(void);
UINT8 player_piece_connect_get(UINT8, UINT8);

UINT8 player_piece_move(INT8, INT8);
void  player_piece_update_gfx(void);
void  player_piece_rotate_apply(UINT8);

void player_piece_cycle_pet_types(void);

#endif // PLAYER_PIECE_H
