/*
Addresses of Loopy-specific hardware registers, BIOS functions and reserved memory.
*/

/*** VDP/MMIO structures ***/
_VDP = 0x0C000000;


/*** BIOS regular functions (call from your code) ***/

/* General */
_bios_vdpMode = 0x0668;
_bios_getSealType = 0x115C;
_bios_vsync = 0x6A5A;

/* Sound */
_bios_initSoundTransmission = 0x613C;
_bios_playBgm = 0x61A0;
_bios_playSfx = 0x61B8;
_bios_updateBgm = 0x6238;
_bios_soundChannels = 0x6AC0;
_bios_soundVolume = 0x6B50;
_bios_soundToggleDemo = 0x6B86;

/* Fast math functions */
_bios_colorInverseGray = 0x2D68;
_bios_valueBlendQuarter = 0x2DC6;
_bios_colorBlendQuarter = 0x2DE0;
_bios_mathMulS16 = 0x2E68;
_bios_mathMulU16 = 0x2E72;
_bios_mathDivU16 = 0x2E7C;
_bios_mathDivS16 = 0x2EA6;
_bios_mathDivS32 = 0x2EDE;

/* Printing */
_bios_printParts = 0x06D4;
_bios_printDirect = 0x0FD6;
_bios_print15bpp = 0x101C;
_bios_print8bpp = 0x1064;
_bios_measurePrintTemp = 0x13B0;


/*** BIOS interrupt handlers (use in vector table) ***/

/* Sound */
_bios_isrAutoUpdateBgm = 0x648C;


/*** BIOS-related memory that the main application may access ***/
_biosvar_autoSoundState = 0x0900003C;
_biosvar_gamepad = 0x09000040;


/*** BIOS constant data ***/
_biosdata_stopBgmTrackList = 0x7F88;
