/**
 * @file lcg64.hpp
 * @brief Linear Congruential Generators.
 * Useful info can be found at http://numbercrunch.de/trng/
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdint>

#define A_Default (18145460002477866997ull)
#define A_LEcuyer1 (2862933555777941757ull)
#define A_LEcuyer2 (3202034522624059733ull)
#define A_LEcuyer3 (3935559000370003845ull)

static uint64_t __lcg64_r = 0;

inline uint64_t lcg64(void)
{
	uint64_t * const r = &__lcg64_r;
	const uint64_t a = A_Default;
	const uint64_t b = 1;
	(*r) = ((*r)*a + b);
	return (*r);
}

inline void lcg64(uint64_t seed)
{
	__lcg64_r = seed;
}

