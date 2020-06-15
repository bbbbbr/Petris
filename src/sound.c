// Some of this code is imported from ZGB

#include <stdarg.h>

#include "sound.h"

#include "gbt_player.h"
#include "options.h"

#define MUSIC_NONE 0
UINT8 music_mute_frames;
void * last_music = MUSIC_NONE;

const UINT8 FX_REG_SIZES[] = {5, 4, 5, 4, 3};
const UINT16 FX_ADDRESS[] = {0xFF10, 0xFF16, 0xFF1A, 0xFF20, 0xFF24};

// extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	UINT8 i;
	UINT8* reg = (UINT8*)FX_ADDRESS[channel];
	va_list list;

	va_start(list, mute_frames);
	for(i = 0; i < FX_REG_SIZES[channel]; ++i, ++reg) {
		*reg = va_arg(list, INT16);
	}
	va_end(list);

	if (channel != CHANNEL_5) {
		gbt_enable_channels(~(0xF & (1 << channel)));
	}

	music_mute_frames = mute_frames;
}


void MusicStop(void) {
    gbt_stop();

    // Reset last_music so that music will resume properly.
    last_music = MUSIC_NONE;

    // Restore sound registers that get turned off
    NR52_REG = 0x80; // Enables sound, always set this first
    NR51_REG = 0xFF; // Enables all channels (left and right)
    NR50_REG = 0x77; // Max volume
}


// Removed banking support
// void MusicPlay(const unsigned char * music[], unsigned char bank, unsigned char loop) {
void MusicPlay(const unsigned char * music[], unsigned char loop) {

    if (music != last_music) {
        last_music = music;
        // gbt_play(music, bank, 7);
        gbt_play(music, 0, 7); // Force bank to 0, no bank
        gbt_loop(loop);
        // REFRESH_BANK; // WARNING: re-enable if using banking with an MBC
    }
}


void MusicUpdateStatus() {

    if (option_game_music == OPTION_MUSIC_1)
        MusicPlay(twilight_drive_mod_Data, GBT_LOOP_YES);
    else if (option_game_music == OPTION_MUSIC_2)
        MusicPlay(freeost_charselect_mod_Data, GBT_LOOP_YES);
    else
        MusicStop();
}

void update_gbt_music() {
    gbt_update();
    // REFRESH_BANK; // WARNING: re-enable if using banking with an MBC
}
