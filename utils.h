#ifndef UTILS_H
#define UTILS_H

namespace RNG {
	uint64_t seed = 0x1234567890ABCDEF;

	inline uint64_t xorshift64star(uint64_t &seed)
	{
	    uint64_t x = seed;
	    x ^= x >> 12;
	    x ^= x << 25;
	    x ^= x >> 27;
	    seed = x;
	    return x * UINT64_C(0x2545f4914f6cdd1d);
	}

	inline uint64_t next_long() {
		return xorshift64star(seed);
	}
}

namespace Utils {
	size_t next_power_of_two(size_t n) {
		size_t p = 1;
		while (p < n) p <<= 1;
		return p;
	}

	inline size_t hash(size_t f) {
		f ^= f >> 33;
		f *= 0xff51afd7ed558ccdL;
		f ^= f >> 33;
		f *= 0xc4ceb9fe1a85ec53L;
		f ^= f >> 33;
		return f;
	}
}

#endif // UTILS_H