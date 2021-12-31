/**
 * Тесты для класа SplayTree
 */

#pragma once

#include <map>
#include <iostream>

#include "splay.hpp"
#include "tools/test_helpers.h"


 /*******************************************************************************
 *                           Вспомогательные функции                           *
 *******************************************************************************/

 /// Карта Sply-дерева: "глубина - значения элементов в дереве с этой глубиной"
template<typename T>
using splay_tree_map = std::map<size_t, std::vector<T> >;


/**
 * Создает карту splay дерева
 */
template <typename T>
auto make_splay_tree_map(SplayTree<T>& tree) {
    splay_tree_map<T> map;
    tree.for_each([&map](const int &val, auto depth) { map[depth].push_back(val); } );
    return map;
}

/**
 * Вывод карты Splay - дерева 
 */
template <typename T>
void print_splay_tree_map(splay_tree_map<T>& map, std::ostream& os)
{
    // определяем максимальную глубину
    int depth = 0;
    while (map.count(depth) > 0)
        depth++;

    // выводим словарь
    for (auto j = 0; j <= depth - 1; j++) {
        os << "d=" << j << "       ";
        for (const auto& x : map[j]) {
            os << x << " ";
        }
        os << std::endl;
    };
    os << std::endl;
}


/*******************************************************************************
 *                                    Тесты                                    *
 *******************************************************************************/

/**
 * Проверка основных операций
 */
void test_splay_ops() try {
    using TREE = SplayTree<int>;
    std::cerr << "Basic test for " << typeid(TREE).name() << "... ";

    std::vector<int> input = { 32, 45, 65, 76, 34, 45, 67, 87, 23 };
    std::vector<int> input_nr = { 32, 45, 65, 76, 34, 67, 87, 23 }; // без повторов
    std::vector<int> sorted(input_nr);
    std::sort(std::begin(sorted), std::end(sorted)); // В таком порядке должны выбираться элементы из дерева
    std::vector<int> tree_content; // выбор все элементов из дерева
    
    TREE tree;
    // 1) добавляем элементы и проверяем содержимое
    for (const auto& x : input) {
            tree.insert(x);
    }

    // 2) все добавленные элементы должны находиться
    for (const auto& x : input_nr) {
        ASSERT_TRUE(tree.search(x));
    }

    // 3) не добавленные - не находятся
    ASSERT_TRUE(!tree.search(0));

    // 4) проверка содержимого и отсуствие дубликата
    tree.for_each([&tree_content](auto x, int) { tree_content.push_back(x); });
    ASSERT_EQUAL(tree_content, sorted);

    // 5) удаляем самый большой и самый маленький элемент и проверяем содержимое 
    tree.remove(*sorted.begin());
    tree.remove(*(sorted.end() - 1));
    std::vector<int> sorted2(sorted.begin() + 1, sorted.end() - 1);
    tree_content.clear();
    tree.for_each([&tree_content](auto x, int) { tree_content.push_back(x); });
    ASSERT_EQUAL(tree_content, sorted2);
}
catch (std::exception& e) {
    std::cerr << " FAIL: \n    " << e.what() << std::endl;
}


/**
 * Проверка распредления вершин по глубине
 * после вставки тествой последовательности элементов
 */
void test_splay_depth() {
    std::cerr << "Test for SplayTree<int> tree ...\n";
    SplayTree<int> tree;

    // Создаем дерево
    std::vector<int> input = { 32, 45, 65, 76, 34, 45, 67, 87, 23, 28, 29 };
    for (const auto& x : input) {
        tree.insert(x);
    }

    auto map = make_splay_tree_map(tree);
    //print_splay_tree_map(map, std::cerr);

    // Проверяем глубины
    try {
        ASSERT_EQUAL(map[0], (std::vector<int>{29}));
        ASSERT_EQUAL(map[1], (std::vector<int>{28, 32}));
        ASSERT_EQUAL(map[2], (std::vector<int>{23, 76}));
        ASSERT_EQUAL(map[3], (std::vector<int>{65, 87}));
        ASSERT_EQUAL(map[4], (std::vector<int>{34, 67}));
        ASSERT_EQUAL(map[5], (std::vector<int>{45}));

        std::cerr << "OK\n";
    }
    catch (std::exception& e) {
        std::cerr << " FAIL: \n    " << e.what() << std::endl;
    }
}

