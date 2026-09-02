#pragma once

#include <stdint.h>
#include <stdbool.h>

/*
Loopy-specific hardware registers, BIOS functions and reserved memory.
Addresses are defined as linker symbols in address_sh7021.ld.
Generally using intX_t / uintX_t / bool types where size is known.
This is NOT a complete listing!!
*/

#define UNK_P uint32_t

struct soundstate_t {
	char unk[0xA0];
};

/*** Regular functions (call from your code) ***/

// General
extern void     bios_vdpMode(uint8_t controllerMode, bool vidExpand);
extern uint16_t bios_getSealType();
extern void     bios_vsync();

// Sound
extern void     bios_initSoundTransmission();
extern void     bios_playBgm(struct soundstate_t *soundState, uint8_t unkp2, uint16_t index, const char *const (*trackList)[]);
extern void     bios_playSfx(struct soundstate_t *soundState, uint8_t unkp2, uint16_t index, const char *const (*trackList)[]);
extern void     bios_updateBgm(struct soundstate_t*, uint8_t unkp2);
extern void     bios_soundChannels(int channelMode);
extern void     bios_soundVolume(int channel, int volume);
extern void     bios_soundToggleDemo();

// Fast math functions
extern uint16_t bios_colorInverseGray(uint16_t);
extern uint8_t  bios_valueBlendQuarter(uint8_t, uint8_t);
extern uint16_t bios_colorBlendQuarter(uint16_t, uint16_t);
extern int32_t  bios_mathMulS16(int16_t, int16_t);
extern uint32_t bios_mathMulU16(uint16_t, uint16_t);
extern uint16_t bios_mathDivU16(uint16_t, uint16_t);
extern int16_t  bios_mathDivS16(int16_t, int16_t);
extern int32_t  bios_mathDivS32(int32_t, int32_t);

// Printing
extern uint8_t  bios_printParts(uint8_t**, uint16_t**, uint32_t*, UNK_P, UNK_P, uint8_t*, uint8_t, bool);
extern uint8_t  bios_printDirect(uint8_t*, uint16_t*, uint32_t, UNK_P, UNK_P,  uint8_t, bool);
extern uint8_t  bios_print15bpp(uint16_t*, bool);
extern uint8_t  bios_print8bpp(uint8_t*, uint16_t*, bool);
extern uint16_t bios_measurePrintTemp(UNK_P);


/*** Interrupt handlers (use in vector table) ***/

// Sound
extern void     bios_isrAutoUpdateBgm();


/*** BIOS-related variables that the main application may access ***/
// TODO should this be done with inline setters/getters instead?
extern struct soundstate_t *biosvar_autoSoundState;
extern uint16_t biosvar_gamepad[3];


/*** BIOS constant data ***/
extern const char *const biosdata_stopBgmTrackList[1];
