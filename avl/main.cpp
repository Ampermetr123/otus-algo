#include <set>
#include <algorithm>

#include "avltree.hpp"
#include "bstree.hpp"
#include "tools/locale.h"
#include "test_profile.h"
#include "test_base.h"
#include "test_avl.h"
#include "test_splay.h"



int main(int, char**) {

    // Проверка, что деревья работат корректно
    test_tree_basic<BSTree<int>>();
    test_splay_ops();
    test_splay_depth();
    test_avl_ops();
    test_avl_heights();

    // Измерение производительности деревьев

    setup_locale_thouthands(std::cout);

    const std::vector<unsigned long> sizes_short = { 10'000, 100'000 };
    const std::vector<unsigned long> sizes_long = { 10'000, 100'000, 1'000'000, 10'000'000};

    // Измерения на случайных данных
    // test_profile<BSTree<int>>(Data_Class::random, sizes_long);
    // test_profile<AVLTree<int>>(Data_Class::random, sizes_long);
    //test_profile<std::set<int>>(Data_Class::random, sizes_long);
    //test_profile<SplayTree<int>>(Data_Class::random, sizes_long);

    // Измерения на последовательных данных
    // test_profile<BSTree<int>>(Data_Class::sequeantial, sizes_short);
    // test_profile<AVLTree<int>>(Data_Class::sequeantial, sizes_long);
    test_profile<SplayTree<int>>(Data_Class::sequeantial, sizes_long);

};
