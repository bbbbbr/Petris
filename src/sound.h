#ifndef SOUND_H
#define SOUND_H

#include <gbdk/platform.h>

typedef enum {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5
} SOUND_CHANNEL;

#define GBT_LOOP_NO  0
#define GBT_LOOP_YES 1


// GAMEGEAR: Present but unused at this time
// External mod music data from gbt_player's mod2gbt
extern const unsigned int boss_fight_mod_Data[];
extern const unsigned int twilight_drive_mod_Data[];
extern const unsigned int freeost_charselect_mod_Data[];
extern const unsigned int villainsofhiphop_mod_Data[];
// extern const unsigned int falling_asleep_mod_Data[];

extern const unsigned char boss_fight_mod[];
extern const unsigned char twilight_drive_mod[];
extern const unsigned char freeost_charselect_mod[];
extern const unsigned char villainsofhiphop_mod[];
// extern const unsigned char falling_asleep_mod[];


extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...);
// void MusicPlay(const unsigned char * music[], unsigned char loop);
void MusicPlay(const unsigned char music[], const unsigned int music_seq_offset[], unsigned char loop);
void MusicUpdateStatus(void);
void MusicStop(void);
void update_gbt_music();

#endif