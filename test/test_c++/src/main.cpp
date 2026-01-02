#include <gp/config.hpp>
//
#include <print>
#include <type_traits>
#include <concepts>

template <typename T>
inline constexpr bool IsArithmeticTuple = false;

template <typename... Ts>
inline constexpr bool IsArithmeticTuple<std::tuple<Ts...>> = (std::is_arithmetic_v<Ts> && ...);

template <typename T>
concept ArithmeticTuple = IsArithmeticTuple<T>;

template <typename T, typename U>
concept CalculateDistance = requires(U vala, U valb) {
    { T::distance(vala, valb) };
};

template <typename T, ArithmeticTuple PointType>
    requires CalculateDistance<T, PointType>
class KDTree {
public:
private:
};

struct Point3D {
    using PointType = std::tuple<int, int, int>;

    static int distance(PointType, PointType);

private:
    int _a, _b, _c;
};

int main()
{
    KDTree<Point3D, std::tuple<int, int, int>> a;
    std::println("{}", IsArithmeticTuple<std::tuple<std::string>>);

    using A1 = std::tuple<int &, int>;
    using A2 = std::tuple<int, int>;
    std::println("A1->A2: {}, A2->A1: {}", std::is_convertible_v<A1, A2>,
                 std::is_convertible_v<A2, A1>);
    return 0;
}