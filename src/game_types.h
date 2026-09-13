// game_types.h

#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "game_board.h"

extern uint8_t game_type_cleanup_tail_count;
extern uint8_t game_type_long_pet_required_size;

// Max should never be larger than:
//   BRD_WIDTH * (BRD_HEIGHT - BRD_MIN_Y_RANDOM_FILL)
#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MIN 3
#define GAME_TYPE_PET_CLEANUP_TAIL_COUNT_MAX 20


#define GAME_TYPE_PET_LONG_PET_SIZE_MIN 3
#define GAME_TYPE_PET_LONG_PET_SIZE_MAX 40

#define GAME_TYPE_CRUNCH_UP_TAIL_COUNT_ADD 2 // (BRD_WIDTH / 3) // 2-3 on lower levels, 1 on higher?

void game_types_init(void);
void game_types_handle_level_transition(void);

uint8_t game_type_pet_cleanup_get_tail_count(void);
void game_type_pet_cleanup_increment_tail_count(void);
void game_type_pet_cleanup_decrement_tail_count(void);

void game_type_long_pet_set_pet_size(uint8_t);
void game_type_long_pet_check_size(uint8_t);

#endif