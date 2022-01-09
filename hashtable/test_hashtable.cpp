#include <gtest/gtest.h>

#include "hash_table.h"
#include <string>


struct simple_hash_code {
  int operator()(int i) const {
    return i % 10;
  }
};


// Проверка исходного состояния, вставки и счетчиков (без рехеширования)
TEST(HashTableTest, BasicAssertions) {
  HashTable<int, std::string, simple_hash_code > ht;
  ASSERT_EQ(ht.get_buckets_count(), 0);
  ASSERT_EQ(ht.size(), 0);
  ASSERT_FLOAT_EQ(ht.get_load_factor(), 0);

  auto result = ht.insert(1, "one");
  ASSERT_TRUE(result);
  ASSERT_EQ(ht.get_buckets_count(), 1) << "must be 1 bucket after insertion 1 element";
  result = ht.insert(2, "two");
  ASSERT_TRUE(result);
  ASSERT_EQ(ht.get_buckets_count(), 2) << "must be 2 bucket after insertion 2 element";

  ASSERT_EQ(ht.size(), 2)<<"size must be 2 after insertion 2 elements";

  EXPECT_FLOAT_EQ(ht.get_load_factor(), 1) << "each element in own bucket, so load factor must be 1.0";

  ht.insert(1 + int(ht.get_vector_size()) , "the same bucket as one");
  EXPECT_FLOAT_EQ(ht.get_load_factor(), 1.5) << "2 elements in same bucket and 1 element in it's onw bucket - 1.5";

}


// Проверка работы итератора 
TEST(HashTableTest, Iterations) {
  using HT = HashTable<int, std::string, simple_hash_code >;
  HT ht;
  using KeyVal = HT::KeyVal;

  ht.insert(1, "one");
  ht.insert(2, "two");
  ht.insert(3, "three");
  ht.insert(11, "eleven");
  ASSERT_TRUE(ht.begin() != ht.end());

  auto it = ht.begin();
  EXPECT_EQ(*it, KeyVal(11, "eleven")) << "1st element";; 
  ++it;
  EXPECT_EQ(*it, KeyVal(1, "one")) << "2nd element";;
  ++it;
  EXPECT_EQ(*it, KeyVal(2, "two")) << "3rd element";;
  ++it;
  EXPECT_EQ(*it, KeyVal(3, "three")) << "4th element";;
  ++it;
  EXPECT_FALSE(it != ht.end());

}

// Проверка функций доступа и поиска
TEST(HashTableTest, Access) {
  using HT = HashTable<int, std::string, simple_hash_code >;
  HT ht;
  using KeyVal = HT::KeyVal;

  ht.insert(1, "one");
  ht.insert(2, "two");
  ht.insert(3, "three");
  ht.insert(11, "eleven");

  EXPECT_TRUE(ht.find(1));
  EXPECT_TRUE(ht.find(2));
  EXPECT_TRUE(ht.find(3));
  EXPECT_TRUE(ht.find(11));

  EXPECT_FALSE(ht.find(21));
  EXPECT_FALSE(ht.find(5));

  ht.at(1) = "really one";
  ASSERT_EQ(ht.at(1), "really one");
  ASSERT_EQ(ht.at(2), "two");
  ASSERT_EQ(ht.at(3), "three");
  ASSERT_EQ(ht.at(11), "eleven");

  ASSERT_NO_THROW(ht.erase(11)) << "while erase existing element";
  ASSERT_THROW(ht.erase(11), std::out_of_range) << "while erase non-existing element";

  ASSERT_THROW(ht.at(123), std::out_of_range);
  ASSERT_THROW(ht.at(11), std::out_of_range);
}


// Проерка основных операций с рехешированием.
TEST(HashTableTest, TestWithRehashing) {
  using HT = HashTable<int, std::string>;
  HT ht;
  int SZ = HT::initial_buckets_size * 25;

  // Adding big amount of elements cause rehashing
  for (auto i = 0; i < SZ; i++) {
    ht.insert(i, std::to_string(i));
  }

  EXPECT_LT(ht.get_load_factor(), HT::max_load_factor) << "while check load factor is ok du to rehashing";

  // Checking all alements are presents
  for (auto i = 0; i < SZ; i++) {
    ASSERT_TRUE(ht.find(i)); 
  }
  for (auto i = 0; i < SZ; i++) {
    ASSERT_EQ(ht.at(i), std::to_string(i));
  }

  ASSERT_THROW(ht.at(SZ + 13), std::out_of_range);
  EXPECT_FALSE(ht.find(SZ));

  // Erase each element
  for (auto i = 0; i < SZ; i++) {
    ASSERT_NO_THROW(ht.erase(i));
  }

  ASSERT_EQ(ht.size(), 0);
  ASSERT_EQ(ht.get_buckets_count(), 0);
  ASSERT_FLOAT_EQ(ht.get_load_factor(), 0);
}