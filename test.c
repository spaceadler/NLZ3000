/* test.c: checks nlz_loop and nlz_clz against each other and against a
 * closed form, so the benchmark's sum can be trusted without running it.
 */
#include <stdio.h>
#include <stdint.h>
#include "nlz.h"

static unsigned failures = 0u;

static void check(int ok, const char *what, unsigned long long a, unsigned long long b)
{
    if (!ok) {
        failures++;
        printf("FAIL %s: got %llu, expected %llu\n", what, a, b);
    }
}

/* Sum of NLZ(i) for i = 0..n, by counting how many i share each highest bit:
 * the 2^k numbers in [2^k, 2^(k+1) - 1] all have NLZ = 63 - k. */
static uint64_t expected_sum(uint64_t n)
{
    uint64_t sum = 64u;                       /* i = 0 */
    for (unsigned k = 0u; k < 64u; k++) {
        const uint64_t lo = UINT64_C(1) << k;
        if (lo > n) {
            break;
        }
        uint64_t hi = (k == 63u) ? UINT64_MAX : ((lo << 1) - 1u);
        if (hi > n) {
            hi = n;
        }
        sum += (hi - lo + 1u) * (63u - k);
    }
    return sum;
}

int main(void)
{
    /* Edge cases: 0, all-ones, and every power of two and its neighbours. */
    check(nlz_loop(0u) == 64u && nlz_clz(0u) == 64u, "NLZ(0)", nlz_clz(0u), 64u);
    check(nlz_loop(UINT64_MAX) == 0u && nlz_clz(UINT64_MAX) == 0u, "NLZ(max)", nlz_clz(UINT64_MAX), 0u);
    for (unsigned k = 0u; k < 64u; k++) {
        const uint64_t p = UINT64_C(1) << k;
        check(nlz_loop(p) == 63u - k, "loop 2^k", nlz_loop(p), 63u - k);
        check(nlz_clz(p) == 63u - k, "clz 2^k", nlz_clz(p), 63u - k);
        check(nlz_loop(p - 1u) == nlz_clz(p - 1u), "2^k - 1", nlz_clz(p - 1u), nlz_loop(p - 1u));
        check(nlz_loop(p + 1u) == nlz_clz(p + 1u), "2^k + 1", nlz_clz(p + 1u), nlz_loop(p + 1u));
    }

    /* A million pseudo-random values (xorshift64), spread over all bit lengths. */
    uint64_t s = UINT64_C(0x9E3779B97F4A7C15);
    for (unsigned i = 0u; i < 1000000u; i++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        const uint64_t x = s >> (i % 64u);
        check(nlz_loop(x) == nlz_clz(x), "random", nlz_clz(x), nlz_loop(x));
    }

    /* Brute-force sums against the closed form, up to 2^20. */
    uint64_t sum_loop = 0u, sum_clz = 0u;
    for (uint64_t i = 0u; i <= (UINT64_C(1) << 20); i++) {
        sum_loop += nlz_loop(i);
        sum_clz += nlz_clz(i);
        if ((i & (i + 1u)) == 0u || i == 1000u || i == 999999u) {   /* 2^m - 1 and a few others */
            check(sum_loop == expected_sum(i), "sum loop", sum_loop, expected_sum(i));
            check(sum_clz == expected_sum(i), "sum clz", sum_clz, expected_sum(i));
        }
    }

    /* For N = 2^m - 1 the closed form simplifies to (65 - m) * 2^m - 1. */
    for (unsigned m = 1u; m < 40u; m++) {
        const uint64_t n = (UINT64_C(1) << m) - 1u;
        const uint64_t simple = ((UINT64_C(65) - m) << m) - 1u;
        check(expected_sum(n) == simple, "(65-m)2^m-1", expected_sum(n), simple);
    }

    printf("bench.c default (N = 2^30 - 1) should print: %llu\n",
           (unsigned long long)expected_sum((UINT64_C(1) << 30) - 1u));
    if (failures == 0u) {
        printf("all checks passed\n");
        return 0;
    }
    printf("%u checks failed\n", failures);
    return 1;
}
