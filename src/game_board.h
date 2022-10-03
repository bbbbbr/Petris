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
#define BRD_MIN_Y_RANDOM_FILL 7


#define BRD_SIZE     (BRD_WIDTH * BRD_HEIGHT)
#define BRD_CON_SIZE (BRD_CON_WIDTH * BRD_CON_HEIGHT)

#define BRD_UNIT_SIZE  8 // 8 screen pixels per board tile

#define BRD_NEWPIECE_X 5 // BRD_WIDTH/2
#define BRD_NEWPIECE_Y BRD_ST_Y

// OAM Sprite coordinates are for LOWER RIGHT of sprite
#define OAM_SPRITE_X_OFFSET (8 + SPRITE_OFS_X)
#define OAM_SPRITE_Y_OFFSET (16 + SPRITE_OFS_Y)
#define BRD_PIECE_X_OFFSET (OAM_SPRITE_X_OFFSET + (BRD_ST_X * BRD_UNIT_SIZE))
#define BRD_PIECE_Y_OFFSET (OAM_SPRITE_Y_OFFSET + (BRD_ST_Y * BRD_UNIT_SIZE))

#define BRD_TAIL_ADD_NORMAL   0
#define BRD_TAIL_ADD_CRUNCHUP 1

#define CGB_BG_ATTRIB_PAL_MASK 0x03

#define BRD_CONNECT_NO  0
#define BRD_CONNECT_YES 1

#define BRD_CHECK_FLAGS_NONE                  0x00
#define BRD_CHECK_FLAGS_IGNORE_PET_TYPE       0x01
#define BRD_CHECK_FLAGS_DONT_HANDLE_PET_COMPLETED  0x20
#define BRD_CHECK_FLAGS_DONT_ADD_POINTS       0x80

#define BRD_TILE_COUNT_BONUS_SOUND_THRESHOLD 4

#define MSG_GET_READY_X  BRD_ST_X + 0
#define MSG_GET_READY_Y  BRD_ST_Y + 5
#define MSG_GET_READY_TEXT   "   GET\n\n  READY!"
#define MSG_GET_READY_CTEXT  "      \n\n        "
#define MSG_GET_READY_REPEAT 2

#define MSG_LEVEL_UP_X  BRD_ST_X + 1
#define MSG_LEVEL_UP_Y  BRD_ST_Y + 5
#define MSG_LEVEL_UP_TEXT   "LEVEL UP \n\n\n  GET\n\n READY!"
#define MSG_LEVEL_UP_CTEXT  "LEVEL UP \n\n\n     \n\n       "

#define MSG_CLEANUP_START_TEXT    "          \n\n\n   ALL\n\n  TAILS!"
#define MSG_CLEANUP_START_CTEXT   " CLEAN UP \n\n\n   ALL\n\n  TAILS!"
#define MSG_CLEANUP_START_REPEAT 3

#define MSG_LONG_PET_X  BRD_ST_X + 0
#define MSG_LONG_PET_Y  BRD_ST_Y + 3

#ifdef NINTENDO
    #define MSG_LONG_PET_START_TEXT    "        \n\n   LONG\n   PET!\n\n (PRESS\n  SELECT\n\n  FOR PET\n  SIZES)\n "
    #define MSG_LONG_PET_START_CTEXT   "  MAKE A\n\n   LONG\n   PET!\n\n (PRESS\n  SELECT\n\n  FOR PET\n  SIZES)\n "
#elif defined(GAMEGEAR)
    #define MSG_LONG_PET_START_TEXT    "        \n\n   LONG\n   PET!\n\n (PRESS\n    UP\n\n  FOR PET\n  SIZES)\n "
    #define MSG_LONG_PET_START_CTEXT   "  MAKE A\n\n   LONG\n   PET!\n\n (PRESS\n    UP\n\n  FOR PET\n  SIZES)\n "
#endif

#define MSG_LONG_PET_START_REPEAT 3

#define MSG_COMPLETE_PET_X  BRD_ST_X + 0
#define MSG_COMPLETE_PET_Y  BRD_ST_Y + 3
#define MSG_COMPLETE_PET_START_TEXT    "          \n\n          \n\n\n    TO    \n\n LEVEL UP!\n\n"
#define MSG_COMPLETE_PET_START_CTEXT   " COMPLETE \n\n   PETS   \n\n\n    TO    \n\n LEVEL UP!\n\n"
#define MSG_COMPLETE_PET_START_REPEAT 3


#define MSG_LEVEL_UP_REPEAT 2

#define BRD_CLR_DELAY_NONE 0
#define BRD_CLR_DELAY_CLEAR_MED 35// 25

#define BRD_PIECE_CLEAR_COUNT_NONE 0

extern UINT8 board_pieces[BRD_SIZE];
extern UINT8 board_attrib[BRD_SIZE];
extern UINT8 board_connect[BRD_SIZE];

extern UINT8 board_tile_clear_cache_x[BRD_SIZE];
extern UINT8 board_tile_clear_cache_y[BRD_SIZE];
extern UINT8 board_tile_clear_count;



void board_init(void);

void board_hide_all(UINT16);
void board_redraw_all(void);
void board_crunch_up(void);
void board_draw_tile_xy(INT8, INT8, UINT8);

void board_reset(void);
void board_clear_tile_xy(INT8, INT8);

void board_flash_message(UINT8, UINT8, char *, char *, UINT8);

INT8 board_find_lowest_open_in_column(INT8, INT8);
UINT8 board_check_open_xy(INT8, INT8);
void board_set_tile_xy(INT8, INT8, UINT8, UINT8, UINT8);
void board_handle_new_piece(INT8, INT8, UINT8, UINT8);

void game_board_fill_random_tails(UINT8, INT8, UINT8);

UINT8 board_piece_get_xy(INT8, INT8, UINT8 * , UINT8 * );
UINT8 board_check_connected_xy(INT8, INT8, UINT8, UINT8 *, UINT8);
void  board_check_completed_pet_xy(INT8, INT8, UINT8, UINT8, UINT8);

void board_handle_pet_completed(UINT8 flags);
void board_debug_show_connect_entire_board(void);

#endif // GAME_BOARD_H
