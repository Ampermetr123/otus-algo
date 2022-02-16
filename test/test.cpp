#include <gtest/gtest.h>
#include "rle.h"
#include <sstream>

using ss = std::stringstream;

class RLETest : public ::testing::Test {
protected:
    static inline const char* data[][2] = {
        { "abc", "\x03""abc"},
        { "aabc", "\x04""aabc"},
        { "abbc", "\x04""abbc"},
        { "abcc", "\x04""abcc"},
        { "aaaa", "\xFC""a"},
        { "abbb", "\x01""a""\xFD""b"},
        { "abbbcccd",  "\x01""a" "\xFD""b" "\xFD""c" "\x01""d"}
    };
    static inline const int data_sz = sizeof(data) / sizeof(data[0]);
};


TEST_F(RLETest, pack_simple) {
    for (int i = 0; i < data_sz; i++) {
        ss inp, out;
        inp << data[i][0];
        auto ret = RLE::pack(inp, out);
        EXPECT_TRUE(ret);
        EXPECT_EQ(out.str(), data[i][1]) << "with i="<<i;
    }
}


TEST_F(RLETest, unpack_simple) {
    for (int i = 0; i < data_sz; i++) {
        ss inp, out;
        inp << data[i][1];
        auto ret = RLE::unpack(inp, out);
        EXPECT_TRUE(ret);
        EXPECT_EQ(out.str(), data[i][0]) << "with i="<<i;
    }
}


TEST_F(RLETest, pack_unpack) {
    ss inp, temp, out;
    inp << "00000000001111111111222222222223333333333444444444455555555556666666666777777777788888888889999999999"
        << "00000000001111111111222222222223333333333444444444455555555556666666666777777777788888888889999999999"
        << "00000000001111111111222222222223333333333444444444455555555556666666666777777777788888888889999999999"
        << "Alice was beginning to get very tired of sitting by her sister \n on the bank, and of having nothing to do:  once or twice she had \n"
        << "peeped into the book her sister was reading, but it had no pictures or conversations in it, `and what is the use of a book, '\n"
        << "thought Alice `without pictures or conversation?";
    auto ret = RLE::pack(inp, temp);
    EXPECT_TRUE(ret);
    ret = RLE::unpack(temp, out);
    EXPECT_TRUE(ret);
    ASSERT_EQ(inp.str(), out.str());
}
