// input.h

#ifndef INPUT_H
#define INPUT_H


#define UPDATE_KEYS() previous_keys = keys; keys = joypad()

#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K)   ((keys & (K)) && !(previous_keys & (K)))
#define KEY_RELEASED(K) ((previous_keys & (K)) && !(keys & (K)))

#define ANY_KEY_PRESSED (keys)
#define ANY_KEY_TICKED  (keys != previous_keys) // TODO: this is sloppy

extern UINT8 keys;
extern UINT8 previous_keys;

#endif // INPUT_H


