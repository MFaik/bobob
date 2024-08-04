#include <array>
#include <unordered_map>

template<>
struct std::hash<std::array<int, 2>> {
    std::size_t operator()(const std::array<int, 2>& a) const noexcept {
        //TODO: profile this
        return (size_t)a[0] + (size_t)a[1];
        // return (a[0] + (324723947 + a[1])) ^93485734985;
    }
};
