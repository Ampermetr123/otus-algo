#pragma once

#include <iostream>
#include <typeinfo>
#include <random>
#include <iostream>

#include "locale.h"
#include "profiler.h"
#include "myarray.h"
#include "test_helpers.h"

namespace myarray {


  /**  Класс для подсчета операций создания, копирования, перемещеия, присваивания, удаления 
    *  всех своих экземпляров
    * */
  class B {
    static inline unsigned long long int ops = 0;
    //  int dummy; // We use some data, to prevent compiller optimization for empty classes
  public:
    static auto total_operations() {
      return ops;
    }
    static void reset_operations() {
      ops = 0;
    }
    B() {
      ops++;
    }
    ~B() {
      ops++;
    }
    B([[maybe_unused]] const B& other) {
      ops++;
    }
    B& operator=([[maybe_unused]] const B& other) {
      // dummy = other.dummy;
      ops++;
      return *this;
    }
    B& operator=([[maybe_unused]] B&& other) {
      // dummy = other.dummy;
      ops++;
      return *this;
    }
    B([[maybe_unused]] B&& other) noexcept {
      ops++;
    }
  };


  class ProfileTest {
  private:

    static void print_profile_case_header(std::ostream& os) {
      os << "| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |" << std::endl;
      os << "|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|" << std::endl;
    };

    template< template<class T> class TARR>
    static void test_profile_case(size_t size, std::ostream& os) {

      const auto make_result = [&os](unsigned long long dur) {
        os << " " << dur << " ms <br> ";
        auto ops = B::total_operations();
        if (ops <= 10'000) {
          os << ops;
        }
        else if (ops % 1'000'000 > 10'000) {
          os.precision(2);
          os << std::fixed << double(ops) / 1'000'000 << " M";
        }
        else {
          os << ops / 1'000'000 << " M";
        }
        os << " | ";
        os.flush();
      };

      std::random_device rd;     
      std::mt19937 rng(rd());   
      std::uniform_int_distribution<int> uni(0, static_cast<int>(size - 1)); 

      os << "| " << size << " | ";

      TARR<B> arr;

      // Add
      B::reset_operations();
      {
        LOG_DURATION(make_result);
        for (size_t i = 0; i < size; i++) {
          arr.add(B());
        }
      }

      // Get(random)
      B::reset_operations();
      {
        LOG_DURATION(make_result);
        for (size_t i = 0; i < size; i++) {
          auto tmp = arr.get(uni(rng));
        }
      }

      // will past it at the end
      auto mem_usage = double(arr.memory_usage()) / (1024 * 1024);

      // RemoveLast
      B::reset_operations();
      {
        LOG_DURATION(make_result);
        for (size_t i = 0; i < size; i++) {
          arr.removeLast();
        }
      }

      ASSERT_EQUAL(arr.count(), 0);

      // Next test are to long for big data
      if (size <= 1'000'000) {

        // Insert T(0) to previous array
        B::reset_operations();
        {
          LOG_DURATION(make_result);
          for (size_t i = 0; i < size; i++) {
            arr.insert(B(), 0);
          }
        }

        // Insert T(0) to new array
        arr.release();
        B::reset_operations();
        {
          LOG_DURATION(make_result);
          for (size_t i = 0; i < size; i++) {
            arr.insert(B(), 0);
          }
        }

        // Remove T(0, random)
        B::reset_operations();
        {
          LOG_DURATION(make_result);
          for (size_t i = 0; i < size && arr.count(); i++) {
            auto index = uni(rng) % arr.count();
            arr.remove(index);
          }
        }

        // Insert T(0, random) to empty
        arr.release();
        B::reset_operations();
        {
          LOG_DURATION(make_result);
          for (size_t i = 0; i < size; i++) {
            auto index = uni(rng) % (arr.count() + 1);
            arr.insert(B(), index);
          }
        }
      }
      else {
        os << " skipped | skipped | skipped | skipped |";
      }

      os.precision(2);
      os << std::fixed << mem_usage << " MB |";
      os << std::endl;
    }

  public:

    template< template<class T> class TARR>
    static void test_array_profile(const std::vector<size_t>& sizes, std::ostream& os, const std::string& test_name = "") {
      try {
        setup_locale_thouthands(os);
        if (test_name.size()) {
          os << "\n## " << test_name << "\n";
        }
        os << "\nUsed cpp class ```" << typeid(TARR<B>).name() << "```\n" << std::endl;
        print_profile_case_header(os);
        for (auto sz : sizes) {
          test_profile_case<TARR>(sz, os);
        }
        os << std::endl << "Done" << std::endl;
      }
      catch (const std::exception& ex) {
        os << "Error : " << ex.what() << std::endl;
      }
    }

  };

}