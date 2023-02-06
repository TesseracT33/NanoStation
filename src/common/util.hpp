#pragma once

#include <array>
#include <fstream>
#include <optional>
#include <vector>

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
//////////////////////////////

template<size_t size> std::optional<std::array<u8, size>> read_file_into_array(auto const& path)
{
    std::ifstream ifs{ path, std::ifstream::in | std::ifstream::binary };
    if (!ifs) {
        return {};
    }
    /* Test the file size */
    ifs.seekg(0, ifs.end);
    if (ifs.tellg() != size) {
        return {};
    }
    /* Read the file */
    std::array<u8, size> arr;
    ifs.seekg(0, ifs.beg);
    ifs.read((char*)arr.data(), size);
    return arr;
}

std::optional<std::vector<u8>> read_file_into_vector(auto const& path)
{
    std::ifstream ifs{ path, std::ifstream::in | std::ifstream::binary };
    if (!ifs) {
        return {};
    }

    std::vector<u8> vec;
    ifs.seekg(0, ifs.end);
    size_t size = ifs.tellg();
    vec.resize(size);

    ifs.seekg(0, ifs.beg);
    ifs.read((char*)vec.data(), size);

    return vec;
}

template<size_t> struct SizeToUInt {};

template<> struct SizeToUInt<1> {
    using type = u8;
};

template<> struct SizeToUInt<2> {
    using type = u16;
};

template<> struct SizeToUInt<4> {
    using type = u32;
};

template<> struct SizeToUInt<8> {
    using type = u64;
};

template<> struct SizeToUInt<16> {
    using type = u128;
};
