#pragma once

#include <memory>
#include <algorithm>
#include <exception>
#include <type_traits>
#include <cassert>
#include <utility>

#include "pretty.h"
#include "interface_array.h"

namespace myarray {

  template<size_t EXPAND_SIZE>
  struct ExpandConstSize {
    static inline constexpr size_t size(size_t current_size) {
      return current_size + EXPAND_SIZE;
    }
  };

  template<size_t EXPAND_FACTOR_MUL, size_t EXPAN_FACTOR_DIV = 1>
  struct ExpandFactorSize {
    static inline constexpr size_t size(size_t current_size) {
      return std::max(current_size + 1,
        size_t(current_size * EXPAND_FACTOR_MUL / EXPAN_FACTOR_DIV));
    }
  };

  template<typename T, typename ExpandStrategy>
  class ExpandArray : public IArray<T> {
  protected:
    using alloc_tr = std::allocator_traits<std::allocator<T>>;
    std::allocator<T> alloc;
    size_t capacity = 0;
    size_t size = 0;
    T* pmem;

    static void reconstruct(std::allocator<T>& alloc, T* psrc, T* pdst, size_t src_size) {
      for (size_t i = 0; i < src_size; i++) {
        alloc_tr::construct(alloc, &pdst[i], std::move(psrc[i]));
        alloc_tr::destroy(alloc, &psrc[i]);
      }
    }

  public:
    ExpandArray() {
      static_assert(std::is_nothrow_move_constructible<T>::value);
    }

    ExpandArray(const ExpandArray& other) {
      if (capacity > other.size)
      {
        auto m = std::min(size, other.size);
        // общая часть
        for (size_t i = 0; i < m; i++) {
          pmem[i] = other.pmem[i];
        }
        // if other > current
        for (size_t i = m; i < other.size; i++) {
          add(other.pmem[i]);
        }
        // if other < current
        while (size != other.size) {
          removeLast();
        }
      }
      else {
        release();
        capacity = other.capacity;
        pmem = alloc_tr::allocate(alloc, capacity);
        for (size_t i = 0; i < other.size; i++) {
          alloc_tr::construct(alloc, &pmem[i], other.pmem[i]);
        }
        size = other.size;
      }
    }

    ExpandArray(ExpandArray&& other) {
      release();
      pmem = other.pmem;
      capacity = other.capacity;
      size = other.size;
      other.pmem = nullptr;
      other.size = 0;
      other.capacity = 0;
    }

    ~ExpandArray() {
      release();
    }

    void release() override {
      while (count()) {
        removeLast();
      }
      if (capacity > 0) {
        alloc_tr::deallocate(alloc, pmem, capacity);
      }
      capacity = 0;
    }

    size_t count() const override {
      return size;
    }

    size_t memory_usage() const override {
      return capacity + sizeof(*this);
    }

    void add(const T& it) override {
      insert(it, size);
    }

    void add(T&& it)  {
      insert(std::forward<T>(it), size);
    }

    void removeLast() override {
      if (size > 0)
        remove(size - 1);
    }

    T& get(size_t idx) override {
      CHECK_BOUNDS(idx, size);
      return pmem[idx];
    }

    const T& get(size_t idx) const override {
      CHECK_BOUNDS(idx, size);
      return pmem[idx];
    };

    T& at(size_t idx) const {
      return pmem[idx]; // No check bounds!
    };

    void insert(const T& it, size_t idx) override  {
      // Так как эта функция шаблона виртуальная, она инстанцируетс даже тогда, когда не происходит её вызов, 
      // поэтому проверяем, что в T имеется конструктор копирования, 
      if constexpr (!std::is_copy_constructible_v<T>) {
        throw std::runtime_error(std::string("Function not supported for type ") + typeid(T).name());
      } else
      {
        insert<>(it, idx);
      }
    }

    template<typename U>
    void insert(U&& it, size_t idx)  {
        CHECK_BOUNDS(idx, size + 1);
        const bool need_realloc = (size >= capacity || capacity == 0);
        if (need_realloc) { // конструируем объекты в новой памяти  
          auto new_size = ExpandStrategy::size(size);
          auto pdst = alloc_tr::allocate(alloc, new_size);
          ExpandArray::reconstruct(alloc, pmem, pdst, idx);
          alloc_tr::construct(alloc, &pdst[idx], std::forward<U>(it));
          ExpandArray::reconstruct(alloc, pmem + idx, pdst + idx + 1, size - idx);
          if (capacity > 0) {
            alloc_tr::deallocate(alloc, pmem, capacity);
          }
          capacity = new_size;
          pmem = pdst;
        }
        else { // ...или перемещаем объекты в имеющейся памяти
          if (size > 0 && idx != size) { // если новый элемент - не единственный и не последний
            alloc_tr::construct(alloc, &pmem[size], std::move(pmem[size - 1]));
            for (auto i = size - 1; i > idx; i--) {
              pmem[i] = std::move(pmem[i - 1]);
            }
            pmem[idx] = std::forward<U>(it);
          }
          else {
            alloc_tr::construct(alloc, &pmem[idx], std::forward<U>(it));
          }
        }
        size += 1;
    }

    void remove(size_t idx) override {
      CHECK_BOUNDS(idx, size);
      for (auto i = idx; i < size - 1; i++) {
        pmem[i] = std::move(pmem[i + 1]);
      }
      alloc_tr::destroy(alloc, &pmem[size - 1]);
      size -= 1;
    }

  };

}