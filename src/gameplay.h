// gameplay.h

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

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


void gameplay_handle_gameover_screen(void);
void gameplay_exit_cleanup(void);
void gameplay_init(void);
void gameplay_handle_pause(void);
void gameplay_handle_input(void);
void gameplay_update(void);
void gameplay_gravity_update(void);



#endif // GAME_PLAYER_H
