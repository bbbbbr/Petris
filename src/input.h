// input.h

#ifndef INPUT_H
#define INPUT_H

#define J_WAIT_ALL_RELEASED 0xFFFF
#define J_WAIT_ANY_PRESSED  0x0000

#define J_ANY  (J_UP | J_DOWN | J_LEFT | J_RIGHT | J_SELECT | J_START | J_A | J_B | J_LTRIG | J_RTRIG | J_X | J_Y)
#define J_DPAD (J_UP | J_DOWN | J_LEFT | J_RIGHT)

#define UPDATE_KEYS() previous_keys = keys; keys = READ_GAMEPAD1
#define UPDATE_KEY_REPEAT(MASK) if (MASK & previous_keys & keys) { key_repeat_count++; } else { key_repeat_count=0; }
#define RESET_KEY_REPEAT(NEWVAL) key_repeat_count = NEWVAL

#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K)   ((keys & (K)) && !(previous_keys & (K)))
#define KEY_RELEASED(K) (!(keys & (K)) && (previous_keys & (K)))

#define ANY_KEY_PRESSED (keys)

void waitpadticked_lowcpu(uint16_t button_mask);

extern uint16_t keys;
extern uint16_t previous_keys;
extern uint16_t key_repeat_count;

#endif // INPUT_H


