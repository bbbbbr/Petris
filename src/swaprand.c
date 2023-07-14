
#include <gbdk/platform.h>
#include <stdint.h>
#include <rand.h>

uint16_t rand_alt = 0u;
uint16_t rand_temp;

/** Swaps between primary and secondary random number state vars.
    Allows running two sequences in parallel
*/
void swaprand(void) {
	rand_temp = __rand_seed;
	__rand_seed = rand_alt;
	rand_alt = rand_temp;
}
	
