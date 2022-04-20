#include <gtest/gtest.h>
#include <string>
#include "location.h"
#include "bclass.h"
#include <string>

// Проверка того, что не вызываются конструкторы объектов
TEST(Test_Location, test_no_constuction) {
    B::reset();
    Location<B> loc(10);
    EXPECT_EQ(B::constuctor_count(), 0);
    ASSERT_EQ(loc.count(), 0);
}


TEST(Test_Location, test_emplace) {
    B::reset();
    {
        Location<B> loc(10);
        loc.emplace(2, 123);
        EXPECT_EQ(loc[2].id, 123);
        EXPECT_EQ((*loc.get(2)).id, 123);
        EXPECT_TRUE(loc.persist(2));
        EXPECT_FALSE(loc.persist(0));
        EXPECT_EQ(loc.count(), 1);
    }
    ASSERT_EQ(B::constuctor_count(), 1);
    ASSERT_EQ(B::destructor_count(), 1);
}


TEST(Test_Location, test_set) {
    B::reset();
    {
        Location<B> loc(10);
        loc.set(2, B(123));
        EXPECT_EQ(loc[2].id, 123);
        EXPECT_EQ((*loc.get(2)).id, 123);
        EXPECT_TRUE(loc.persist(2));
        EXPECT_FALSE(loc.persist(0));
        EXPECT_EQ(loc.count(), 1);

    }
    ASSERT_EQ(B::constuctor_count(), 2);
    ASSERT_EQ(B::destructor_count(), 2);
}


TEST(Test_Location, test_erase) {
    B::reset();
    Location<B> loc(10);

    loc.emplace(2, 2);
    EXPECT_TRUE(loc.persist(2));
    loc.emplace(4, 4);
    EXPECT_TRUE(loc.persist(4));
    EXPECT_EQ(B::constuctor_count(), 2);
    EXPECT_EQ(B::destructor_count(), 0);
    EXPECT_EQ(loc.count(), 2);


    loc.erase(loc.begin());
    EXPECT_EQ(B::destructor_count(), 1);
    EXPECT_EQ(loc.count(), 1);

    loc.erase(4);
    EXPECT_EQ(B::destructor_count(), 2);
    EXPECT_EQ(loc.count(), 0);
}


TEST(Test_Location, test_count) {
    B::reset();
    Location<B> loc(10);
    loc.set(2, B(120));
    loc.set(2, B(122));
    loc.emplace(2, 123);
    loc.emplace(4, 19);
    loc.emplace(5, 21);
    EXPECT_TRUE(loc.persist(2));
    EXPECT_TRUE(loc.persist(4));
    EXPECT_TRUE(loc.persist(5));
    EXPECT_FALSE(loc.persist(1));
    ASSERT_EQ(loc.count(), 3);
}


TEST(Test_Location, test_iterator) {
    Location<std::string> loc(1024);
    std::string thousand("1000");
    loc.set(800, "800");
    loc.emplace(5, "5");
    loc.set(1000, thousand);
    loc.emplace(300, "300");

    auto it = loc.begin();
    auto end = loc.end();
    EXPECT_TRUE(it != end);
    EXPECT_EQ(*it, "5");
    ++it;
    EXPECT_EQ(*it, "300");
    EXPECT_EQ(*it++, "300");
    EXPECT_EQ(*it, "800");
    it++;
    EXPECT_STRCASEEQ(it->c_str(), "1000");
    *it = "1000+";
    EXPECT_EQ(loc[1000], "1000+");
    it++;
    EXPECT_EQ(it, end);
}

TEST(Test_Location, test_const_iterator) {
    Location<std::string> loc(1024);
    std::string thousand("1000");
    loc.set(800, "800");
    loc.emplace(5, "5");
    loc.set(1000, thousand);
    loc.emplace(300, "300");

    auto it = loc.cbegin();
    const auto end = loc.end();  // cust iterator to const_iterator
    EXPECT_TRUE(it != end);
    EXPECT_EQ(*it, "5");
    ++it;
    EXPECT_EQ(*it, "300");
    EXPECT_EQ(*it++, "300");
    EXPECT_EQ(*it, "800");
    it++;
    EXPECT_STRCASEEQ(it->c_str(), "1000");
    it++;
    EXPECT_EQ(it, end);
}
