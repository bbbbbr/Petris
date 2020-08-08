// magic_code.h

#ifndef magic_code_H
#define magic_code_H

// TODO: FIXME: Using an enum here breaks the compiler/linker somehow
#define MAGIC_CODE_STATE_RESET     0x00
// There are multuple counter states between these two
#define MAGIC_CODE_STATE_ACTIVATED 0xFF // This should be higher than the index of the last button sequence entry

extern UINT8 magic_code_state;

void magic_code_reset(void);
void magic_code_update(void);


#endif // magic_code_H
