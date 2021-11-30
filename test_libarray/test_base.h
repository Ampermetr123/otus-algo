#pragma once
#include "test_helpers.h"

template< template<class T> class TARR>
void test_array_base() {

  constexpr auto make_string = [](const TARR<int>& a) {
    std::ostringstream os;
    for (int i = 0; i < a.count(); i++) {
      os << a.get(i);
    }
    return os.str();
  };

  try {

    std::cerr << "Basic test for '" << typeid(TARR<int>).name() << "'";
    TARR<int>  arr;

    // ADD
    for (int i = 0; i < 10; i++) {
      arr.add(1);
    }
    for (int i = 0; i < 10; i++) {
      arr.add(i);
    }
    ASSERT_EQUAL(make_string(arr), std::string("11111111110123456789"));

    // REMOVE_LAST
    arr.removeLast();
    arr.removeLast();
    ASSERT_EQUAL(make_string(arr), std::string("111111111101234567"));
    ASSERT_EQUAL(arr.count(), 18);

    // insert
    arr.insert(9, 0);
    ASSERT_EQUAL(make_string(arr), std::string("9111111111101234567"));
    arr.insert(9, 19);
    ASSERT_EQUAL(make_string(arr), std::string("91111111111012345679"));
    ASSERT_EQUAL(arr.count(), 20);
    arr.insert(9, 15);
    ASSERT_EQUAL(make_string(arr), std::string("911111111110123945679"));
    ASSERT_EQUAL(arr.count(), 21);

    //remove
    arr.remove(0);
    ASSERT_EQUAL(make_string(arr), std::string("11111111110123945679"));
    arr.remove(14);
    ASSERT_EQUAL(make_string(arr), std::string("1111111111012345679"));
    ASSERT_EQUAL(arr.count(), 19);

    //release
    arr.release();
    ASSERT_EQUAL(arr.count(), 0);
    ASSERT_EQUAL( arr.memory_usage(), sizeof(arr) );

    std::cerr << " => SUCCESS" << std::endl;

  } catch (std::exception& e) {
    std::cerr << " FAIL: \n    " << e.what() << std::endl;
  }
}
