
// This is an add-on for gbdk's rand.s

#ifndef SWAPRAND_INCLUDE
#define SWAPRAND_INCLUDE

#include <types.h>

/** Swaps between primary and secondary random number state vars.
    Allows running two sequences in parallel
*/

#ifdef NINTENDO
	void swaprand(void); /* Non-banked */
#elif defined(GAMEGEAR)
	inline void swaprand(void) { }
#endif


#endif
