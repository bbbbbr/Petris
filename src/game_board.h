// game_board.h

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#define BRD_ST_X   8 // Location in BG Tiles
#define BRD_ST_Y   0
#define BRD_WIDTH  10 // Width in BG Tiles
#define BRD_HEIGHT 15
#define BRD_END_X  (BRD_ST_X + BRD_WIDTH  -1)
#define BRD_END_Y  (BRD_ST_Y + BRD_HEIGHT -1)

#define BRD_CON_WIDTH (BRD_WIDTH - 1)
#define BRD_CON_HEIGHT (BRD_HEIGHT - 1)

#define BRD_MIN_X  0
#define BRD_MAX_X  (BRD_WIDTH - 1)
#define BRD_MIN_Y  0
#define BRD_MAX_Y  (BRD_HEIGHT - 1)


#define BRD_SIZE     (BRD_WIDTH * BRD_HEIGHT)
#define BRD_CON_SIZE (BRD_CON_WIDTH * BRD_CON_HEIGHT)

#define BRD_UNIT_SIZE  8 // 8 screen pixels per board tile

#define BRD_NEWPIECE_X 5 // BRD_WIDTH/2
#define BRD_NEWPIECE_Y BRD_ST_Y

// OAM Sprite coordinates are for LOWER RIGHT of sprite
#define OAM_SPRITE_X_OFFSET 8
#define OAM_SPRITE_Y_OFFSET 16
#define BRD_PIECE_X_OFFSET (OAM_SPRITE_X_OFFSET + (BRD_ST_X * BRD_UNIT_SIZE))
#define BRD_PIECE_Y_OFFSET (OAM_SPRITE_Y_OFFSET + (BRD_ST_Y * BRD_UNIT_SIZE))

#define CGB_BG_ATTRIB_PAL_MASK 0x03

#define BRD_CONNECT_NO  0
#define BRD_CONNECT_YES 1

#define BRD_CHECK_FLAGS_NONE            0x00
#define BRD_CHECK_FLAGS_IGNORE_PET_TYPE 0x01

#define BRD_TILE_COUNT_BONUS_SOUND_THRESHOLD 4

// TODO: move these to a different location?
#define MSG_GET_READY_X  BRD_ST_X + 2
#define MSG_GET_READY_Y  BRD_ST_Y + 5
#define MSG_GET_READY_TEXT   " GET\n\nREADY!"
#define MSG_GET_READY_CTEXT  "    \n\n      "
#define MSG_GET_READY_REPEAT 2

#define MSG_LEVEL_UP_X  BRD_ST_X + 1
#define MSG_LEVEL_UP_Y  BRD_ST_Y + 5
#define MSG_LEVEL_UP_TEXT   "LEVEL UP \n\n\n  GET\n\n READY!"
#define MSG_LEVEL_UP_CTEXT  "LEVEL UP \n\n\n     \n\n       "
#define MSG_LEVEL_UP_REPEAT 2

#define BRD_CLR_DELAY_NONE 0
#define BRD_CLR_DELAY_CLEAR_MED 35// 25

void board_hide_all(UINT16);
void board_redraw_all(void);
void board_draw_tile_xy(INT8, INT8, UINT8);

void board_reset(void);
void board_clear_tile_xy(INT8, INT8);

void board_flash_message(UINT8, UINT8, char *, char *, UINT8);

INT8 board_find_lowest_open_in_column(INT8, INT8);
UINT8 board_check_open_xy(INT8, INT8);
void board_set_tile_xy(INT8, INT8, UINT8, UINT8, UINT8);

void board_fill_random(void);

UINT8 board_piece_get_xy(INT8, INT8, UINT8 * , UINT8 * );
UINT8 board_check_connected_xy(INT8, INT8, UINT8, UINT8 *, UINT8);
UINT8 board_check_completed_pet_xy(INT8, INT8, UINT8, UINT8, UINT8);

void board_handle_pet_completed(UINT8 flags);
void board_debug_show_connect_entire_board(void);

#endif // GAME_BOARD_H
