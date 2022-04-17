#pragma once

#include <memory>
#include <iostream>
#include "optlog.h"
#include <string.h>
#include <vector>

// Location - хранилище - массив для данных типа T длиной SIZE
// В отличие от std::vector НЕ инициализирует память (не вызывает конструктор)
// до момента установки элемента массива.
// Контролирует установку элемента масива с помощью битового массива.
// Используется параметр Allocator, чтобы можно было кастомизировать выделение памяти.



template <typename T, typename Allocator = std::allocator<T> >
class Location {

    using data_allocator_t = std::allocator_traits<Allocator>;
    using char_allocator_t = std::allocator_traits< typename Allocator:: template rebind<uint8_t>::other >;

    Allocator data_alloc;
    typename Allocator:: template rebind<uint8_t>::other char_alloc;

    size_t size;
    const size_t chunk_size;

    struct Chunk {
        std::shared_ptr<uint8_t> ppersist;
        std::shared_ptr<T> pdata;
    };

    Chunk allocate_chunk();

    std::vector<Chunk> data;


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

public:
    Location(size_t size, size_t chunk_size);

    template<typename... Args>
    void emplace(size_t index, Args&&... args); //constructing in-place

    template<typename U>
    void set(size_t index, U&& val); // using  T copy- or move- constructor

    T* get(size_t index); // checking bounds and persist

    T& operator[](size_t index); // no bounds/persist checking

    inline bool persist(size_t i);

    // ? если нужно, то можно реализовать
    // size_t count(); // return number of constructed objects
};



template<typename T, class Allocator>
Location<T, Allocator>::Chunk Location<T, Allocator>::allocate_chunk() {
    Chunk chunk;

    // Persistance map allocation
    auto bitset_size = (chunk_size % 8 > 0) ? (chunk_size / 8 + 1) : (chunk_size / 8);
    ppersist.reset(char_allocator_t::allocate(char_alloc, bitset_size),
        [size, this](uint8_t* ptr) { char_allocator_t::deallocate(char_alloc, ptr, size); }
    );
    memset(static_cast<void*>(ppersist.get()), 0, bitset_size);

    // Data allocation
    pdata.reset(data_allocator_t::allocate(data_alloc, size),
        [pp = ppersist, size, &alloc = data_alloc](T* ptr) {
            // destroy every object, that where created
            for (size_t i = 0; i < size; i++) {
                if ( Location::get_bit(pp.get() , i) ) {
                    data_allocator_t::destroy(alloc, ptr + i);
                }
            }
            data_allocator_t::deallocate(alloc, ptr, size);
        }
    );
}


template<typename T, class Allocator>
Location<T, Allocator>::Location(size_t size, size_t chunk_size):size(size), chunk_size(chunk_size) {

    auto chunks = size / chunk_size + (size % chunk_size ? 1 : 0);

    for (int i = 0; i < chunks; i++) {


    }


}


template<typename T, class Allocator>
inline bool Location<T, Allocator>::persist(size_t i) {
    return i < size && Location::get_bit(ppersist.get(), i);
}


template<typename T, class Allocator>
template <typename... Args>
void Location<T, Allocator>::emplace(size_t index, Args&&... args) {
    if (persist(index)) {
        data_allocator_t::destroy(data_alloc, pdata.get() + index);
    }
    data_allocator_t::construct(data_alloc, pdata.get() + index, std::forward<Args...>(args)...);
    Location::set_bit(ppersist.get(), index);
}


template<typename T, class Allocator>
template<typename U>
void Location<T, Allocator>::set(size_t index, U&& val) {
    if (persist(index)) {
        data_allocator_t::destroy(data_alloc, pdata.get() + index);
    }
    data_allocator_t::construct(data_alloc, pdata.get() + index, std::forward<U>(val));
    Location::set_bit(ppersist.get(), index);
}


template<typename T, class Allocator>
T* Location<T, Allocator>::get(size_t index) {
    if (persist(index)) {
        return pdata.get() + index;
    }
    return nullptr;
}


template<typename T, class Allocator>
T& Location<T, Allocator>::operator[](size_t index) {
    return *(pdata.get() + index);
}
