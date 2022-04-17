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

    //  not compile for const_operator
    // *it = "1000+";
    // EXPECT_EQ(loc[1000], "1000+");

    it++;
    EXPECT_EQ(it, end);
}




//   HashTable<int, std::string, simple_hash_code > ht;
//   ASSERT_EQ(ht.get_buckets_count(), 0);
//   ASSERT_EQ(ht.size(), 0);
//   ASSERT_FLOAT_EQ(ht.get_load_factor(), 0);

//   auto result = ht.insert(1, "one");
//   ASSERT_TRUE(result);
//   ASSERT_EQ(ht.get_buckets_count(), 1) << "must be 1 bucket after insertion 1 element";
//   result = ht.insert(2, "two");
//   ASSERT_TRUE(result);
//   ASSERT_EQ(ht.get_buckets_count(), 2) << "must be 2 bucket after insertion 2 element";

//   ASSERT_EQ(ht.size(), 2)<<"size must be 2 after insertion 2 elements";

//   EXPECT_FLOAT_EQ(ht.get_load_factor(), 1) << "each element in own bucket, so load factor must be 1.0";

//   ht.insert(1 + int(ht.get_vector_size()) , "the same bucket as one");
//   EXPECT_FLOAT_EQ(ht.get_load_factor(), 1.5) << "2 elements in same bucket and 1 element in it's onw bucket - 1.5";

// }
