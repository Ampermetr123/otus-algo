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
#include <tuple>
#include <algorithm>
#include <deque>
#include "location.h"
#include "cuckoo_traits.h"


 /************************************************************************
  *                          Helpers                                     *
  ************************************************************************/

  // compile time pow
template<class T>
inline constexpr T ct_pow(const T base, unsigned int const exp) {
    return (exp == 0) ? 1 : (base * ct_pow(base, exp - 1));
}

static constexpr size_t evict_to_queue_size(unsigned int evict_sz, size_t locations_count) {
    if (locations_count > 2) {
        return  locations_count * (ct_pow((locations_count - 1), evict_sz) - 1) / (locations_count - 2);
    }
    else {
        return 2 * (evict_sz + 1);
    }
}

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
    static_assert(Traits::max_evict >= 1, "Traits::max_evict - incorrect value!");
    static_assert(Traits::initial_location_size > 2, "Traits::initial_location_size - incorrect value!");
    static_assert(Traits::insert_method == InsertMethod::classic ||
        Traits::insert_method == InsertMethod::greed_dfs ||
        Traits::insert_method == InsertMethod::greed_bfs,
        "Traits::insert_method - incorrect value!");
private:
    using keyval_t = std::pair<const Key, T>;
    using keyval_mutable_t = std::pair<Key, T>;
    using hash_funcs_t = std::array<HashFunc, E>;
    using location_t = Location<keyval_t, Allocator>;
    using ins_route_t = std::array< std::pair<size_t, size_t>, 2 * Traits::max_evict>;
    using location_array_t = std::array< std::unique_ptr<location_t>, E>;

    hash_funcs_t hash_funcs;
    location_array_t locs;
    size_t rehash_tryes = 0;
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
    size_t hash_index(size_t i, const Key& key) {
        return hash_funcs[i](key) % locs[0]->max_size();
    }

    bool is_free_at_loc(size_t i, const Key& key, size_t& index_out) {
        index_out = hash_index(i, key); // hash_funcs[i](key) % locs[0]->max_size();
        return  !locs[i]->persist(index_out);
    };
    /** Построение маршрута перемещения методов DFS
     * return размер найденного пути или  0, если путь не найден
     */
    size_t find_route_dfs(const Key& key, ins_route_t& route, size_t deep = 0, size_t ignore_loc = E) {
        if (deep >= Traits::max_evict) {
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
            auto ret = find_route_dfs(other_key, route, deep + 1, i);
            if (ret > 0) {
                return ret;
            }
        }
        return 0;
    }


    /** Построение маршрута перемещения методом BFS
      * return размер найденного пути или  0, если путь не найден
      */
    size_t find_route_bfs(const Key& key, ins_route_t& route) {
        using bfs_node_t = std::tuple<size_t, size_t, size_t, size_t>;
        using bfs_queue_t =  std::array< bfs_node_t, evict_to_queue_size(Traits::max_evict, E)>;
        bfs_queue_t bfs_queue;
        size_t qhead = 0;
        size_t qtail = 0;
        for (size_t i = 0; i < E; i++) {
            bfs_queue[qtail] = std::make_tuple(0, 0, i, hash_index(i, key));
            qtail++;
        }
        while (qhead < qtail) {
            auto [deep, parent_index, i, h] = bfs_queue[qhead];
            if (!locs[i]->persist(h)) {
                auto ri = deep;
                while (true) {
                    route[ri] = { std::get<2>(bfs_queue[qhead]), std::get<3>(bfs_queue[qhead]) };
                    parent_index = std::get<1>(bfs_queue[qhead]);
                    if (ri == 0) {
                        break;
                    }
                    qhead = parent_index;
                    ri--;
                };
                return deep + 1;
            }
            // TODO alternative - оганичивать по глубине
            else if (qtail + E - 1 < evict_to_queue_size(Traits::max_evict, E)) {
                // Другие локации для этого ключа ставим в очередь
                const auto& new_key = locs[i]->get(h)->first;
                const auto  parent = qhead;
                for (size_t j = 0; j < E; j++) {
                    if (j == i) {
                        continue;
                    }
                    bfs_queue[qtail] = std::make_tuple(deep + 1, parent, j, hash_index(j, new_key));
                    qtail++;
                }
            }
            qhead++;
        }
        return 0;
    }


    /**
      * @brief Вставка с выталкиванием "жадным" алгоритмом DFS
      * @return iterator - место вставленного элемента, или end();
      */
    iterator insert_greed(const keyval_t& kv) {
        // Creating insertion list
        ins_route_t route;
        size_t evict_sz;
        if constexpr (Traits::insert_method == InsertMethod::greed_bfs) {
            evict_sz = find_route_bfs(kv.first, route);
        }
        else if constexpr (Traits::insert_method == InsertMethod::greed_dfs) {
            evict_sz = find_route_dfs(kv.first, route);
        }
        else {
            static_assert(Traits::insert_method != InsertMethod::greed_dfs
                && Traits::insert_method != InsertMethod::greed_bfs,
                "Incorrect insert method in Traits");
        }

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
     * @return iterator - место вставленного элемента,
     *          или end() - не удвачная вставка;
     *  При неудачной вставке содержит элемент,
     *  который бтолкнут
     */
    iterator insert_classic(keyval_mutable_t& x) {
        size_t evict = 0;
        while (evict < Traits::max_evict) {
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
                    evict++;
                }
            }
        }
         return end();
    }

    std::pair<iterator, bool> rehash_and_insert(const keyval_t& kv) {
        if (rehash_tryes >= Traits::max_rehash_tryes) {
            rehash_tryes = 0;
            return { end(), false };
        }

        rehash_tryes++;
        const auto sz = size();
        auto new_sz = static_cast<size_t>(sz * rehash_tryes * Traits::resize_factor);
        if (new_sz <= sz) {
            new_sz = sz + 1;
        }

        if (rehash(new_sz)) {
            auto ret = insert(kv);  // coul make recuresive call rehash_and_insert
            rehash_tryes = 0;
            return ret;
        }
        else {
            return rehash_and_insert(kv);
        }
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
            const auto hi = hash_index(i, key);
            auto kv = locs[i]->get(hi);
            if (kv != nullptr && key == kv->first) {
                return iterator(&locs, i, hi);
            }
        }
        return end();
    }

    bool count(const Key& key) noexcept {
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


    std::pair<iterator, bool> insert(const keyval_t& kv) {
        auto iter = find(kv.first);
        if (iter != end()) {
            return { iter, false };
        }
        if constexpr (Traits::insert_method == InsertMethod::classic) {
            keyval_mutable_t x = kv;
            iter = insert_classic(x);
            if (iter == end()) {
                return rehash_and_insert(x);
            }
        }
        else {  // InsertMethod::greed_dfs or InsertMethod::greed_bfs
            iter = insert_greed(kv);
            if (iter == end()) {
                return rehash_and_insert(kv);
            }
        }
        return { iter, true };
    }


    bool rehash(size_t new_loc_size) {
        if (new_loc_size < Traits::initial_location_size) {
            return false;
        }
        rehash_counter++;
        location_array_t tmp_locs;
        // переносим даннные во временный массив tmp_locs
        // основной массив - пустой с увеличенной памятью
        for (size_t i = 0; i < E; i++) {
            tmp_locs[i].swap(locs[i]);
            locs[i].reset(new location_t(new_loc_size));
        }

        // вставка данных в новые локации
        bool rehash_result = true;
        for (size_t i = 0; i < E; i++) {
            for (const auto& kv : *tmp_locs[i]) {
                if constexpr (Traits::insert_method == InsertMethod::classic) {
                    keyval_mutable_t x = kv;
                    if (insert_classic(x) == end()) {
                        rehash_result = false;
                        break;
                    }
                }
                else {  // InsertMethod::greed_dfs or InsertMethod::greed_bfs
                    if (insert_greed(kv) == end()) {
                        rehash_result = false;
                        break;
                    }
                }
            }
        }

        if (rehash_result == false) {
            for (size_t i = 0; i < E; i++) {
                tmp_locs[i].swap(locs[i]);
            }
        };
        for (size_t i = 0; i < E; i++) {
            tmp_locs[i].reset();
        }
        return rehash_result;
    };


    float get_load() {
        return float(size()) / (E * locs[0]->max_size()) * 100;
    }

};
