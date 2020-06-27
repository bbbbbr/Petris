// audio_common.h

#include "sound.h"

#define PLAY_SOUND_GAME_OVER          PlayFx(CHANNEL_1, 30, 0x4D, 0x94, 0xF6, 0x26, 0x87); PlayFx(CHANNEL_4, 30, 0x26, 0x35, 0xF2, 0xC0); // delay(700)

// #define PLAY_SOUND_LEVEL_UP        PlayFx(CHANNEL_2, 30, 0x00, 0x82, 0x06, 0xC7); delay(150); PlayFx(CHANNEL_2, 30, 0x00, 0x82, 0x06, 0xC7); delay(80); PlayFx(CHANNEL_2, 30, 0x41, 0x94, 0x22, 0x87);
// #define PLAY_SOUND_LEVEL_UP        PlayFx(CHANNEL_2, 30, 0x41, 0x92, 0x0B, 0xC7); delay(150); PlayFx(CHANNEL_2, 30, 0x41, 0x92, 0x0B, 0xC7); delay(150); PlayFx(CHANNEL_2, 30, 0x41, 0x92, 0x47, 0xC7); // 5B
// #define PLAY_SOUND_LEVEL_UP        PlayFx(CHANNEL_1, 30, 0x76, 0x86, 0xB5, 0xCF, 0x86);
#define PLAY_SOUND_LEVEL_UP           PlayFx(CHANNEL_1, 30, 0x76, 0x86, 0xF3, 0xE3, 0x86) // TODO: improve level up sound


#define PLAY_SOUND_TILE_CLEAR_NORMAL  PlayFx(CHANNEL_1, 30, 0x76, 0xC3, 0xF3, 0x40, 0x87)
#define PLAY_SOUND_TILE_CLEAR_BONUS   PlayFx(CHANNEL_1, 30, 0x76, 0xC3, 0xF3, 0x80, 0x87)
#define PLAY_SOUND_TILE_CLEAR_SPECIAL PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0xF1, 0x73, 0x86) // PlayFx(CHANNEL_1, 30, 0x45, 0xC3, 0x53, 0x00, 0x87);
#define PLAY_SOUND_TILE_CLEAR_BOMB    PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0xF1, 0x73, 0x86)



#define PLAY_SOUND_PRINT_CHAR         PlayFx(CHANNEL_1, 0,  0x20, 0x81, 0xF3, 0x59, 0x86)

#define PLAY_SOUND_OPTION_CURSOR_MOVE PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0xF4, 0x94, 0x86)

// #define PLAY_SOUND_PIECE_ROTATE    PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x24, 0xC6, 0x86)
// #define PLAY_SOUND_PIECE_ROTATE    PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0x24, 0xA4, 0x86)
#define PLAY_SOUND_PIECE_ROTATE       PlayFx(CHANNEL_1, 30, 0x24, 0x80, 0xF4, 0x94, 0x86)
#define PLAY_SOUND_PIECE_LANDED       PlayFx(CHANNEL_1, 30, 0x1C, 0x81, 0xF1, 0x73, 0x86)
#define PLAY_SOUND_PIECE_MOVE         PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0xF2, 0xA6, 0x86)
// #define PLAY_SOUND_PIECE_MOVE      PlayFx(CHANNEL_1, 30, 0x00, 0xC0, 0x31, 0x8F, 0x85)
// #define PLAY_SOUND_PIECE_MOVE      PlayFx(CHANNEL_1, 30, 0x30, 0x81, 0x33, 0x37, 0x87)
// #define PLAY_SOUND_PIECE_MOVE      PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x22, 0xD6, 0x86)
// #define PLAY_SOUND_PIECE_MOVE      PlayFx(CHANNEL_1, 30, 0x74, 0x80, 0x12, 0xD6, 0x86)
