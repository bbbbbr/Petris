// player_hinting.h

#ifndef PLAYER_HINTING_H
#define PLAYER_HINTING_H

#define PLAYER_PIECE_HIDE 0
#define PLAYER_PIECE_SHOW 1

#define SPR_LONG_PET_HINT_OFFSET_X (BRD_PIECE_X_OFFSET + 4)
#define SPR_LONG_PET_HINT_OFFSET_Y (BRD_PIECE_Y_OFFSET + 4)
#define SPR_LONG_PET_SIZE_OFFSET_X (BRD_PIECE_X_OFFSET)
#define SPR_LONG_PET_SIZE_OFFSET_Y (BRD_PIECE_Y_OFFSET)

#define HINT_LONG_PET_MIN_SIZE 3
#define HINT_PET_LENGTH_SLOT_EMPTY -1
#define HINT_PET_LENGTH_SLOT_NONE 255

#define HINT_PET_LENGTH_TIMEOUT 150 // In Frames

extern bool hinting_petlength_enabled;

void player_hinting_flicker_update(void);

void player_hinting_special_reset(void);
void player_hinting_special_show(uint8_t);
void player_hinting_special_update_gfx(void);
void player_hinting_special_move(void);

void player_hinting_drop_reset(void);
void player_hinting_drop_show(uint8_t);
void player_hinting_drop_update(void);

void hinting_petlength_reset(void);
void hinting_petlength_turn_on(void);
void hinting_petlength_add(int8_t board_x, int8_t board_y, uint8_t length, uint8_t attrib);
void hinting_petlength_showhide(void);
void hinting_petlength_remove(int8_t board_x, int8_t board_y);
void hinting_petlength_scrollup(void);
void hinting_petlength_refreshxy(void);

#endif // PLAYER_HINTING_H
