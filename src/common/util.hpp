#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <fstream>
#include <string>
#include <type_traits>
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

inline std::expected<std::vector<u8>, std::string> read_file(auto const& path, size_t expected_file_size = 0)
{
    std::ifstream ifs{ path, std::ifstream::in | std::ifstream::binary };
    if (!ifs) {
        return std::unexpected("Could not open the file.");
    }
    ifs.seekg(0, ifs.end);
    size_t file_size = ifs.tellg();
    bool test_size = expected_file_size > 0;
    if (test_size && file_size != expected_file_size) {
        return std::unexpected(
          std::format("The file was of the wrong size; expected {}, got {}.", expected_file_size, file_size));
    }
    std::vector<u8> vec(file_size);
    ifs.seekg(0, ifs.beg);
    ifs.read(reinterpret_cast<char*>(vec.data()), file_size);
    return vec;
}

template<std::integral Int> constexpr auto to_signed(Int val)
{
    return static_cast<std::make_signed_t<Int>>(val);
}

template<std::integral Int> constexpr auto to_unsigned(Int val)
{
    return static_cast<std::make_unsigned_t<Int>>(val);
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

/////////// Tests /////////////
static_assert(one_of(0, 0));
static_assert(!one_of(0, 1));
static_assert(one_of(0, 1, 2, 0, 3));
static_assert(one_of(0, 1, 2, 3, 0));
static_assert(!one_of(0, 1, 2, 3, 4));
