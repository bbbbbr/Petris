
#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#include "gbdk/platform.h"



// ;/***************************************************************************
// ; *                                                                         *
// ; * Module  : rand.s                                                        *
// ; *                                                                         *
// ; * Purpose : A rand() generator using the linear congruential method       *
// ; *                                                                         *
// ; * Version : 1.01, January 7 1998                                          *
// ; *             Added _initrand to set seed without recompiling             *
// ; *           1, January 6 1998                                             *
// ; *                                                                         *
// ; * Author  : Luc Van den Borre ( Homepage : NOC.BASE.ORG )                 *
// ; *                                                                         *
// ; **************************************************************************/

//  Why use an algorithm for generating random numbers?
// 
//  - Given a certain seed value, the same sequence of random numbers is generated
//    every time. This is a good thing when debugging (reproducible). On the other
//    hand, you've got 2^16 seed values, each of which will produce a sequence of
//    numbers that stays different for any of the other sequences for 'an appreciable
//    time.' (I can't say how long exactly.)
// 
//  - The linear congruential method is one of the 'best' random number generators
//    around. However, this implementation uses a 16 bit accumulator, while at least
//    32 bits are needed for a generator that passes all the statistical tests.
//    Still, I'm relatively confident that this is random enough for even the most
//    demanding game.

//  Random number generator using the linear congruential method
//   X(n+1) = (a*X(n)+c) mod m
//  with a = 17, m = 65536 and c = $5c93 (arbitrarily)
//  The seed value is also chosen arbitrarily as $a27e
//  Ref : D. E. Knuth, "The Art of Computer Programming" , Volume 2

uint16_t __rand_seed;


uint16_t rand(void) {  // TODO: test output

    __rand_seed = (__rand_seed * 17) + 0x5C93u;

    // Note D is the low byte,E the high byte. This is intentional because
    // the high byte can be slightly 'more random' than the low byte, and I presume
    // most will cast the return value to a uint8_t.
    return ((__rand_seed >> 8) | (__rand_seed << 8));
}

uint16_t initrand(uint16_t seed) {
    __rand_seed = seed;
}
