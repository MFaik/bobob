#pragma once

#include <array>
#include <unordered_map>
#include <string>

template<>
struct std::hash<std::array<int, 2>> {
    inline std::size_t operator()(const std::array<int, 2>& a) const noexcept {
        return (size_t)a[0] + (((size_t)a[1])<<32);
    }
};

int pow2roundup (int x);

std::string get_file_contents(const char *filename);
