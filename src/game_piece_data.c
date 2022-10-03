// Copyright 2020 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

// game_piece_data.c

#include <gbdk/platform.h>

#include "game_piece_data.h"


// No longer needed, pet tiles are not x/y mirror deduped anymore
//
// This gets applied to BG Tile/Sprite INDEX:0x10
// const UINT8 GP_ROT_LUT_TILE[4] =
//                 {GP_ROT_HORZ_BITS,     // GP_ROTATE_0
//                  GP_ROT_VERT_BITS,     // GP_ROTATE_90
//                  GP_ROT_HORZ_BITS,     // GP_ROTATE_180
//                  GP_ROT_VERT_BITS};    // GP_ROTATE_270


// No longer needed, pet tiles are not x/y mirror deduped anymore
//
// // This gets BG Tile/Sprite ATTRIB:0x60
// const UINT8 GP_ROT_LUT_ATTR[4] =
//                 {GP_MIRROR_OFF,
//                  GP_MIRROR_OFF,
//                  GP_MIRROR_X,
//                  GP_MIRROR_Y };


// Game board piece connection to adjacent
// piece matching connection lookup
const UINT8 GP_CONNECT_LUT[] =
                {GP_CONNECT_RIGHT_BITS  | GP_CONNECT_NONE_BITS,  // GP_SEG_TAIL     0
                 GP_CONNECT_LEFT_BITS   | GP_CONNECT_RIGHT_BITS, // GP_SEG_TORSO    1
                 GP_CONNECT_RIGHT_BITS  | GP_CONNECT_DOWN_BITS,  // GP_SEG_TURN     2
                 GP_CONNECT_RIGHT_BITS  | GP_CONNECT_NONE_BITS}; // GP_SEG_HEAD     3


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

