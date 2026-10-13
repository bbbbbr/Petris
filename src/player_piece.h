// player_piece.h

#ifndef PLAYER_PIECE_H
#define PLAYER_PIECE_H

#define PLAYER_PIECE_SHOW 1
#define PLAYER_PIECE_HIDE 0

extern  int8_t player_x;
extern  int8_t player_y;
extern  int8_t player_rotate; // Uses wraparound, so allow negative nums
extern uint8_t player_piece;
extern uint8_t player_attrib;

void  player_piece_update_xy(uint8_t);
uint8_t player_piece_try_reload(void);
void  player_piece_set_on_board(void);
uint8_t player_piece_connect_get(uint8_t, uint8_t);

uint8_t player_piece_move(int8_t, int8_t);
void  player_piece_update_gfx(void);
void  player_piece_rotate_apply(uint8_t);

void player_piece_cycle_pet_types(void);

#endif // PLAYER_PIECE_H
