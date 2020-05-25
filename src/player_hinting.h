// player_hinting.h

#ifndef PLAYER_HINTING_H
#define PLAYER_HINTING_H

#define PLAYER_PIECE_HIDE 0
#define PLAYER_PIECE_SHOW 1

#define HINT_LONG_PET_INCREMENT_NO  FALSE
#define HINT_LONG_PET_INCREMENT_YES TRUE

#define HINT_LONG_PET_MIN_SIZE 3

void player_hinting_flicker_update(UINT8);

void player_hinting_special_show(UINT8);
void player_hinting_special_update_gfx(void);
void player_hinting_special_move(void);

void player_hinting_drop_show(UINT8);
void player_hinting_drop_update(void);

void hinting_petlength_reset(void);
void hinting_petlength_toggle_enabled(void);
void hinting_petlength_add(INT8 board_x, INT8 board_y, UINT8 length, UINT8 attrib);
void hinting_petlength_hide(void);
void hinting_petlength_show(UINT8 do_increment);
void hinting_petlength_remove(INT8 board_x, INT8 board_y);

#endif // PLAYER_HINTING_H
