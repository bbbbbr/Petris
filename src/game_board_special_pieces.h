// game_board_special_pieces.h

#ifndef GAME_BOARD_SPECIAL_PIECES_H
#define GAME_BOARD_SPECIAL_PIECES_H

// TODO: load special piece handling into a LUT with Difficulty Level options
//#define PIECE_SPECIAL_THRESHOLD_BOMB 5 // TODO: create real criteria for giving the user a special piece
// #define PIECE_SPECIAL_THRESHOLD_BOMB 2

//#define PIECE_SPECIAL_THRESHOLD_MERGE 0x003F // once every 64 // TODO: create real criteria for giving the user a special piece
// #define PIECE_SPECIAL_THRESHOLD_MERGE 0x003 // once every 4 // TODO: create real criteria for giving the user a special piece

void board_handle_special_bomb(INT8,INT8);

#endif // GAME_BOARD_SPECIAL_PIECES_H
