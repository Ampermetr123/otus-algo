#include "location.h"
#include "optlog.h"
#include <cassert>
#include "cuckoo_hashtable.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "profiler.h"






struct MyStringHash {
    int seed;
    MyStringHash(int seed = 344323) :seed(seed) {}
    std::size_t operator()(const std::string& s) const noexcept {
        std::string ss;
        size_t i = 0;
        ss.reserve(s.length());
        for (const auto& ch : s) {
            i = (i + 1) % 24;
            ss.push_back(ch ^ char(seed >> i));
        }
        return std::hash<std::string>{}(ss);
    }
};

using value_t = std::array<size_t, 512>;

void load_data(std::string file_path, std::vector<std::string>& v) {
    std::ifstream f(file_path);
    if (!f) {
        std::cout << "File " << file_path << " not found " << std::endl;
        return;
    }
    std::string word;
    while (f >> word) {
        v.emplace_back(std::move(word));
    }
}


template <typename HASHTABLE, bool CHT = true>
bool test_task_insert(std::vector<std::string>& input, HASHTABLE& ht) {
    [[maybe_unused]] size_t umap_rehash_counter = 0;
    [[maybe_unused]] size_t umap_bucket_counter = 0;
    if constexpr (!CHT) {
        umap_bucket_counter = ht.bucket_count();
    }
    size_t word_counter = 0;
    {
        LogDuration ld([](auto ms) {std::cout << "Building dictionary time: " << ms << " ms" << std::endl;  });
        for (auto& word : input) {
            if (ht.count(word)) {
                ht[word][0] += 1;
            }
            else {

                const auto& [it, b] = ht.insert({ word, value_t()});
                word_counter++;
                if (b == false || it == ht.end()) {
                    std::cout << "Error" << std::endl;
                    std::cerr << "have ht.size = " << ht.size() << " word #" << word_counter << " - " << word;
                    return false;
                }

                if constexpr (!CHT) {
                    if (ht.bucket_count() - umap_bucket_counter > 1) {
                        umap_rehash_counter++;
                    };
                    umap_bucket_counter = ht.bucket_count();
                }
            }
        }
    }

    std::cout << "Total words: " << input.size() << std::endl;
    std::cout << "Hash table size: " << ht.size() << std::endl;
    if constexpr (CHT) {
        std::cout << "Hash table load: " << ht.get_load() << std::endl;
        std::cout << "Rehash times: " << ht.get_rehash_counter() << std::endl;
    }
    else {
        std::cout << "Load factor: " << ht.load_factor() << std::endl;
        std::cout << "Rehash times: " << umap_rehash_counter << std::endl;
    }

     std::cout << "'war' count: " << ht[std::string("War")][0] + ht[std::string("war")][0] << "; ";
    // std::cout << "'peace' count: " << ht[std::string("Peace")] + ht[std::string("peace")] << std::endl;

    {
        LogDuration ld([](auto ms) {std::cout << "Acces time: " << ms << " ms" << std::endl;  });
        value_t tmp;
        for (auto& word : input) {
            tmp = ht[word];
            tmp[0]++;
        }
    }

    std::cout << "----------------------" << std::endl;
    return true;
}


int main(int, char**) {

    std::vector<std::string> data;
    data.reserve(600000);
    load_data("war_and_peace.txt", data);
    load_data("alice.txt", data);

    [[maybe_unused]] std::array hash_funcs_2 = { MyStringHash(344323), MyStringHash(531954) };
    [[maybe_unused]] std::array hash_funcs_3 = { MyStringHash(344323), MyStringHash(531954), MyStringHash(1234567) };
    [[maybe_unused]] std::array hash_funcs_4 = { MyStringHash(344323), MyStringHash(531954), MyStringHash(1234567), MyStringHash(7654321) };

    {
        std::cout << "\n------  std::unorederd_map ------- " << std::endl;
        using umap_t = std::unordered_map<std::string, value_t, MyStringHash>;
        umap_t umap;
        test_task_insert<umap_t, false>(data, umap);
    }
/***********************  E = 1 ****************************/
    {
        std::cout << "\n------  Cuckoo Hashtable Classic E=2 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 2, CHT_Traits_Classic> cht(hash_funcs_2);
        test_task_insert(data, cht);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable DFS E=2 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 2, CHT_Traits_Greed_DFS2> cht(hash_funcs_2);
        test_task_insert(data, cht);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable BFS E=2 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 2, CHT_Traits_Greed_BFS2> cht(hash_funcs_2);
        test_task_insert(data, cht);
    }
/***********************  E = 3 ****************************/
    {
        std::cout << "\n------  Cuckoo Hashtable Classic E=3 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 3, CHT_Traits_Classic> cht_3_classic(hash_funcs_3);
        test_task_insert(data, cht_3_classic);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable DFS E=3 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 3, CHT_Traits_Greed_DFS> cht_3_greed(hash_funcs_3);
        test_task_insert(data, cht_3_greed);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable BFS E=3 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 3, CHT_Traits_Greed_BFS> cht(hash_funcs_3);
        test_task_insert(data, cht);
    }

/***********************  E = 4 ****************************/
    {
        std::cout << "\n------  Cuckoo Hashtable Classic E=4 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 4, CHT_Traits_Classic> cht(hash_funcs_4);
        test_task_insert(data, cht);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable DFS E=4 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 4, CHT_Traits_Greed_DFS> cht(hash_funcs_4);
        test_task_insert(data, cht);
    }

    {
        std::cout << "\n------  Cuckoo Hashtable BFS E=4 ------- " << std::endl;
        CuckooHashTable<std::string, value_t, MyStringHash, 4, CHT_Traits_Greed_BFS> cht(hash_funcs_4);
        test_task_insert(data, cht);
    }

    return 0;
}
