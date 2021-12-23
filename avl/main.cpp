#include <set>
#include <algorithm>

#include "avltree.hpp"
#include "bstree.hpp"
#include "tools/locale.h"
#include "test_profile.h"
#include "test_base.h"


int main(int, char**) {

    test_basic<BSTree<int>>();
    test_basic<AVLTree<int>>();
    test_avl();

    setup_locale_thouthands(std::cout);

    const std::vector<unsigned long> sizes_short = { 10'000, 100'000 };
    const std::vector<unsigned long> sizes_long = { 10'000, 100'000, 1'000'000, 10'000'000};

    // test_profile<BSTree<int>>(Data_Class::random, sizes_long);
    // test_profile<AVLTree<int>>(Data_Class::random, sizes_long);
    test_profile<std::set<int>>(Data_Class::random, sizes_long);

    // test_profile<BSTree<int>>(Data_Class::sequeantial, sizes_short);
    // test_profile<AVLTree<int>>(Data_Class::sequeantial, sizes_long);
    test_profile<std::set<int>>(Data_Class::sequeantial, sizes_long);

};
