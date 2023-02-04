#pragma once

template<typename... T> constexpr bool always_false{};

template<typename K, typename V1, typename... V2> constexpr bool one_of(K key, V1 first, V2... rest)
{
    if constexpr (sizeof...(rest) == 0) {
        return key == first;
    } else {
        return key == first || one_of(key, rest...);
    }
}

/////////// Tests /////////////
static_assert(one_of(0, 0));
static_assert(!one_of(0, 1));
static_assert(one_of(0, 1, 2, 0, 3));
static_assert(one_of(0, 1, 2, 3, 0));
static_assert(!one_of(0, 1, 2, 3, 4));
