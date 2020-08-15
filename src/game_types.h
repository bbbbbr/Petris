// game_types.h

#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// #include "game_board.h"

extern UINT8 game_type_cleanup_tail_count;
extern UINT8 game_type_long_pet_required_size;

#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN 3
#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX 20

#define GAME_TYPE_PET_LONG_PET_SIZE_MIN 3
#define GAME_TYPE_PET_LONG_PET_SIZE_MAX 40

#define GAME_TYPE_CRUNCH_UP_TAIL_COUNT_ADD 2 // (BRD_WIDTH / 3) // 2-3 on lower levels, 1 on higher?

void game_types_init(void);
void game_types_handle_level_transition(void);

UINT8 game_type_pet_cleanup_get_tail_count(void);
void game_type_pet_cleanup_increment_tail_count(void);
void game_type_pet_cleanup_decrement_tail_count(void);

void game_type_long_pet_set_pet_size(UINT8);
void game_type_long_pet_check_size(UINT8);

#endif