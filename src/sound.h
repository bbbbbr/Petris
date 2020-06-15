#ifndef SOUND_H
#define SOUND_H

#include <gb/gb.h>

typedef enum {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5
} SOUND_CHANNEL;

#define GBT_LOOP_NO  0
#define GBT_LOOP_YES 1


// External mod music data from gbt_player's mod2gbt
extern const unsigned char * boss_fight_mod_Data[];
extern const unsigned char * twilight_drive_mod_Data[];
extern const unsigned char * freeost_charselect_mod_Data[];


extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...);
void MusicPlay(const unsigned char * music[], unsigned char loop);
void MusicUpdateStatus(void);
void MusicStop(void);
void update_gbt_music();

#endif