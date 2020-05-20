// game_types.h

#ifndef GAME_TYPES_H
#define GAME_TYPES_H

extern UINT8 game_type_cleanup_tail_count;
extern UINT8 game_type_long_pet_required_size;

void game_types_init(void);
void game_types_handle_level_transition(void);

UINT8 game_type_pet_cleanup_get_tail_count(UINT8);
void game_type_pet_cleanup_increment_tail_count(void);
void game_type_pet_cleanup_decrement_tail_count(void);

void game_type_long_pet_set_pet_size(UINT8);
void game_type_long_pet_check_size(UINT8);

#endif