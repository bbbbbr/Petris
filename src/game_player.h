// game_player.h

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

typedef enum {
    PLAYER_MV_LEFT = 1,
    PLAYER_MV_RIGHT,
    PLAYER_MV_DOWN
} PLAYER_MOVE_STATES;

typedef enum {
    PLAYER_ROT_LEFT = 1,
    PLAYER_ROT_RIGHT
} PLAYER_ROT_STATES;

typedef enum {
    MOVE_OK = 0,
    MOVE_BLOCKED,
    MOVE_BLOCKED_GAME_OVER,
} PIECE_MOVE_STATES;

typedef enum {
    PLAYER_START = 0,
    PLAYER_NEWPIECE,
    PLAYER_INPLAY,
    PLAYER_PIECE_LANDED,
    PLAYER_CHECK_BOARD
} PLAYER_STATES;


void  player_init(void);
void player_piece_show(UINT8);
void  player_piece_reset(void);
void  player_piece_set_on_board(void);
UINT8 player_piece_connect_get();

UINT8 player_move(INT8, INT8);
void  player_piece_update_gfx();
void  player_rotate_apply(UINT8 dir);

void player_handle_pause(void);
void  player_handle_input(void);
void  player_gravity_update(void);

#define PLAYER_PIECE_UPDATE_SPRITE_XY move_sprite(SPR_PLAYER, (player_x * BRD_UNIT_SIZE) + BRD_PIECE_X_OFFSET, (player_y * BRD_UNIT_SIZE) + BRD_PIECE_Y_OFFSET)

#endif // GAME_PLAYER_H
