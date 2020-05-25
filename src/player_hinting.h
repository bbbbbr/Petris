// player_hinting.h

#ifndef PLAYER_HINTING_H
#define PLAYER_HINTING_H

#define PLAYER_PIECE_HIDE 0
#define PLAYER_PIECE_SHOW 1

#define SPR_LONG_PET_HINT_OFFSET_X (BRD_PIECE_X_OFFSET + 3)
#define SPR_LONG_PET_HINT_OFFSET_Y (BRD_PIECE_Y_OFFSET + 3)
#define HINT_LONG_PET_MIN_SIZE 3
#define HINT_PET_LENGTH_SLOT_EMPTY -1
#define HINT_PET_LENGTH_SLOT_NONE 99




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
void hinting_petlength_show(void);
void hinting_petlength_remove(INT8 board_x, INT8 board_y);

#endif // PLAYER_HINTING_H
