#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>

using String = std::string;
using StringView = std::string_view;

template <typename T>
using Vector = std::vector<T>;
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;