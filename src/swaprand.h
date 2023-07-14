
#ifndef SWAPRAND_INCLUDE
#define SWAPRAND_INCLUDE

#include <gbdk/platform.h>
#include <stdint.h>

/** Swaps between primary and secondary random number state vars.
    Allows running two sequences in parallel
*/
void swaprand(void);

#endif
