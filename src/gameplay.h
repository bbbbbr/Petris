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
    MOVE_BLOCKED
} PIECE_MOVE_STATES;

typedef enum {
    PLAYER_START = 0,
    PLAYER_NEWPIECE_PRELAUNCH,
    PLAYER_NEWPIECE_LAUNCH,
    PLAYER_INPLAY,
    PLAYER_PIECE_LANDED,
    PLAYER_CHECK_BOARD
} PLAYER_STATES;

#define GAMEPLAY_GET_READY_FLASHES 2

#define GAME_CRUNCHUP_NONE                0
#define GAME_CRUNCHUP_SHAKE_RESET         0  // Reset must be zero so that enabled/disabled test works
#define GAME_CRUNCHUP_SHAKE_START         1
#define GAME_CRUNCHUP_SHAKE_COMPLETE      13 // Shake ~1/4 second before crunch-up
#define GAME_CRUNCHUP_FRAME_THRESHOLD     (60 * 12) // N seconds between crunch-ups (don't factor in board shake time)
#define GAME_CRUNCHUP_FRAME_COUNTER_RESET 0

extern UINT8 game_rand_init;
extern UINT8 volatile game_crunchups_enqueued;
extern UINT8 volatile game_shake_enqueued;
extern UINT8 volatile game_is_paused;

void gameplay_drop_speed_update(void);
void gameplay_ended_cleanup(void);
void gameplay_handle_gameover_screen(void);
void random_init(void);
void gameplay_init(void);
void gameplay_prepare_board(void);
void gameplay_handle_get_ready(void);
void gameplay_handle_pause(void);
void gameplay_handle_input(void);
void gameplay_update(void);

void gameplay_gravity_update(void);
void gameplay_crunchup_update(void);



#endif // GAME_PLAYER_H
