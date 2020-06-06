// player_gfx.h

// gfx.h

#ifndef PLAYER_GFX_H
#define PLAYER_GFX_H

// Sprite id definition
#define SPR_PLAYER          0x00 // Player tile
#define SPR_PLAYER_NEXT     0x01 // Next piece preview
#define SPR_DROP_HINT       0x02 // TODO: Helper hinting for where the tile will land
#define SPR_SPECIAL_HINT_1  0x03 // Start of hint sprites
#define SPR_SPECIAL_HINT_2  0x04 // Start of hint sprites
#define SPR_SPECIAL_HINT_3  0x05 // Start of hint sprites
#define SPR_SPECIAL_HINT_4  0x06 // Start of hint sprites
#define SPR_LONG_PET_HINT_NUM_START     0x0A
#define SPR_LONG_PET_HINT_NUM_TILES_PER 3 // 2 digit tiles and one penultimate size tile
#define SPR_LONG_PET_HINT_POOL_SIZE     6 // Last N pet lengths shown (2 x this number of sprites used)
#define SPR_LONG_PET_HINT_LAST  (SPR_LONG_PET_HINT_NUM_START + (SPR_LONG_PET_HINT_NUM_TILES_PER * SPR_LONG_PET_HINT_POOL_SIZE) - 1)




#endif // PLAYER_GFX_H