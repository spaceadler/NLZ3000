/* nlz.h: two ways to count the leading zeros of a 64-bit number.
 *
 * NLZ(x) is the number of 0 bits above the highest 1 bit:
 * NLZ(1) = 63, NLZ(2^63) = 0, and NLZ(0) = 64 by definition.
 */
#ifndef NLZ_H
#define NLZ_H

#include <stdint.h>

/* Software: test one bit at a time from bit 63 down, stop at the first 1. */
static inline unsigned nlz_loop(uint64_t x)
{
    unsigned n = 0u;
    uint64_t bit = UINT64_C(1) << 63;

    while ((bit != 0u) && ((x & bit) == 0u)) {
        n++;
        bit >>= 1;
    }
    return n;
}

/* Hardware: one count-leading-zeros instruction (CLZ on Arm, LZCNT on x86
 * with -march=native). __builtin_clzll(0) is undefined, so 0 is handled
 * here; both instructions return 64 for 0 anyway, so the check compiles away. */
static inline unsigned nlz_clz(uint64_t x)
{
    return (x == 0u) ? 64u : (unsigned)__builtin_clzll(x);
}

#endif
