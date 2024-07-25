#include <array>
#include <unordered_map>

template<>
struct std::hash<std::array<int, 2>> {
    std::size_t operator()(const std::array<int, 2>& a) const noexcept {
        return (a[0] + (324723947 + a[1])) ^93485734985;
    }
};
