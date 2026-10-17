// player_gfx.h

// gfx.h

#ifndef PLAYER_GFX_H
#define PLAYER_GFX_H

// Sprite id definition

// Sprites used during Active Gameplay
#define SPR_PLAYER          0x00 // Player tile
#define SPR_PLAYER_NEXT     0x01 // Next piece preview
#define SPR_DROP_HINT       0x02 // Helper hinting for where the tile will land
#define SPR_SPECIAL_HINT_1  0x03 // Start of hint sprites
#define SPR_SPECIAL_HINT_2  0x04 // Start of hint sprites
#define SPR_SPECIAL_HINT_3  0x05 // Start of hint sprites
#define SPR_SPECIAL_HINT_4  0x06 // Start of hint sprites
#define SPR_LONG_PET_HINT_NUM_START     0x0A
#define SPR_LONG_PET_HINT_NUM_TILES_PER 3 // 2 digit tiles and one penultimate size tile
#define SPR_LONG_PET_HINT_POOL_SIZE     6 // Last N pet lengths shown (2 x this number of sprites used)
#define SPR_LONG_PET_HINT_LAST  (SPR_LONG_PET_HINT_NUM_START + (SPR_LONG_PET_HINT_NUM_TILES_PER * SPR_LONG_PET_HINT_POOL_SIZE) - 1)


#define SPR_GAMEOVER_START SPR_LONG_PET_HINT_LAST + 1
#define SPR_GAMEOVER_COUNT (9u) // 9 Sprites: G A M E O V E R (In this case 9 sprites composed of 2 8x8 tiles, so 18 actually)

#define SPR_PAUSE_START    (SPR_GAMEOVER_START)  // Reuse Game Over sprite slots for Pause text

#define SPR_MAXPET_START (SPR_GAMEOVER_START)         // Reuse Game Over sprite slots for Max Pet text
#define SPR_MAXPET_COUNT (8u * FONT_8x16_TILE_HEIGHT) // 8 Sprites: MAX PET
#define SPR_MAXPET_LAST  (SPR_MAXPET_START + SPR_MAXPET_COUNT)


#endif // PLAYER_GFX_H