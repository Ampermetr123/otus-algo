#include "rle.h"
#include <vector>
#include <iostream>


void RLE::flush_unique(std::vector<char>& v, std::ostream& os) {
    auto size = static_cast<const char>(v.size());
    if (size == 0) {
        return;
    }
    os.write(&size, 1);
    os.write(&v[0], size);
    v.clear();
}


void RLE::flush_repeated(const char ch, const char sz, std::ostream& os) {
    char t = -sz;
    os.write(&t, 1);
    os.write(&ch, 1);
}


bool RLE::pack(std::istream& is, std::ostream& os) {
    std::vector<char> mem;
    mem.reserve(max_sz);
    char ch = 0, rep_count = 0, rep_ch = 0;
    auto state = empty;

    while (is.read(&ch, 1).good()) {
        if (os.fail()) {
            return false;
        }

        if (state == empty) {
            mem.push_back(ch);
            state = less2rep;
            continue;
        }

        else if (state == less2rep) {
            if (mem.back() == ch) {
                state = eq2rep;
            }
            mem.push_back(ch);
            if (mem.size() == max_sz) {
                flush_unique(mem, os);
                state = empty;
                continue;
            }
        }

        else if (state == eq2rep) {
            if (mem.back() == ch) {
                mem.pop_back();
                mem.pop_back();
                if (mem.size()) {
                    flush_unique(mem, os);
                    state = empty;
                }
                rep_count = 3;
                rep_ch = ch;
                state = more2rep;
            }
            else {
                mem.push_back(ch);
                if (mem.size() == max_sz) {
                    flush_unique(mem, os);
                    state = empty;
                    continue;
                }
                state = less2rep;
            }

        }

        else if (state == more2rep) {
            if (ch == rep_ch) {
                rep_count += 1;
                if (rep_count == max_sz) {
                    flush_repeated(rep_ch, rep_count, os);
                    state = empty;
                    continue;
                }
            }
            else {
                flush_repeated(rep_ch, rep_count, os);
                mem.push_back(ch);
                state = less2rep;
            }
        }
    }// while

    if (state == more2rep) {
        flush_repeated(rep_ch, rep_count, os);
    }
    else if (mem.size()) {
        flush_unique(mem, os);
    }
    return os.good() && is.eof();
}


bool RLE::unpack(std::istream& is, std::ostream& os) {
    char ch;
    char buf[max_sz];

    while (is.read(&ch, 1).good()) {
        if (ch > 0) {
            is.read(buf, ch);
            os.write(buf, ch);
        }
        else if (ch < 0) {
            is.read(buf, 1);
            while (ch != 0) {
                os.write(buf, 1);
                ch++;
            }
        }
    }
    return os.good() && is.eof();
}
