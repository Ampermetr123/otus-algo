#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include "tools/test_helpers.h"


/**
 * Тест основных операций, для типов BSTree и AVLTree
 * @tparam TREE - тип дерева 
 */

template<typename TREE>
void test_tree_basic() try {
    std::cerr << "Basic test for " << typeid(TREE).name() << "... ";
    std::vector<int> input = { 32, 45, 65, 76, 34, 45, 67, 87, 23 };
    std::vector<int> input_nr = { 32, 45, 65, 76, 34, 67, 87, 23 }; // без повторов
    std::vector<int> sorted(input_nr);
    std::sort(std::begin(sorted), std::end(sorted)); // В таком порядке должны выбираться элементы из дерева
    std::vector<int> deleted; // здесь будем хранить все удаленные из дерева элементы
    std::vector<int> tree_content; // выбор все элементов из дерева
    {
        TREE tree;
        // 1) добавляем элементы и проверяем содержимое
        for (const auto& x : input) {
            tree.insert(x, [&deleted](auto* ptr) { deleted.push_back(ptr->val); delete ptr;});
        }

        for (const auto& x : input_nr) {
            ASSERT_TRUE(tree.search(x));
        }

        tree.for_each([&tree_content](auto sp) { tree_content.push_back(sp->val); });
        ASSERT_EQUAL(tree_content, sorted);

        // 2) удаляем самый большой и самый маленький элемент и проверяем содержимое 
        tree.remove(*sorted.begin());
        tree.remove(*(sorted.end() - 1));
        std::vector<int> sorted2(sorted.begin() + 1, sorted.end() - 1);
        tree_content.clear();
        tree.for_each([&tree_content](auto sp) { tree_content.push_back(sp->val); });
        ASSERT_EQUAL(tree_content, sorted2);
    }

    // 3) Проверяем, что вместе с деревом удалены все элементы
    std::sort(std::begin(deleted), std::end(deleted));
    ASSERT_EQUAL(deleted, sorted);
    std::cerr << "OK\n";

}
catch (std::exception& e) {
    std::cerr << " FAIL: \n    " << e.what() << std::endl;
}
