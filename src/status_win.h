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

#ifndef STATUS_WIN_POPUP_H
#define STATUS_WIN_POPUP_H

#define WIN_Y_MAX        144
#define WIN_SCROLL_DELAY 3

#define STATUS_WIN_TILE_UL    0
#define STATUS_WIN_TILE_UR    1
#define STATUS_WIN_TILE_HBAR  2
#define STATUS_WIN_TILE_VBAR  3
#define STATUS_WIN_TILE_BLANK 4
#define MKPET_TILE(offset) (TILES_PET_START + offset)


void status_win_popup_init(void);
void status_win_popup_show(UINT8);
void status_win_popup_hide(void);

#endif // STATUS_WIN_POPUP_H