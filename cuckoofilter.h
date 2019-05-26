#ifndef CUCKOO_FILTER_H
#define CUCKOO_FILTER_H

#include <vector>
#include <type_traits>
#include <array>
#include <algorithm>

#include "utils.h"

template < class T, class Hash = std::hash<T>, size_t BUCKET_SIZE = 4, size_t FINGERPRINT_SIZE = 16 >
struct CuckooFilter {
	using fingerprint_t = typename std::conditional< FINGERPRINT_SIZE <= 8, uint8_t, uint16_t >::type;

	static const int MAX_NUM_KICKS = 1 << 9;
	static const fingerprint_t FINGERPRINT_MASK = (1 << FINGERPRINT_SIZE) - 1;
	
	std::vector< std::array< fingerprint_t, BUCKET_SIZE > > a;
	size_t mask;

	CuckooFilter(size_t num_items) {
		size_t num_buckets = Utils::next_power_of_two(num_items / BUCKET_SIZE);
		a.resize(num_buckets);
		mask = num_buckets - 1;
	}

	inline size_t get_hash_fingerprint(fingerprint_t f) {
		return Utils::hash(f);
	}

	inline void get_fingerprint_and_hash(const T &x, fingerprint_t &f, size_t &i) {
		size_t h = Hash{}(x);
		f = h % FINGERPRINT_MASK + 1;
		i = Utils::hash(h);
	}

	inline bool try_insert(size_t i, fingerprint_t f) {
		i &= mask;
		for (size_t j = 0; j < BUCKET_SIZE; ++j) {
			if (!a[i][j]) return a[i][j] = f, true;
		}
		return false;
	}

	inline bool try_erase(size_t i, fingerprint_t f) {
		i &= mask;
		for (size_t j = 0; j < BUCKET_SIZE; ++j) {
			if (a[i][j] == f) return a[i][j] = 0, true;
		}
		return false;
	}

	inline bool bucket_contains(size_t i, fingerprint_t f) {
		i &= mask;
		for (size_t j = 0; j < BUCKET_SIZE; ++j) {
			if (a[i][j] == f) return true;
		}
		return false;
	}

	bool insert(const T &x) {
		fingerprint_t f; size_t i;
		get_fingerprint_and_hash(x, f, i);
		if (try_insert(i, f) || try_insert(i ^ get_hash_fingerprint(f), f)) return true;
		for (int it = 0; it < MAX_NUM_KICKS; ++it) {
			std::swap(f, a[i & mask][RNG::next_long() % BUCKET_SIZE]);
			if (try_insert(i ^= get_hash_fingerprint(f), f)) return true;
		}
		return false;
	}

	bool lookup(const T &x) {
		fingerprint_t f; size_t i;
		get_fingerprint_and_hash(x, f, i);
		return bucket_contains(i, f) || bucket_contains(i ^ get_hash_fingerprint(f), f);
	}

	bool erase(const T &x) {
		fingerprint_t f; size_t i;
		get_fingerprint_and_hash(x, f, i);
		return try_erase(i, f) || try_erase(i ^ get_hash_fingerprint(f), f);
	}
};

#endif // CUCKOO_FILTER_H