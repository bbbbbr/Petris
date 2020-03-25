// game_piece_data.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "game_piece_data.h"


// This gets applied to BG Tile/Sprite INDEX:0x10
const UINT8 GP_ROT_LUT_TILE[4] =
                {GP_ROT_HORZ_BITS,     // GP_ROTATE_0
                 GP_ROT_VERT_BITS,     // GP_ROTATE_90
                 GP_ROT_HORZ_BITS,     // GP_ROTATE_180
                 GP_ROT_VERT_BITS};    // GP_ROTATE_270

// This gets BG Tile/Sprite ATTRIB:0x60
const UINT8 GP_ROT_LUT_ATTR[4] =
                {GP_MIRROR_OFF,
                 GP_MIRROR_OFF,
                 GP_MIRROR_X,
                 GP_MIRROR_Y };

// TODO: clean up unused LUTS
/*
// Game board piece connection bitmask lookup
const UINT8 GP_CONNECT_LUT[4] =
                {GP_CONNECT_1_LEFT_BITS,
                 GP_CONNECT_1_RIGHT_BITS,
                 GP_CONNECT_1_UP_BITS,
                 GP_CONNECT_1_DOWN_BITS };


// Game board piece connection to adjacent
// piece matching connection lookup
const UINT8 GP_CONNECT_MATCH_LUT[4] =
                {GP_CONNECT_2_RIGHT_BITS,
                 GP_CONNECT_2_LEFT_BITS,
                 GP_CONNECT_2_DOWN_BITS,
                 GP_CONNECT_2_UP_BITS };
*/

// Game board piece connection to adjacent
// piece matching connection lookup
// GP_SEG_TAIL     0
// GP_SEG_TORSO    1
// GP_SEG_TURN     2
// GP_SEG_HEAD     3
// const UINT8 GP_CONNECT_SEG_LUT[4][2] =
//                 {{GP_CONNECT_RIGHT, GP_CONNECT_NONE},
//                  {GP_CONNECT_LEFT,  GP_CONNECT_RIGHT},
//                  {GP_CONNECT_RIGHT, GP_CONNECT_DOWN},
//                  {GP_CONNECT_RIGHT, GP_CONNECT_NONE}};

// Game board piece connection to adjacent
// piece matching connection lookup
// GP_SEG_TAIL     0
// GP_SEG_TORSO    1
// GP_SEG_TURN     2
// GP_SEG_HEAD     3
const UINT8 GP_CONNECT_LUT[] =
                {GP_CONNECT_RIGHT_BITS  | GP_CONNECT_NONE_BITS,
                 GP_CONNECT_LEFT_BITS   | GP_CONNECT_RIGHT_BITS,
                 GP_CONNECT_RIGHT_BITS  | GP_CONNECT_DOWN_BITS,
                 GP_CONNECT_RIGHT_BITS  | GP_CONNECT_NONE_BITS};

// Maps the matching bits for the opposite side of a connection
const UINT8 GP_CONNECT_MATCHING_LUT[] =
                 {GP_CONNECT_NONE_BITS,    //                          0x00U
                  GP_CONNECT_RIGHT_BITS,   // GP_CONNECT_LEFT_BITS     0x01U
                  GP_CONNECT_DOWN_BITS,    // GP_CONNECT_UP_BITS       0x02U
                  GP_CONNECT_NONE_BITS,    //                          0x03U
                  GP_CONNECT_LEFT_BITS,    // GP_CONNECT_RIGHT_BITS    0x04U
                  GP_CONNECT_NONE_BITS,    //                          0x05U
                  GP_CONNECT_NONE_BITS,    //                          0x06U
                  GP_CONNECT_NONE_BITS,    //                          0x07U
                  GP_CONNECT_UP_BITS};      // GP_CONNECT_DOWN_BITS     0x08U

// Looks up which direction a connection bit points to
const INT8 GP_CONNECT_NEXT_X_LUT[] =
                 { 0,    //                          0x00U
                  -1,    // GP_CONNECT_LEFT_BITS     0x01U
                   0,    // GP_CONNECT_UP_BITS       0x02U
                   0,    //                          0x03U
                   1,    // GP_CONNECT_RIGHT_BITS    0x04U
                   0,    //                          0x05U
                   0,    //                          0x06U
                   0,    //                          0x07U
                   0};   // GP_CONNECT_DOWN_BITS     0x08U

const INT8 GP_CONNECT_NEXT_Y_LUT[] =
                 { 0,    //                          0x00U
                   0,    // GP_CONNECT_LEFT_BITS     0x01U
                  -1,    // GP_CONNECT_UP_BITS       0x02U
                   0,    //                          0x03U
                   0,    // GP_CONNECT_RIGHT_BITS    0x04U
                   0,    //                          0x05U
                   0,    //                          0x06U
                   0,    //                          0x07U
                   1};   // GP_CONNECT_DOWN_BITS     0x08U

