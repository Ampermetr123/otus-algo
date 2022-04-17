#pragma once

#include <memory>
#include <iostream>
#include <iterator>
#include "optlog.h"
#include <string.h>
#include <cassert>

// Location - хранилище-массив для данных типа T длиной SIZE
// В отличие от std::vector НЕ инициализирует память (не вызывает конструктор)
// до момента установки элемента массива.
// Контролирует установку элемента масива с помощью битового массива.
// Поддерживает итераторы по установленным значениям
// Используется параметр Allocator, чтобы можно было кастомизировать выделение памяти.


template <typename T, typename Allocator = std::allocator<T> >
class Location {

    using data_allocator_t = std::allocator_traits<Allocator>;
    using char_allocator_t = std::allocator_traits< typename Allocator:: template rebind<uint8_t>::other >;
    Allocator data_alloc;
    typename Allocator:: template rebind<uint8_t>::other char_alloc;

    const size_t size;
    size_t fill_size = 0;
    std::shared_ptr<uint8_t> ppersist;
    std::shared_ptr<T> pdata;

    static inline bool get_bit(const uint8_t* ptr, size_t i) {
        const uint8_t byte = *(ptr + (i / 8));
        const uint8_t bitmask = uint8_t(1) << (i % 8);
        bool persist = byte & bitmask;
        return persist;
    }

    static inline void set_bit(uint8_t* ptr, size_t i) {
        uint8_t* pbyte = (ptr + (i / 8));
        const uint8_t bitmask = uint8_t(1) << (i % 8);
        *pbyte = *pbyte | bitmask;
    }

    static inline void clear_bit(uint8_t* ptr, size_t i) {
        uint8_t* pbyte = (ptr + (i / 8));
        const uint8_t bitmask = uint8_t(1) << (i % 8);
        *pbyte = *pbyte & (~bitmask);
    }

    /**
     * @brief Возвращает индекс следующего сущестующего элемента, начиная с позиции pos.
     * @return индекс элемента или размер хранилища (size),
     * если существующих элементов не найдено.
     */
    size_t get_next_persisted_index(size_t pos) const {
        size_t i = pos / 8;
        while (i < size / 8) {
            if (*(ppersist.get() + i) != 0)
                break;
            i++;
        }
        auto j = std::max(i * 8, pos);
        while (j < size) {
            if (get_bit(ppersist.get(), j)) {
                return j;
            }
            j++;
        }
        return size;
    }

public:

    /*******************************************************************************
     *                         Iterator implementation                             *
     *******************************************************************************/
    template <typename TT>
    class iterator_base {
    protected:
        using location_t = typename std::conditional<std::is_const<TT>::value, const Location, Location>::type;
        location_t* locptr;
        size_t pos = 0;
    public:
        friend Location;
        using iterator_category = std::input_iterator_tag;
        using value_type = TT;
        using difference_type = int;
        using pointer = TT*;
        using reference = TT&;
        iterator_base(location_t* locptr, size_t p) : locptr(locptr), pos(p) { assert(locptr); }
        iterator_base(const iterator_base& rv) { locptr = rv.locptr;  pos = rv.pos; }
        iterator_base& operator=(const iterator_base& rv) {
            locptr = rv.locptr;
            pos = rv.pos;
            return *this;
        }
        iterator_base& operator++() {
            pos = locptr->get_next_persisted_index(pos + 1);
            return *this;
        }
        iterator_base operator++(int) {
            auto tmp = pos;
            pos = locptr->get_next_persisted_index(pos + 1);
            return iterator_base(locptr, tmp);
        }
        reference operator*()  const { return (*locptr)[pos]; }
        pointer operator->() const { return locptr->pdata.get() + pos; }
        bool operator==(const iterator_base& rv) const { return (locptr == rv.locptr && pos == rv.pos); }
        bool operator!=(const iterator_base& rv) const { return !(*this == rv); }
        // one way conversion: iterator -> const_iterator
        operator iterator_base<T const>() const {
            return iterator_base<T const>(locptr, pos);
        };
    };

    using iterator = iterator_base<T>;
    using const_iterator = iterator_base<const T>;

    /*******************************************************************************
     *                       Constructor and public methods                        *
     *******************************************************************************/

    explicit Location(size_t size);
    ~Location() {
         lg1 << "Loacation destructor of size = " << size << std::endl;
    };

    iterator begin() { return iterator(this, get_next_persisted_index(0)); }
    iterator end() { return iterator(this, size); }
    const_iterator cbegin() const { return const_iterator(this, get_next_persisted_index(0)); }
    const_iterator cend() const { return const_iterator(this, size); }

    template<typename... Args>
    void emplace(size_t index, Args&&... args); //constructing T element in-place

    template<typename U>
    void set(size_t index, U&& val); // using T copy- or move- constructor

    T* get(size_t index); // checking bounds and persist
    const T* get(size_t index) const; // checking bounds and persist

    T& operator[](size_t index); // no bounds/persist checking
    const T& operator[](size_t index) const;

    inline bool persist(size_t i) const;

    void erase(size_t i);

    void erase(iterator it);

    size_t count() const noexcept { return fill_size; }; // return number of constructed objects

    size_t max_size() const noexcept { return size; }


};


template<typename T, class Allocator>
Location<T, Allocator>::Location(size_t size) :size(size) {
    // Persistance map allocation
    auto bitset_size = (size % 8 > 0) ? (size / 8 + 1) : (size / 8);
    lg1 << "Loacation constructor of size = " << size << ", bitsize = " << bitset_size << std::endl;
    ppersist.reset(char_allocator_t::allocate(char_alloc, bitset_size),
        [size, this](uint8_t* ptr) { char_allocator_t::deallocate(char_alloc, ptr, size); }
    );
    memset(static_cast<void*>(ppersist.get()), 0, bitset_size);

    // Data allocation
    pdata.reset(data_allocator_t::allocate(data_alloc, size),
        [pp = ppersist, size, &alloc = data_alloc](T* ptr) {
            // destroy every object, that where created
            for (size_t i = 0; i < size; i++) {
                if (Location::get_bit(pp.get(), i)) {
                    data_allocator_t::destroy(alloc, ptr + i);
                }
            }
            data_allocator_t::deallocate(alloc, ptr, size);
        }
    );
}


template<typename T, class Allocator>
inline bool Location<T, Allocator>::persist(size_t i) const {
    return i < size&& Location::get_bit(ppersist.get(), i);
}


template<typename T, class Allocator>
template <typename... Args>
void Location<T, Allocator>::emplace(size_t index, Args&&... args) {
    if (persist(index)) {
        data_allocator_t::destroy(data_alloc, pdata.get() + index);
    }
    else {
        fill_size++;
    }
    data_allocator_t::construct(data_alloc, pdata.get() + index, std::forward<Args>(args)...);
    Location::set_bit(ppersist.get(), index);

}


template<typename T, class Allocator>
template<typename U>
void Location<T, Allocator>::set(size_t index, U&& val) {
    if (persist(index)) {
        data_allocator_t::destroy(data_alloc, pdata.get() + index);
    }
    else {
        fill_size++;
    }
    data_allocator_t::construct(data_alloc, pdata.get() + index, std::forward<U>(val));
    Location::set_bit(ppersist.get(), index);
}


template<typename T, class Allocator>
T* Location<T, Allocator>::get(size_t index) {
    return persist(index) ? pdata.get() + index : nullptr;
}


template<typename T, class Allocator>
const T* Location<T, Allocator>::get(size_t index) const {
    return persist(index) ? pdata.get() + index : nullptr;
}

template<typename T, class Allocator>
void Location<T, Allocator>::erase(typename Location<T, Allocator>::iterator it) {
    if (it.locptr == this && persist(it.pos)) {
        erase(it.pos);
    }
}

template<typename T, class Allocator>
void Location<T, Allocator>::erase(size_t index) {
    if (persist(index)) {
        data_allocator_t::destroy(data_alloc, pdata.get() + index);
        clear_bit(ppersist.get(), index);
        fill_size--;
    }
}

template<typename T, class Allocator>
T& Location<T, Allocator>::operator[](size_t index) {
    return *(pdata.get() + index);
}


template<typename T, class Allocator>
const T& Location<T, Allocator>::operator[](size_t index) const {
    return *(pdata.get() + index);
}
