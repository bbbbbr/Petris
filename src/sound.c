// Some of this code is imported from ZGB

#include <stdarg.h>

#include "sound.h"

#ifdef NINTENDO
    #include "gbt_player.h"
#endif

#include "options.h"

#ifdef NINTENDO
    #include "unapack.h"
#endif


#define MUSIC_NONE 0
UINT8 music_mute_frames = 0;
void * last_music = MUSIC_NONE;

const UINT8 FX_REG_SIZES[] = {5, 4, 5, 4, 3};
const UINT16 FX_ADDRESS[] = {0xFF10, 0xFF16, 0xFF1A, 0xFF20, 0xFF24};

// uncompressed sizes of audio tracks
//
// villainsofhiphop.b0.mod,CODE,3045,
// boss_fight.b0.mod,CODE,1217,
// freeost_charselect.b0.mod,CODE,2411,
// twilight_drive.b0.mod,CODE,2998,
//
// Arrays for storing the decompressed audio output in non-banked RAM
#define MUSIC_LEN_BYTES_MAX      3100 // WARNING: MUST UPDATE if uncompressed audio size increases
#define MUSIC_SEQ_ADDR_COUNT_MAX 20
UINT8 music_decompressed[MUSIC_LEN_BYTES_MAX];
unsigned char * music_seq_addrs[MUSIC_SEQ_ADDR_COUNT_MAX];



void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
#ifdef NINTENDO
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
#endif
}


void MusicStop(void) {
#ifdef NINTENDO
    gbt_stop();

    // Reset last_music so that music will resume properly.
    last_music = MUSIC_NONE;

    // Restore sound registers that get turned off
    NR52_REG = 0x80; // Enables sound, always set this first
    NR51_REG = 0xFF; // Enables all channels (left and right)
    NR50_REG = 0x77; // Max volume
#endif
}


// void MusicPlay(const unsigned char * music[], unsigned char bank, unsigned char loop) {

// Removed banking support
// void MusicPlay(const unsigned char * music[], unsigned char loop) {
//
//     if (music != last_music) {
//         last_music = music;
//         // gbt_play(music, bank, 7);
//         gbt_play(music, 0, 7); // Force bank to 0, no bank
//         gbt_loop(loop);
//         // REFRESH_BANK; // WARNING: re-enable if using banking with an MBC
//     }
// }

// Added decompression support
// Requires modified mod2gbt which produces a single large array and list of offsets
void MusicPlay(const unsigned char music_compressed[], const unsigned int music_seq_offset[], unsigned char loop) {

#ifdef NINTENDO
    UINT8 c;

    if (music_compressed != last_music) {

        // Pause music before starting decompression so that
        gbt_stop();

        // Decompress music
        UNAPACK(music_compressed, music_decompressed);

        // Rewrite sequence table
        for (c = 0; c < MUSIC_SEQ_ADDR_COUNT_MAX; c++) {

            // If address offset is end-of-sequence indicator
            // rewrite it to 0x0000 that gbt_player expects
            // and break out of the loop
            if (music_seq_offset[c] == 0xFFFF) {
                music_seq_addrs[c] = 0x0000;
                break;
            } else {
                music_seq_addrs[c] = music_decompressed + music_seq_offset[c];
            }
        }

        last_music = (void *) music_compressed;
        gbt_play(music_seq_addrs, 0, 7); // Force bank to 0, no bank
        gbt_loop(loop);
        // REFRESH_BANK; // WARNING: re-enable if using banking with an MBC
    }
#endif
}


void MusicUpdateStatus() {

#ifdef NINTENDO
    if (option_game_music == OPTION_MUSIC_TWILIGHT)
        MusicPlay(twilight_drive_mod, twilight_drive_mod_Data, GBT_LOOP_YES);
    else if (option_game_music == OPTION_MUSIC_UPBEAT)
        MusicPlay(freeost_charselect_mod, freeost_charselect_mod_Data, GBT_LOOP_YES);
   else if (option_game_music == OPTION_MUSIC_CHILL)
       MusicPlay(villainsofhiphop_mod, villainsofhiphop_mod_Data, GBT_LOOP_YES);
    else
        MusicStop();
#endif
}

void update_gbt_music() {
#ifdef NINTENDO
    gbt_update();
    // REFRESH_BANK; // WARNING: re-enable if using banking with an MBC
#endif
}
