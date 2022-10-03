// input.h

#ifndef INPUT_H
#define INPUT_H

#define J_WAIT_ALL_RELEASED 0xFF
#define J_WAIT_ANY_PRESSED  0x00

#define UPDATE_KEYS() previous_keys = keys; keys = joypad()
#define UPDATE_KEY_REPEAT(MASK) if (MASK & previous_keys & keys) { key_repeat_count++; } else { key_repeat_count=0; }
#define RESET_KEY_REPEAT(NEWVAL) key_repeat_count = NEWVAL

#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K)   ((keys & (K)) && !(previous_keys & (K)))
#define KEY_RELEASED(K) (!(keys & (K)) && (previous_keys & (K)))

#define ANY_KEY_PRESSED (keys)

void waitpadticked_lowcpu(UINT8 button_mask, volatile UINT8 * loop_control);

extern UINT8 keys;
extern UINT8 previous_keys;
extern UINT8 key_repeat_count;

#endif // INPUT_H


