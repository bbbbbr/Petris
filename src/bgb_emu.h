// bgb_emu.h
#ifndef __BGB_EMU_INCLUDE
#define __BGB_EMU_INCLUDE

// See: https://gbdev.gg8.se/forums/viewtopic.php?id=703

#define ADD_DOLLARD(A) ADD_DOLLARD1 (A)
#define ADD_DOLLARD1(A) A##$
#define BGB_MESSAGE(message_text) BGB_MESSAGE1(ADD_DOLLARD(__LINE__), message_text)
#define BGB_MESSAGE1(lbl, message_text) \
__asm \
  ld d, d \
  jr lbl \
  .dw 0x6464 \
  .dw 0x0000 \
  .ascii message_text \
lbl: \
__endasm


#define BGB_PROFILE_START BGB_MESSAGE(1$, "%ZEROCLKS%");

// Note: Clock count is affected by interrupts, code can be wrapped in __critical { } to disable them for the block
#define BGB_PROFILE_END   BGB_MESSAGE(2$, "TIME: %-8+LASTCLKS%");

#endif

