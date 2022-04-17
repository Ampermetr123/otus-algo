#include <gtest/gtest.h>

#include "cuckoo_hashtable.h"
#include <string>
#include <array>
#include <set>
#include "location.h"


static size_t h1(size_t x) { return x % 11; }
static size_t h2(size_t x) { return (x / 11) % 11; }

static size_t h11(size_t x) { return x % 15; }
static size_t h22(size_t x) { return (x / 15) % 15; }


TEST(Test_CuckooHT, const_iterator) {
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  ASSERT_EQ(ht.cbegin(), ht.cend());
  ht.insert({ 2, 2 });
  ht.insert({ 9, 9 });
  ASSERT_TRUE(ht.cbegin() != ht.cend());
  auto it = ht.cbegin();
  ASSERT_EQ(it->first, 2);
  ++it;
  ASSERT_EQ(it->first, 9);
  it++;
  ASSERT_EQ(it, ht.cend());
}

TEST(Test_CuckooHT, iterator) {
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  ASSERT_EQ(ht.begin(), ht.end());
  ht.insert({ 2, 2 });
  ht.insert({ 9, 9 });
  ASSERT_TRUE(ht.begin() != ht.end());
  auto it = ht.begin();
  ASSERT_EQ(it->first, 2);
  it->second = 43;
  //ASSERT_EQ(ht[2], 43);
  ++it;
  ASSERT_EQ(it->first, 9);
  it++;
  ASSERT_EQ(it, ht.end());
}

TEST(Test_CuckooHT, test_insert_access) {
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  const auto& [it, b] = ht.insert({ 2, 22 });
  ASSERT_TRUE(b);
  std::pair<const int, int> x(2, 22);
  ASSERT_EQ(*it, x);
  ASSERT_EQ(ht[2], 22);
  ht[1] = 11;
  ASSERT_EQ(ht[1], 11);

  {
    const auto& [it, b] = ht.insert({ 1, 0 });
    ASSERT_FALSE(b);
    ASSERT_EQ(ht[1], 11);
  }

  ASSERT_TRUE(ht.contains(1));
  ASSERT_FALSE(ht.contains(5));
}

TEST(Test_CuckooHT, test_erase) {
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  std::set keys = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 };
  std::set keys_after = { 20, 75, 100, 67, 105, 3 }; // -50, 53, 36, 39
  for (const auto& k : keys) {
    ht.insert({ k, k * k });
  }
  ASSERT_EQ(keys.size(), ht.size());
  ht.erase(50);
  ht.erase(53);
  ht.erase(ht.find(36));
  ht.erase(39);
  ASSERT_EQ(keys_after.size(), ht.size());

  std::set<int> res;
  for (const auto& [k, v] : ht) {
    res.insert(k);
  }
  ASSERT_EQ(res, keys_after);
}

TEST(Test_CuckooHT, test_size) {
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  std::array keys = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 };
  for (auto k : keys) {
    ht.insert({ k, k * k });
  }
  ASSERT_EQ(keys.size(), ht.size());
}

TEST(Test_CuckooHT, test_classic_layout) {
  CuckooHashTable<int, int, decltype(&h1), 2, CHT_Traits_Classic> ht({ &h1, &h2 });
  std::vector<int> keys = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 };
  for (auto k : keys) {
    ht.insert({ k, k * k });
  }

  // example on https://ru.wikipedia.org/wiki/%D0%9A%D1%83%D0%BA%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%BE_%D1%85%D0%B5%D1%88%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5
  std::vector<int> classic_layout = { 100, 36, 50, 75, 3, 20, 39, 53, 67, 105 };
  auto correct_it = classic_layout.begin();
  for (auto it = ht.begin(); it != ht.end(); ++it) {
    ASSERT_TRUE(correct_it != classic_layout.end());
    auto v = *correct_it;
    std::pair<const int, int> kv(v, v * v);
    EXPECT_EQ(*it, kv);
    ++correct_it;
  }
  EXPECT_FALSE(correct_it != classic_layout.end());
}

TEST(Test_CuckooHT, test_greed) {
  CuckooHashTable<int, int, decltype(&h1), 2, CHT_Traits_Greed> ht({ &h1, &h2 });
  std::vector<int> keys = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 };
  for (auto k : keys) {
    const auto& [it, res] = ht.insert({ k, k * k });
    ASSERT_TRUE(it != ht.end()) << " with key " << k;
    ASSERT_TRUE(res);
  }
  EXPECT_EQ(ht.size(), keys.size());
  for (auto k : keys) {
    ASSERT_EQ(ht[k], k * k);
    ASSERT_TRUE(ht.contains(k));
    ASSERT_TRUE(ht.find(k) != ht.end());
  }
}

TEST(Test_CuckooHT, test_rehash_limit) {
  // Плохие хеш функции - увеличение таблиц не решает проблемы
  CuckooHashTable<int, int, decltype(&h1), 2> ht({ &h1, &h2 });
  std::array keys = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39, 1, 11 };

  for (auto k : keys) {
    const auto& [it, b] = ht.insert({ k, k * k });
    if (it == ht.end()) {
      SUCCEED();
      return;
    }
  }
  FAIL() << "rehash fail excepted ";
}

TEST(Test_CuckooHT, test_rehash_classic) {
  CuckooHashTable<int, int, decltype(&h11), 2> ht({ &h11, &h22 });
  std::array keys =
  { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 , 15, 13, 31, 33, 29, 18, 65, 78 };
  ASSERT_EQ(ht.get_rehash_counter(), 0);
  for (auto k : keys) {
    const auto& [it, b] = ht.insert({ k, k * k });
    ASSERT_TRUE(it != ht.end()) << "with key=" << k;
    ASSERT_TRUE(b) << "with key=" << k;
  }
  ASSERT_EQ(ht.size(), keys.size());
  ASSERT_EQ(ht.get_rehash_counter(), 1);
  EXPECT_EQ(ht.size(), keys.size());
  for (const auto& [k, v] : ht) {
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), k) != keys.end());
    ASSERT_EQ(k * k, v);
  }
}

TEST(Test_CuckooHT, test_rehash_greed) {
  CuckooHashTable<int, int, decltype(&h11), 2, CHT_Traits_Greed> ht({ &h11, &h22 });
  std::array keys =
  { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 , 15, 13, 31, 33, 29, 18, 65, 78 };
  ASSERT_EQ(ht.get_rehash_counter(), 0);
  for (auto k : keys) {
    const auto& [it, b] = ht.insert({ k, k * k });
    ASSERT_TRUE(it != ht.end()) << "with key=" << k;
    ASSERT_TRUE(b) << "with key=" << k;
  }
  ASSERT_EQ(ht.size(), keys.size());
  ASSERT_EQ(ht.get_rehash_counter(), 1);
  EXPECT_EQ(ht.size(), keys.size());
  for (const auto& [k, v] : ht) {
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), k) != keys.end());
    ASSERT_EQ(k * k, v);
  }
}
