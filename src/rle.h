#pragma once
#include <iostream>
#include <vector>

class RLE {
private:
    enum {
        empty, less2rep, eq2rep, more2rep
    };

    static inline const size_t max_sz = 127;
    static void flush_unique(std::vector<char>& v, std::ostream& os);
    static void flush_repeated(const char ch, const char sz, std::ostream& os);

public:
    static bool RLE::pack(std::istream& is, std::ostream& os);
    static bool RLE::unpack(std::istream& is, std::ostream& os);

};
