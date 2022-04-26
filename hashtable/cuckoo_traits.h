#pragma once
#include <stddef.h>

/*******************************************************************************
  *                          Configuration templates                            *
  *******************************************************************************/
  /*  CHT = Cuckoo Hash Table */

enum class InsertMethod {
    classic,    // Классическая вставка через перестановку (Rasmus Pagh original)
    greed_dfs,  // "Жадный" алгоритм для вставки DFS
    greed_bfs   // "Жадный" алгоритм для вставки BFS
};

/**
 * Максимальное количество операций сравнения
 *  при поиске места вставки  (N = max_loop)
 * Для алгоритма Сlassic:
 *  S = E * N
 * Для алгоритма Greed (DFS/BFS):
 * (E = 2) =>  S = 2 * N
 * (E > 2) => S = E *((E-1)^N)-1/(E-2)
 *            S(E=3) = 3*(2^N-1)
 *            S(E=4) = 2*(3^N-1)
 **/

template<typename T>
struct CHT_Traits {};


struct CHT_Traits_Classic__ {};
template<>
struct CHT_Traits<CHT_Traits_Classic__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr unsigned max_evict = 33;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::classic;
};
using CHT_Traits_Classic = CHT_Traits<CHT_Traits_Classic__>;


struct CHT_Traits_Greed_BFS2__ {};
template<>
struct CHT_Traits<CHT_Traits_Greed_BFS2__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr unsigned max_evict = 33 ;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::greed_bfs;
};
using CHT_Traits_Greed_BFS2 = CHT_Traits<CHT_Traits_Greed_BFS2__>;


struct CHT_Traits_Greed_DFS2__ {};
template<>
struct CHT_Traits<CHT_Traits_Greed_DFS2__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr unsigned max_evict = 33;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::greed_dfs;
};
using CHT_Traits_Greed_DFS2 = CHT_Traits<CHT_Traits_Greed_DFS2__>;


struct CHT_Traits_Greed_BFS__ {};
template<>
struct CHT_Traits<CHT_Traits_Greed_BFS__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr unsigned max_evict = 4 ;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::greed_bfs;
};
using CHT_Traits_Greed_BFS = CHT_Traits<CHT_Traits_Greed_BFS__>;


struct CHT_Traits_Greed_DFS__ {};
template<>
struct CHT_Traits<CHT_Traits_Greed_DFS__> {
    static inline constexpr float resize_factor = 1.5;
    static inline constexpr unsigned max_evict = 4;
    static inline constexpr size_t initial_location_size = 10;
    static inline constexpr size_t max_rehash_tryes = 3;
    static inline constexpr auto insert_method = InsertMethod::greed_dfs;
};
using CHT_Traits_Greed_DFS = CHT_Traits<CHT_Traits_Greed_DFS__>;
