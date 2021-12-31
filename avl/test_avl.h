/**
 * Тесты для класа AVLTree
 */

#pragma once
#include <map>
#include <iostream>

#include "avltree.hpp"
#include "test_base.h"
#include "tools/test_helpers.h"


/*******************************************************************************
 *                           Вспомогательные функции                           *
 *******************************************************************************/

template<typename T>
using avl_tree_map = std::map<size_t, std::vector<T>>;

/**
 * Создает карту для AVL дерева: "высота - список вершин в дереве с этой высотой"
 */
template <class T>
auto make_avl_tree_map(AVLTree<T>& tree) {
    avl_tree_map<T> map;
    tree.for_each( [&map](auto sptr){ map[sptr->height].push_back(sptr->val); } );
    return map;
}


/**
 * Вывод карты AVL - дерева 
 */
template <class T>
void print_avl_tree_map(avl_tree_map<T>& tmap, std::ostream& os) {
    // Определяем максимальную высоту
    size_t i = 1;
    while (tmap.count(i) > 0)
        i++;

    // Вывод 
    for (auto j = i - 1; j > 0; j--) {
        os << "h=" << j << "       ";
        for (const auto x : tmap[j] ) {
            os << x << " ";
        }
        os << std::endl;
    };
}


/*******************************************************************************
 *                                    Тесты                                    *
 *******************************************************************************/

/**
 * Проверка основных операций
 */
const auto test_avl_ops = test_tree_basic<AVLTree<int> >;

/**
 * Проверка распредления вершин по высотам
 * после вставки тествой последовательности элементов
 */
void test_avl_heights() {

    std::cerr << "Test internals for AVLTree<int> tree ...\n";
    const std::vector<int> input = { 32, 45, 65, 76, 34, 45, 67, 87, 23, 28, 29 };

    // Создаем дерево
    AVLTree<int> avlt;
    for (const auto& x : input) {
        avlt.insert(x);
    }

    auto map = make_avl_tree_map(avlt);
    // print_avl_tree_map(map, std::cerr);

    // Проверяем распредление вершин
    try {
        ASSERT_EQUAL(map[4], std::vector<int>{45});
        ASSERT_EQUAL(map[3], (std::vector<int>{32, 67}));
        ASSERT_EQUAL(map[2], (std::vector<int>{28, 76}));
        ASSERT_EQUAL(map[1], (std::vector<int>{23, 29, 34, 65, 87}));

        std::cerr << "OK\n";
    }
    catch (std::exception& e) {
        std::cerr << " FAIL: \n    " << e.what() << std::endl;
    }

}
