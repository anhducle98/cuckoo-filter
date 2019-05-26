#include "cuckoofilter.h"

#include <bits/stdc++.h>

using namespace std;

void stress_test_all() {
	int total_items = int(1 << 20) * 0.95;
	CuckooFilter<uint64_t, std::hash<uint64_t>, 4, 16> cf(total_items);
	set<uint64_t> S;

	for (int i = 0; i < total_items; ++i) {
		uint64_t x = RNG::next_long();
		S.insert(x);
	}

	clock_t checkpoint1 = clock();
	for (auto x : S) {
		assert(cf.insert(x));
	}

	clock_t checkpoint2 = clock();
	for (auto x : S) {
		assert(cf.lookup(x));
	}
	clock_t checkpoint3 = clock();

	int num_rand_queries = int(1e7);
	int sum = 0;
	for (int i = 0; i < num_rand_queries; ++i) {
		sum += cf.lookup(RNG::next_long());
	}
	clock_t checkpoint4 = clock();

	int wrong_count = 0;
	int num_queries = int(1e6);
	for (int i = 0; i < num_queries; ++i) {
		uint64_t x = RNG::next_long();
		wrong_count += S.count(x) != cf.lookup(x);
	}

	cerr << "wrong_count = " << wrong_count << endl;
	cerr << "error_rate = " << 100.0 * wrong_count / num_queries << "%" << endl;
	cerr << "insert_time = " << double(checkpoint2 - checkpoint1) / CLOCKS_PER_SEC << endl;
	cerr << "postive_lookup_time = " << double(checkpoint3 - checkpoint2) / CLOCKS_PER_SEC << endl;
	cerr << "random lookup speed = " << num_rand_queries / (double(checkpoint4 - checkpoint3) / CLOCKS_PER_SEC) << " queries/sec" << endl;
	cerr << "sum = " << sum << endl;
}


void stress_speed() {
	clock_t checkpoint1 = clock();
	CuckooFilter<uint64_t> cf(int(1 << 20));

	int num_elements = int(cf.a.size() * 4 * 0.95);
	for (int i = 0; i < num_elements; ++i) {
		cf.insert(RNG::next_long());
	}
	clock_t checkpoint2 = clock();
	int num_rand_queries = int(1e7);
	int sum = 0;
	for (int i = 0; i < num_rand_queries; ++i) {
		sum += cf.lookup(RNG::next_long());
	}
	clock_t checkpoint3 = clock();

	cerr << "num_elements = " << num_elements << endl;
	cerr << "insert_time = " << double(checkpoint2 - checkpoint1) / CLOCKS_PER_SEC << endl;
	cerr << "lookup_time = " << double(checkpoint3 - checkpoint2) / CLOCKS_PER_SEC << endl;
	cerr << "lookup speed = " << setprecision(2) << fixed << num_rand_queries / (double(checkpoint3 - checkpoint2) / CLOCKS_PER_SEC) << " queries/sec" << endl;
	cerr << "sum = " << sum << endl;
}

void test_string() {
	CuckooFilter<string> cf(4);
	cf.insert("le");
	cf.insert("anh");
	cf.insert("duc");
	cf.insert("viet");
	cf.insert("name");
	assert(cf.lookup("duc"));
	assert(cf.lookup("viet"));
	assert(!cf.lookup("hust"));
	cf.erase("viet");
	assert(!cf.lookup("viet"));
}

int main() {
	stress_test_all();
	//stress_speed();
	//test_string();
	return 0;
}