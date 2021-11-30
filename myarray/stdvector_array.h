#include "interface_array.h"
#include <vector>

#pragma once

namespace myarray {

  template<typename T>
  class STDVectorArray : public IArray<T> {
  private:
    std::vector<T> v;

  public:

    size_t count() const override {
      return v.size();
    };

    void add(const T& it) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        v.push_back(it);
      }
    }

    void add(T&& it) {
      v.push_back(std::forward<T>(it));
    }

    void removeLast() override {
      v.pop_back();
    };

    const T& get(size_t idx) const override {
      CHECK_BOUNDS(idx, v.size());
      return v[idx];
    }

    T& get(size_t idx) override {
      CHECK_BOUNDS(idx, v.size());
      return v[idx];
    }

    void insert(const T& it, size_t idx) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        insert<>(it, idx);
      }
    }

    template<class U>
    void insert(U&& it, size_t idx) {
      CHECK_BOUNDS(idx, v.size() + 1);
      v.insert(std::begin(v) + idx, std::forward<U>(it));
    };


    void remove(size_t idx) override {
      CHECK_BOUNDS(idx, v.size());
      v.erase(std::begin(v) + idx);
    };


    size_t memory_usage() const override {
      return v.capacity() + sizeof(*this);
    };


    void  release() {
      v.clear();
      v.shrink_to_fit();
    };  // remove all elements and free memory 
  };

}