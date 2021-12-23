#pragma once

#include <vector>
#include <random>
#include <unordered_map>

#include "tools/profiler.h"


/** Вставка в дерево n произвольных числел из data
 */
template<typename TREE>
void generate_tree(TREE& avlt, const std::vector<int>& data) {
    for (const auto& x : data) {
        avlt.insert(x);
    }
}

/**  Поиск в дереве каждого gap числа из данных data
 */
template<typename TREE>
void search_in_tree(TREE& avlt, const std::vector<int>& data, size_t gap) {
    for (size_t i = 0; i < data.size(); i += gap) {
        const auto& x = data[i];
        static volatile bool seed = false; // avoid optimisation and non calling function
        if constexpr (std::is_same_v < TREE, std::set<int> >) {
            static auto t = avlt.begin();  // avoid optimisation and non calling function
            t = avlt.find(x); 
        }
        else {
            seed = avlt.search(x);

        }
    }
}


/** Удаление из дерева каждого gap числа из данных data
 */
template<typename TREE>
void remove_from_tree(TREE& avlt, const std::vector<int>& data, size_t gap) {
    for (size_t i = 0; i < data.size(); i += gap) {
        const auto& x = data[i];
        if constexpr (std::is_same_v < TREE, std::set<int> >) {
            [[maybe_unused]] auto t = avlt.find(x);
            if (t != avlt.end()) {
                avlt.erase(t);
            }
        }
        else {
            avlt.remove(x);

        }
    }
}




enum class Data_Class {
    random,
    sequeantial
};

/**
 * Циклический запуск тестов профилирования для различных размеров N
*  Выполненение с измренеием вермени:
*   - n операций вставки случайного числа
*   - n/10 операций поиска случайного числа
*   - n/10 операций удаление случайного числа
*/
template<typename TREE>
void test_profile(Data_Class dc, const std::vector<unsigned long>& sizes) {
    std::cout << "\nProfile test for:" << typeid(TREE).name() <<
       ( dc == Data_Class::random ? " random data\n" : " sequential data \n");
    std::cout << "N     INSERT_N / SEARCH_N/10 / REMOVE_N/10 "<<std::endl;

    // Статические данные - избежать повторного создания и иметь одинаковые входные данные
    static std::unordered_map< size_t, std::vector<int> > data_rnd;
    static std::unordered_map< size_t, std::vector<int> > data_seq;

    for (auto n : sizes) {
        
        std::cout << "N=" << n << "  | ";
        std::cout.flush(); 

        if (dc == Data_Class::random && data_rnd.count(n) == 0) {
            data_rnd[n] = std::vector<int>();
            data_rnd[n].reserve(n);
            std::mt19937 engine;
            std::uniform_int_distribution<int> rnd(0, std::numeric_limits<int>::max());
            for (unsigned long i = 0; i < n; i++) {
                data_rnd[n].push_back(rnd(engine));
            }
        }

        if (dc == Data_Class::sequeantial && data_seq.count(n) == 0) {
            data_seq[n] = std::vector<int>();
            data_seq[n].reserve(n);
            for (unsigned long i = 0; i < n; i++) {
                data_seq[n].push_back(i);
            }
        }

        std::vector<int>* data = nullptr;
        if (dc == Data_Class::random) data = &data_rnd[n];
        if (dc == Data_Class::sequeantial) data = &data_seq[n];
        if (data == nullptr) {
            std::cout << "Internal test error \n";
            return;
        }

        TREE tree;
        {
            LogDuration ld([n](auto ms) {std::cout << ms << " / "; std::cout.flush(); });
            generate_tree(tree, *data);
        }
        {
            LogDuration ld([n](auto ms) {std::cout << ms << " / "; std::cout.flush();  });
            search_in_tree(tree, *data, 10);
        }
        {
            LogDuration ld([n](auto ms) {std::cout << ms << "\n"; std::cout.flush(); });
            remove_from_tree(tree, *data, 10);
        }

    } // for sizes

}
