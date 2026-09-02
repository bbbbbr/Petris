#include "loopy.h"


// Offsets of Character/Tile Pattern data from VRAM_BASE based on mode
const uint32_t CHARBASE_OFFSETS[8] = {
    0x4000,  // BG_LAYOUT_64X64_SPLIT   0
    0x2000,  // BG_LAYOUT_64X64_SHARED  1
    0x2000,  // BG_LAYOUT_64X32_SPLIT   2
    0x1000,  // BG_LAYOUT_64X32_SHARED  3
    0x2000,  // BG_LAYOUT_32X64_SPLIT   4
    0x1000,  // BG_LAYOUT_32X64_SHARED  5
    0x1000,  // BG_LAYOUT_32X32_SPLIT   6
    0x0800,  // BG_LAYOUT_32X32_SHARED  7
};


// Offsets of BG1 Tilemap from VRAM_BASE based on mode
const uint32_t BG1_OFFSETS[8] = {
    64 * 64,  // BG_LAYOUT_64X64_SPLIT   0
    0,        // BG_LAYOUT_64X64_SHARED  1
    64 * 32,  // BG_LAYOUT_64X32_SPLIT   2
    0,        // BG_LAYOUT_64X64_SHARED  3
    32 * 64,  // BG_LAYOUT_32X64_SPLIT   4
    0,        // BG_LAYOUT_64X64_SHARED  5
    32 * 32,  // BG_LAYOUT_32X32_SPLIT   6
    0,                           // BG_LAYOUT_64X64_SHARED  7
};
