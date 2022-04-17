/**
 * @file cuckoo_hashtable.h
 * @author Sergey Simonov
 * @brief Cuckoo hashing implementation with the interface close to
 * standard std::map and std::unordered_map interface
 **/

#pragma once

#include <type_traits>
#include <iostream>
#include <array>
#include <memory>
#include <utility>
#include <algorithm>
#include "location.h"


 /*******************************************************************************
  *                          Configuration templates                            *
  *******************************************************************************/
  /*  CHT = Cuckoo Hash Table */

struct CHT_Traits_Classic__ {};
struct CHT_Traits_Greed__ {};

enum class InsertMethod {
    classic, // Классическая вставка через перестановку (Rasmus Pagh original)
    greed // "Жадный" алгоритм для вставки
};

template<typename T>
struct CHT_Traits {};
template<>
struct CHT_Traits<CHT_Traits_Classic__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr size_t max_loop = 12;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::classic;
};

template<>
struct CHT_Traits<CHT_Traits_Greed__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr size_t max_loop = 24;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::greed;
};

using CHT_Traits_Greed = CHT_Traits<CHT_Traits_Greed__>;
using CHT_Traits_Classic = CHT_Traits<CHT_Traits_Classic__>;


/************************************************************************
 *                          CuckooHashTable                             *
 ************************************************************************/
template <
    typename Key, typename T, typename HashFunc, std::size_t E,
    typename Traits = CHT_Traits_Classic,
    typename Allocator = std::allocator<std::pair<const Key, T> >
>
class CuckooHashTable {
    static_assert(Traits::resize_factor > 1, "Traits::resize_factor incorrect value!");
    static_assert(Traits::max_loop >= E, "Traits::max_loop - incorrect value!");
    static_assert(Traits::initial_location_size > 1, "Traits::initial_location_size - incorrect value!");
private:
    using keyval_t = std::pair<const Key, T>;
    using keyval_mutable_t = std::pair<Key, T>;
    using hash_funcs_t = std::array<HashFunc, E>;
    using location_t = Location<keyval_t, Allocator>;
    using ins_route_t = std::array< std::pair<size_t, size_t>, Traits::max_loop* E>;
    using location_array_t = std::array< std::unique_ptr<location_t>, E>;

    hash_funcs_t hash_funcs;
    location_array_t locs;
    location_array_t tmp_locs; // используем во время рехеширования
    size_t tmp_loc_size; // используем во время рехеширования
    size_t rehash_tryes = false;
    size_t rehash_counter = 0;

public:
/*******************************************************************************
 *                          Iterators                                          *
 *******************************************************************************/
    template <typename TT>
    class iterator_base {
        friend CuckooHashTable;
    protected:
        location_array_t* plocs;
        size_t pos;
        typename location_t::iterator iter;
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = TT;
        using difference_type = int;
        using pointer = TT*;
        using reference = TT&;
        iterator_base(location_array_t* plocs, size_t pos, typename location_t::iterator iter) noexcept
            : plocs(plocs), pos(pos), iter(iter) {
            assert(plocs);
        }
        iterator_base(location_array_t* plocs, size_t pos, size_t index) noexcept
            : plocs(plocs), pos(pos), iter(typename location_t::iterator((*plocs)[pos].get(), index)) {
            assert(plocs);
        }
        iterator_base& operator=(const iterator_base& rv) {
            if (this == &rv)
                return *this;
            plocs = rv.plocs;
            pos = rv.pos;
            iter = rv.iter;
            return *this;
        };
        iterator_base& operator++() {
            ++iter;
            while (iter == (*plocs)[pos]->end() && pos < E - 1) {
                pos++;
                iter = (*plocs)[pos]->begin();
            }
            return *this;
        }
        iterator_base operator++(int) {
            auto prev_pos = pos;
            auto prev_iter = iter;
            operator++();
            return iterator_base(plocs, prev_pos, prev_iter);
        }
        reference operator*()  const { return *iter; }
        pointer operator->() const { return iter.operator->(); }
        bool operator==(const iterator_base& rv) const {
            return (iter == rv.iter);
        }
        bool operator!=(const iterator_base& rv) const { return iter != rv.iter; }
        // one way conversion: iterator -> const_iterator
        operator iterator_base<const TT>() const {
            return iterator_base<const TT>(plocs, pos, iter);
        };
    };

     using iterator = iterator_base<keyval_t>;

    iterator begin() {
        // Ищем первую не пустую локацию
        size_t i = 0;
        auto it = locs[i].get()->begin();
        while (it == locs[i].get()->end() && i < E - 1) {
            i++;
            it = locs[i].get()->begin();
        }
        return iterator(&locs, i, it);
    }

    iterator end() { return iterator(&locs, E - 1, locs[E - 1]->end()); }

    using const_iterator = iterator_base<const keyval_t>;

    const_iterator cbegin() { return begin(); }

    const_iterator cend() { return end(); }

private:
/*******************************************************************************
*            Private Methods                                                   *
*******************************************************************************/
    bool is_free_at_loc(size_t i, const Key& key, size_t& index_out) {
        index_out = hash_funcs[i](key) % locs[0]->max_size();
        return  !locs[i]->persist(index_out);
    };

    /**
     * return размер найденного пути или  0, если путь не найден
     */
    size_t find_route(const Key& key, ins_route_t& route, size_t deep, size_t ignore_loc = E) {
        if (deep >= Traits::max_loop) {
            return 0;
        }
        // Свободная локация для key?
        size_t hash_indexes[E];
        for (size_t i = 0; i < E; i++) {
            if ((i != ignore_loc) && is_free_at_loc(i, key, hash_indexes[i])) {
                route[deep] = { i, hash_indexes[i] };
                return deep + 1;
            }
        }
        // Свободная локация для key не найдена
        // - рекурсивно ищем свободную для элементов, которые занимают место
        for (size_t i = 0; i < E; i++) {
            if (i == ignore_loc) {
                continue;
            }
            auto other_key = locs[i]->get(hash_indexes[i])->first;
            route[deep] = { i, hash_indexes[i] };
            // Просматриваем другие локации для этого ключа
            auto ret = find_route(other_key, route, deep + 1, i);
            if (ret > 0) {
                return ret;
            }
        }
        return 0;
    }

    /**
      * @brief Вставка с выталкиванием "жадным" алгоритмом
      * @return iterator - место вставленного элемента, или end();
      */
    iterator insert_greed(const keyval_t& kv) {
        // Creating insertion list
        ins_route_t route;
        size_t evict_sz = find_route(kv.first, route, 0);
        if (evict_sz == 0) {
            return end();
        }
        for (auto to = evict_sz - 1; to > 0; to--) {
            auto from = to - 1;
            const auto pfrom = locs[route[from].first]->get(route[from].second);
            locs[route[to].first]->set(route[to].second, std::move(*pfrom));
            locs[route[from].first]->erase(route[from].second);
        }
        const auto& [loc_index, key_index] = route[0];
        locs[loc_index]->set(key_index, kv);
        return iterator(&locs, loc_index, key_index);
    }

    /**
     * @brief Вставка с классической схемой выталкивания
     * @return iterator - место вставленного элемента, или end();
     */
    iterator insert_classic(keyval_mutable_t& x) {
        size_t loop = 0;
        while (loop < Traits::max_loop) {
            loop++;
            for (size_t i = 0; i < E; i++) {
                size_t index;
                if (is_free_at_loc(i, x.first, index)) {
                    locs[i]->set(index, std::move(x));
                    return iterator(&locs, i, index);
                }
                else {
                    auto tmp = std::move((*locs[i])[index]);
                    locs[i]->set(index, std::move(x));
                    x = std::move(tmp);
                }
            }
        }
        return end();
    }

public:
    /*******************************************************************************
    *             Constructor and public Methods                                 *
    *******************************************************************************/
    CuckooHashTable(const hash_funcs_t& hash_funcs) :
        hash_funcs(hash_funcs) {
        for (size_t i = 0; i < E; i++) {
            locs[i].reset(new location_t(Traits::initial_location_size));
        }
    }

    size_t get_rehash_counter() const noexcept {
        return rehash_counter;
    }

    iterator find(const Key& key) noexcept {
        for (size_t i = 0; i < E; i++) {
            const auto hash = hash_funcs[i](key) % locs[0]->max_size();
            auto kv = locs[i]->get(hash);
            if (kv != nullptr && key == kv->first) {
                return iterator(&locs, i, hash);
            }
        }
        return end();
    }

    bool contains(const Key& key) noexcept {
        return find(key) != end();
    }

    size_t size() const noexcept {
        size_t sz = 0;
        for (const auto& ploc : locs) {
            sz += ploc->count();
        }
        return sz;
    }

    void erase(iterator it) {
        if (it.plocs == &locs && it != end()) {
            locs[it.pos]->erase(it.iter);
        }
    }

    void erase(const Key& key) {
        return erase(find(key));
    }

    /**
     * Возвращает ссылку на элемент, соотвествующий ключу key
     * Если в таблице отсутсвует ключ, то добавляет ключ и контруирует элемент T(), используя
     * DefaultConstructor, и возваращает ссылку на этот элемент
     */
    T& operator[](const Key& key) {
        if (!std::is_default_constructible_v<T>) {
            assert(false);
        };
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        else {
            return insert({ key, T() }).first->second;
        }
    }

    /** Вставка в ключа и значения в таблицу
    * @return iterator и boolean
    * iterator указывает на вставленный/существующей элемент
    * или = end(), если вставка завершена ошибкой
    * boolean = true элемент быставлен, false - ключ существовал, элемент не был вставлен.
    */
    std::pair<iterator, bool> insert(const keyval_t& kv) {
        auto iter = find(kv.first);
        if (iter != end()) {
            return { iter, false };
        }
        if constexpr (Traits::insert_method == InsertMethod::classic) {
            keyval_mutable_t kv_copy = kv;
            // создаем копию, так как insert_classic...
            iter = insert_classic(kv_copy);
            if (iter == end()) {
                // kv_copy содержит вытесненный элемент
                if (rehash()) {
                    auto ret = insert(kv_copy);
                    rehash_tryes = 0;
                    return ret;
                }
                else {
                    return { end(), false };
                }
            };
        }
        else if constexpr (Traits::insert_method == InsertMethod::greed) {
            iter = insert_greed(kv);
            if (iter == end()) {
                if (rehash()) {
                    auto ret = insert(kv);
                    rehash_tryes = 0;
                    return ret;
                }
            }
        }
        else {
            static_assert(Traits::insert_method != InsertMethod::classic
                && Traits::insert_method != InsertMethod::greed,
                "Unknown insert method in Traits");
        }
        return { iter, true };
    }


    bool rehash() {
        rehash_counter++;
        rehash_tryes++;
        if (rehash_tryes > Traits::max_rehash_tryes + 1) {
            lg0 << "Rehashing failed (limit exceed)..." << std::endl;
            rehash_tryes = 0;
            return false;
        }
        if (rehash_tryes > 1) {
            lg0 << "Rehashing one more time" << std::endl;
            tmp_loc_size = tmp_loc_size * Traits::resize_factor;
        }
        else {
            lg0 << "Rehashing..." << std::endl;
            tmp_loc_size = locs[0]->max_size() * Traits::resize_factor;
        }

        // переносим даннные во временный массив tmp_locs
        // основной массив  - пустой с увеличенной памятью
        for (size_t i = 0; i < E; i++) {
            tmp_locs[i].swap(locs[i]);
            locs[i].reset(new location_t(tmp_loc_size));
        }
        // вставка данных в новые локации
        for (size_t i = 0; i < E; i++) {
            for (auto& kv : *tmp_locs[i]) {
                if constexpr (Traits::insert_method == InsertMethod::classic) {
                    keyval_mutable_t kv_copy = kv;
                    if (insert_classic(kv_copy) == end()) {
                        // При встаавке снова нужно рехеширование
                        // Откат назад
                        for (size_t i = 0; i < E; i++) {
                            tmp_locs[i].swap(locs[i]);
                        }
                        return rehash();
                    }
                }
                else if constexpr (Traits::insert_method == InsertMethod::greed) {
                    if (insert_greed(kv) == end()) {
                        // При встаавке снова нужно рехеширование
                        // Откат назад
                        for (size_t i = 0; i < E; i++) {
                            tmp_locs[i].swap(locs[i]);
                        }
                        return rehash();
                    }
                }
            }
        }
        for (size_t i = 0; i < E; i++) {
            tmp_locs[i].reset();
        }
        return true;
    };

    float get_load() {
        return float(size()) / (E * locs[0]->max_size()) * 100;
    }

    void print() {
        for (size_t i = 0; i < E; i++) {
            std::cout << "\nLocation " << i << ":" << std::endl;
            auto loc_size = locs[0]->max_size();
            for (size_t j = 0; j < loc_size; j++) {
                if (locs[i]->persist(j)) {
                    const auto& kv = *(locs[i]->get(j));
                    std::cout << std::setw(3) << j << ": " << kv.first << "  -  " << kv.second << std::endl;
                }
                else {
                    std::cout << std::setw(3) << j << ": * " << std::endl;
                }
            }
        }
    }
};
