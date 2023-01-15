#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace Brilliant
{
    namespace Graph
    {
        struct vector {};
        struct set {};
        struct hash_set {};
        struct multiset {};
        struct hash_multiset {};
        struct map {};
        struct hash_map {};
        struct multimap {};
        struct hash_multimap {};

        template<class Tag, class Key, class Value = Key> struct StorageGen {};

        template<class Key> struct StorageGen<vector, Key> { using type = std::vector<Key>; };

        template<class Key> struct StorageGen<set, Key> { using type = std::set<Key>; };
        template<class Key> struct StorageGen<hash_set, Key> { using type = std::unordered_set<Key>; };
        template<class Key> struct StorageGen<multiset, Key> { using type = std::multiset<Key>; };
        template<class Key> struct StorageGen<hash_multiset, Key> { using type = std::unordered_multiset<Key>; };

        template<class Key, class Value> struct StorageGen<map, Key, Value> { using type = std::map<Key, Value>; };
        template<class Key, class Value> struct StorageGen<hash_map, Key, Value> { using type = std::unordered_map<Key, Value>; };
        template<class Key, class Value> struct StorageGen<multimap, Key, Value> { using type = std::multimap<Key, Value>; };
        template<class Key, class Value> struct StorageGen<hash_multimap, Key, Value> { using type = std::unordered_multimap<Key, Value>; };

        template<class Tag, class Key, class Value = Key>
        using storage_gen_t = typename StorageGen<Tag, Key, Value>::type;
    }
}