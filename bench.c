/* bench.c: sum NLZ(i) for every i from 0 to N and time the loop.
 *
 * Built twice from the same file:
 *   default      -> nlz_loop (software, bit by bit)
 *   -DUSE_CLZ    -> nlz_clz  (one instruction)
 * The sum is printed so the compiler can't drop the work, and so the two
 * builds can be checked against each other and against test.c.
 */
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "nlz.h"

#ifndef N
#define N ((UINT64_C(1) << 30) - 1u)   /* 0 .. 2^30 - 1 */
#endif

#ifdef USE_CLZ
#define NLZ nlz_clz
#else
#define NLZ nlz_loop
#endif

static double now(void)
{
    struct timespec t;
    (void)clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + ((double)t.tv_nsec / 1e9);
}

int main(void)
{
    uint64_t sum = 0u;
    const double start = now();

    for (uint64_t i = 0u; i <= N; i++) {
        sum += NLZ(i);
    }

    const double seconds = now() - start;
    printf("Sum of leading zeros: %llu\n", (unsigned long long)sum);
    printf("Time: %.4f s\n", seconds);
    return 0;
}
