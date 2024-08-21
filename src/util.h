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

//https://stackoverflow.com/questions/26351587/how-to-create-stdarray-with-initialization-list-without-providing-size-directl
template <typename... T>
constexpr auto make_array(T&&... values) ->
    std::array<
       typename std::decay<
           typename std::common_type<T...>::type>::type,
       sizeof...(T)> {
    return std::array<
        typename std::decay<
            typename std::common_type<T...>::type>::type,
        sizeof...(T)>{std::forward<T>(values)...};
}
