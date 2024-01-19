#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using String = std::string;
using StringView = std::string_view;

template <typename T>
using Vector = std::vector<T>;
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;