
// This is an add-on for gbdk's rand.s

#ifndef SWAPRAND_INCLUDE
#define SWAPRAND_INCLUDE

#include <types.h>

/** Swaps between primary and secondary random number state vars.
    Allows running two sequences in parallel
*/
void swaprand(void); /* Non-banked */

#endif
