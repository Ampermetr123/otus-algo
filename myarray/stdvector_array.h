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

    void add(T&& it) override {
      if constexpr (!std::is_move_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        v.push_back(std::forward<T>(it));
      }
    }

    void removeLast() override {
      v.pop_back();
    };

    const T& get(size_t idx) const override {
      return v.at(idx);
    }

    T& get(size_t idx) override {
      return v.at(idx);
    }

    void insert(const T& it, size_t idx) override {
      if (idx < 0 || idx > v.size()) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": index [" + std::to_string(idx) + " is out of bound");
      }
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        v.insert(std::begin(v) + idx, it);
      }
    }

    void insert(T&& it, size_t idx) override {
      if (idx < 0 || idx > v.size()) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": index [" + std::to_string(idx) + " is out of bound");
      }
      if constexpr (!std::is_move_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        v.insert(std::begin(v) + idx, std::forward<T>(it));
      }
    };


    void remove(size_t idx) override {
      if (idx < 0 || idx >= v.size()) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": index [" + std::to_string(idx) + " is out of bound");
      }
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