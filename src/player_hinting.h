// player_hinting.h

#ifndef PLAYER_HINTING_H
#define PLAYER_HINTING_H

#define PLAYER_PIECE_SHOW 1
#define PLAYER_PIECE_HIDE 0

void player_hinting_flicker_update(UINT8);

void player_hinting_special_show(UINT8);
void player_hinting_special_update_gfx(void);
void player_hinting_special_move(void);


#endif // PLAYER_HINTING_H
