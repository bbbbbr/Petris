// magic_code.h

#ifndef MAGIC_CODE_H
#define MAGIC_CODE_H


#define MAGIC_CODE_STATE_RESET     0x00
// NOTE: There are multiple counter states between
//       these two upper and lower bound #defines
#define MAGIC_CODE_STATE_ACTIVATED 0xFF // This should be higher than the index of the last button sequence entry

extern UINT8 magic_code_state;

void magic_code_reset(void);
void magic_code_update(void);


#endif // MAGIC_CODE_H
