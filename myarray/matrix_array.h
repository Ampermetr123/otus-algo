#pragma once

#include <memory>
#include <cstdlib>

#include "expand_array.h"
#include "pretty.h"


namespace myarray {

  #define CHECK_BOUNDS(idx, size) { \
    if ((idx) >= (size))          \
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": index " + std::to_string(idx) + " is out of bound"); \
  }

  template<typename T, size_t ROWSIZE>
  class MatrixArray : public IArray<T> {
  protected:
    using TROW = ExpandArray<T, ExpandConstSize<ROWSIZE> >;
    ExpandArray< std::unique_ptr<TROW>, ExpandConstSize<1> > rows;
    size_t size = 0;


  public:
    MatrixArray() {
      static_assert(std::is_nothrow_move_constructible<T>::value);
    }

    ~MatrixArray() {
      release();
    }

    void release() override {
      rows.release();
      size = 0;
    }

    size_t count() const override {
      return size;
    }

    size_t memory_usage() const override {
      size_t s = 0;
      for (size_t i = 0; i < rows.count(); i++) {
        s += rows.get(i)->memory_usage();
      }
      return s + sizeof(*this);
    }

    void add(const T& it) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        const auto r = size / ROWSIZE;
        if (r >= rows.count()) {
          rows.add(std::make_unique<TROW>());
        }
        rows.get(r)->add(it);
        size += 1;
      }
    }

    void add(T&& it) override {
      if constexpr (!std::is_move_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      {
        const auto r = size / ROWSIZE;
        if (r >= rows.count()) {
          rows.add(std::make_unique<TROW>());
        }
        rows.get(r)->add(std::forward<T>(it));
        size += 1;
      }
    }


    void removeLast() override {
      if (size > 0) {
        const auto r = (size - 1) / ROWSIZE;
        rows.get(r)->removeLast();
        size -= 1;
      }
    }

    T& get(size_t idx) override {
      return const_cast<T&>(std::as_const(*this).get(idx));
    }

    const T& get(size_t idx) const override {
      CHECK_BOUNDS(idx, size);
      auto pos = std::lldiv(static_cast<long long>(idx), ROWSIZE);
      return rows.get(pos.quot)->get(pos.rem);
    }

    void insert(const T& it, size_t idx) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        CHECK_BOUNDS(idx, size+1);
        if (idx == size) {
          add(it);
        }
        else {
          add(std::move(get(size - 1)));
          // shift right to 1 pos from [idx to size-2 ]
          for (auto i = size - 2; i > idx; i--) {
            // Like get(i) = std::move(get(i - 1)), but without boundary check
            const auto pr = std::lldiv(static_cast<long long>(i), ROWSIZE);
            const auto pl = std::lldiv(static_cast<long long>(i-1), ROWSIZE);
            rows.get(pr.quot)->get(pr.rem) = std::move(rows.get(pl.quot)->get(pl.rem));
          }
          get(idx) = it;
        }
      }
    }

    void insert(T&& it, size_t idx) override {
      if constexpr (!std::is_move_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        CHECK_BOUNDS(idx, size+1);
        if (idx == size) {
          add(std::forward<T>(it));
        }
        else {
          add(std::move(get(size - 1)));
          // shift right to 1 pos from [idx to size-2 ]
          for (auto i = size - 2; i > idx; i--) {
            // Like get(i) = std::move(get(i - 1)), but without boundary check
            const auto pr = std::lldiv(static_cast<long long>(i), ROWSIZE);
            const auto pl = std::lldiv(static_cast<long long>(i-1), ROWSIZE);
            rows.get(pr.quot)->get(pr.rem) = std::move(rows.get(pl.quot)->get(pl.rem));
          }
          get(idx) = std::forward<T>(it);
        }
      }
    }

    void remove(size_t idx) override {
      CHECK_BOUNDS(idx, size);
      // shift_left to 1 pos up to idx
      for (auto i = idx; i < size - 1; i++) {
        get(i) = std::move(get(i + 1));
      }
      this->removeLast();
    }
  };

}