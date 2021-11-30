/**
 * Тест библиотеки динамических массивов "MyArray"
 */

#include <fstream>
#include <memory>
#include <vector>

#include "myarray.h"
#include "test_internal.h"
#include "test_profile.h"
#include "test_base.h"


using namespace myarray;


template<typename T>
using MatrixArray4 = MatrixArray<T, 4>;

template<typename T>
using MatrixArray100 = MatrixArray<T, 100>;

int main() {

  // Тестирование по основным операциям
  test_array_base<SingleArray>();
  test_array_base<VectorArray>();
  test_array_base<FactorArray>();
  test_array_base<MatrixArray4>();
  test_array_base<STDVectorArray>();

  // Ручное тестирвоание - анализ вывода в консоли
  //test_array_internal<SingleArray>();
  //test_array_internal<VectorArray>();
  //test_array_internal<MatrixArray4>();


  const std::vector<size_t> sizes_short = { 1'000, 10'000, 100'000 , 10'000'000};
  const std::vector<size_t> sizes_long = { 1'000, 10'000, 100'000, 1'000'000, 10'000'000 };

  {
    std::ofstream f("report_myarray.md");
    ProfileTest::test_array_profile< SingleArray >(sizes_long, f, "SingleArray");
    std::cerr << "*";
    ProfileTest::test_array_profile< VectorArray >(sizes_long, f, "VectorArray (100)");
    std::cerr << "*";
    ProfileTest::test_array_profile< MatrixArray100 >(sizes_short, f, "MatrixArray (100)");
    std::cerr << "*";
    ProfileTest::test_array_profile< FactorArray >(sizes_long, f, "FactorArray (1.5)");
    std::cerr << "*";
    ProfileTest::test_array_profile< STDVectorArray >(sizes_long, f, " std::vector");
    std::cerr << "*";
  }

  return 0;
}


