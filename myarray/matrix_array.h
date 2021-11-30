#pragma once

#include <memory>
#include <cstdlib>

#include "expand_array.h"
#include "pretty.h"


namespace myarray {

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
        s += rows.at(i)->memory_usage();
      }
      return s + sizeof(*this);
    }

    void add(const T& it) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        add<>(it);
      }
    }

    template<typename U>
    void add(U&& it) {
        const auto r = size / ROWSIZE;
        if (r >= rows.count()) {
          rows.add(std::make_unique<TROW>());
        }
        rows.at(r)->add(std::forward<U>(it));
        size += 1;
    }

    void removeLast() override {
      if (size > 0) {
        const auto r = (size - 1) / ROWSIZE;
        rows.at(r)->removeLast();
        size -= 1;
      }
    }

    T& get(size_t idx) override {
      return const_cast<T&>(std::as_const(*this).get(idx));
    }

    const T& get(size_t idx) const override {
      CHECK_BOUNDS(idx, size);
      auto pos = std::lldiv(static_cast<long long>(idx), ROWSIZE);
      return rows.at(pos.quot)->at(pos.rem);
    }

    void insert(const T& it, size_t idx) override {
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      }
      else {
        insert<>(it, idx);
      }
    }

    template<typename U> 
    void insert(U&& it, size_t idx)  {
        CHECK_BOUNDS(idx, size+1);
        if (idx == size) {
          add(std::forward<U>(it));
        }
        else {
          add(std::move(get(size - 1)));
          // shift right to 1 pos from [idx to size-2 ]
          for (auto i = size - 2; i > idx; i--) {
            // Like get(i) = std::move(get(i - 1)), but without boundary check
            const auto pr = std::lldiv(static_cast<long long>(i), ROWSIZE);
            const auto pl = std::lldiv(static_cast<long long>(i-1), ROWSIZE);
            rows.at(pr.quot)->at(pr.rem) = std::move(rows.at(pl.quot)->at(pl.rem));
          }
          get(idx) = std::forward<U>(it);
        }
    }
    
    void remove(size_t idx) override {
      CHECK_BOUNDS(idx, size);
      // shift_left to 1 pos up to idx
      for (auto i = idx; i < size - 1; i++) {
        // like get(i) = std::move(get(i + 1));
        const auto pr = std::lldiv(static_cast<long long>(i+1), ROWSIZE);
        const auto pl = std::lldiv(static_cast<long long>(i), ROWSIZE);
        rows.at(pl.quot)->at(pl.rem) = std::move(rows.at(pr.quot)->at(pr.rem));
      }
      this->removeLast();
    }
    
  };

}