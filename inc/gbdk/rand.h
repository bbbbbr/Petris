#ifndef _GBDK_RAND_H
#define _GBDK_RAND_H

extern uint16_t __rand_seed;
extern uint16_t rand(void);
extern uint16_t initrand(uint16_t seed);

#endif