// Copyright 2026 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include <gbdk/platform.h>

#include "common.h"

uint16_t game_state = GAME_INTRO_INIT;
uint16_t vbl_count = 0;

const uint16_t shared_4bpp_transparent_tile[] = {
    0x0000u,
    0x0000u,
    0x0000u,
    0x0000u,
    0x0000u,
    0x0000u,
    0x0000u,
    0x0000u,
};
