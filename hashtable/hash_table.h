#pragma once
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <list>
#include <vector>

template <typename Key, typename T, typename Hash = std::hash<Key>>
class HashTable {
public:
    // Do rehash when everage size of bucket is > max_load_factor
    inline static const float max_load_factor = 1.7f;
    inline static const size_t initial_buckets_size = 4;
    using KeyVal = std::pair<const Key, T>;
    using Bucket = std::list<KeyVal>;
    // Храним список unique_ptr, а не Bucket, т.к. пустой Bucket занимает больше места в памяти, чем unique_ptr
    using BucketVector = std::vector<std::unique_ptr<Bucket> >;
    inline static const Hash get_hash_code;

private:
    size_t buckets_count = 0;
    size_t elements_count = 0;
    BucketVector buckets;
    
    size_t bucket_index(size_t hashCode, size_t vector_size);
    std::pair<size_t, typename Bucket::iterator> find_keyval(const Key& key);
    void rehash();

public:
    HashTable() :buckets(initial_buckets_size) {};
    bool insert(const Key& key, const T& val);
    bool find(const Key& key);
    void erase(const Key& key);
    T& at(const Key& key);
    
    size_t size() {
        return elements_count;
    };

    auto get_load_factor() {
        return buckets_count > 0 ? float(elements_count) / buckets_count : 0;
    }

    auto get_buckets_count() {
        return buckets_count;
    }

    size_t get_vector_size() {
        return buckets.size();
    }

    class iterator {
        typename BucketVector::iterator vector_iter;
        typename BucketVector::iterator vector_end;
        typename Bucket::iterator bucket_iter;
    public:
        typedef typename KeyVal value_type;
        typedef typename KeyVal* pointer;
        typedef typename KeyVal& reference;

        iterator(typename BucketVector::iterator begin, typename BucketVector::iterator end)
            : vector_iter(begin), vector_end(end) {
            while (vector_iter != vector_end && *vector_iter == nullptr) {
                vector_iter++;
            }
            if (vector_iter != vector_end) {
                bucket_iter = (*vector_iter)->begin();
            }
        }

        KeyVal& operator*() {
            return *bucket_iter;
        };

        iterator& operator++() {
            if (vector_iter == vector_end) {
                return *this;
            }
            bucket_iter++;
            if (bucket_iter == (*vector_iter)->end()) {
                vector_iter++;
                while (vector_iter != vector_end && *vector_iter == nullptr) {
                    vector_iter++;
                }
                if (vector_iter != vector_end) {
                    bucket_iter = (*vector_iter)->begin();
                }
            }
            return *this;
        }

        bool operator != (const iterator& other) const {
            if (vector_iter != other.vector_iter) {
                return true;
            }
            else if (vector_iter != vector_end) {
                return (bucket_iter != other.bucket_iter);
            }
            return false;
        }
    };

    iterator begin() {
        return iterator(buckets.begin(), buckets.end());
    }

    iterator end() {
        return iterator(buckets.end(), buckets.end());
    }
};


/*******************************************************************************
 *                      Private functions inmplementation                      *
 *******************************************************************************/

/**
 * Преобразует HashCode в index вектора Buckets
 */
template <typename Key, typename T, typename Hash>
size_t HashTable<Key, T, Hash>::bucket_index(size_t hashCode, size_t vector_size) {
    assert(vector_size > 0);
    return hashCode % vector_size;
}

/**
 * Возвращает пару индекс в векторе (buckets) и итераторор в списке (bucket),
 * которые соотвествуеют ключу key.
 * @throw std::out_of_range, если ключ остуствует в таблице
 */
template <typename Key, typename T, typename Hash>
std::pair<size_t, typename HashTable<Key, T, Hash>::Bucket::iterator>
HashTable<Key, T, Hash>::find_keyval(const Key& key) {
    auto hash_code = get_hash_code(key);
    auto idx = bucket_index(hash_code, buckets.size());
    auto& bucket_uptr = buckets[idx];
    if (bucket_uptr == nullptr) {
        throw std::out_of_range("key not found");
    }
    auto& bucket = *bucket_uptr;
    auto it = std::find_if( bucket.begin(), bucket.end(),
                            [&key](const auto& kv) { return kv.first == key;});
    if (it == bucket.end()) {
        throw std::out_of_range("key not found");
    }
    return { idx, it };
}

/**
 * Рехеширвоание таблицы
 */
template <typename Key, typename T, typename Hash>
void HashTable<Key, T, Hash>::rehash() {
    auto size = buckets.size() * 2;
    BucketVector new_buckets(size);
    size_t new_buckets_count = 0;
   
    for (auto& kv : *this) {
        auto hash_code = get_hash_code(kv.first);
        auto idx = bucket_index(hash_code, size);
        auto& bucket_uptr = new_buckets[idx];
        if (bucket_uptr == nullptr) {
            bucket_uptr.reset(new Bucket);
            new_buckets_count++;
        }
        bucket_uptr->push_back(std::move(kv));
    }

    buckets.swap(new_buckets);
    buckets_count = new_buckets_count;
}


/*******************************************************************************
 *                       Public functions implementation                       *
 *******************************************************************************/


/** Вставка новой пары ключ/значение
 *  return true on success
 *  or false, if key is already exists in hash table
 * */
template <typename Key, typename T, typename Hash>
bool HashTable<Key, T, Hash>::insert(const Key& key, const T& val) {
    auto hash_code = get_hash_code(key);
    auto idx = bucket_index(hash_code, buckets.size());
    auto& bucket_uptr = buckets[idx];
    if (bucket_uptr == nullptr) {
        bucket_uptr.reset(new Bucket);
        buckets_count++;
    }

    // Chek inserted key is unique
    auto& bucket = *bucket_uptr;
    if (bucket.end() != std::find_if(bucket.begin(), bucket.end(),
        [&key](const auto& kv) { return kv.first == key;})) {
        return false;
    }

    bucket.emplace_front(key, val);
    elements_count += 1;

    if (get_load_factor() > max_load_factor) {
        rehash();
    }
    return true;
}


/**
 * @return true, если key присутствует в таблице
 */
template <typename Key, typename T, typename Hash>
bool HashTable<Key, T, Hash>::find(const Key& key) {
    auto hash_code = get_hash_code(key);
    auto idx = bucket_index(hash_code, buckets.size());
    auto& bucket_uptr = buckets[idx];
    if (bucket_uptr == nullptr) {
        return false;
    }
    auto& bucket = *bucket_uptr;
    if (bucket.end() != std::find_if(bucket.begin(), bucket.end(),
        [&key](const auto& kv) { return kv.first == key;})) {
        return true;
    }
    return false;
}


/**
 * Возвращает ссылку на значение для ключа key
 * @throw std::out_of_range, если ключа нет в таблице
 */
template <typename Key, typename T, typename Hash>
T& HashTable<Key, T, Hash>::at(const Key& key) {
    auto it = find_keyval(key).second;
    return (*it).second;
}


/**
 * Удаляет ключ и значение из таблицы
 * @throw std::out_of_range, если ключа нет в таблице
 */
template <typename Key, typename T, typename Hash>
void HashTable<Key, T, Hash>::erase(const Key& key) {
    auto& [idx, it] = find_keyval(key);
    buckets[idx]->erase(it);
    elements_count -= 1;
    if (buckets[idx]->empty()) {
        buckets[idx].reset();
        buckets_count -= 1;
    }
}