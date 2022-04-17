#include "location.h"
#include "optlog.h"
#include "a.h"
#include <cassert>
#include "cuckoo_hashtable.h"
#include <string>
#include <functional>
#include <unordered_map>

struct MyStringHash {
    int seed;
    MyHash(int seed):seed(seed) {}
    std::size_t operator()(const std::string& s) const noexcept
    {
        std::string ss;
        size_t i = 0;
        ss.reserve(s.length());
        for (const auto& ch : s) {
            i = (i + 1) % 24;
            ss.push_back(ch ^ (seed>>i));
        }
        return std::hash<std::string>{}(ss);
    }
};

size_t h1(size_t x) { return x % 11; }
size_t h2(size_t x) { return (x / 11) % 11; }



int main(int, char**) {

    lg0 << "Hello" << std::endl;
    const size_t N = 2;
    std::array< decltype(&h1) ,  N> hash_funcs = { &h1, &h2 };
    CuckooHashTable<int, int, decltype(&h1), N> ht(hash_funcs);
    return 0;
}
