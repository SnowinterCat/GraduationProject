// Use this question for testing: https://www.luogu.com.cn/problem/P2093
#include <gp/config.hpp>
// Standard Library
#include <print>
#include <iostream>
// System Library
// Third-Party Library
// Local Library
#include <gp/algorithm/KDTree.hpp>

struct Point2D {
    using DistanceType = int64_t;

    static constexpr gp::alg::DimensionType Dimension = 2;

    static auto distance(const std::array<DistanceType, Dimension> &aa,
                         const std::array<DistanceType, Dimension> &bb) -> DistanceType
    {
        DistanceType disX = aa[0] - bb[0];
        DistanceType disY = aa[1] - bb[1];
        return (disX * disX) + (disY * disY);
    }

    auto     operator<=>(const Point2D &) const = default;
    explicit operator std::array<DistanceType, Dimension>() const
    {
        return {static_cast<DistanceType>(x), static_cast<DistanceType>(y)};
    }

    int64_t x;
    int64_t y;
};

auto main() -> int
{
    auto num    = int();
    auto cnt    = int();
    auto points = std::vector<Point2D>();
    std::cin >> num;
    points.resize(num);
    for (int i = 0; i < num; ++i) {
        std::cin >> points[i].x >> points[i].y;
    }

    gp::alg::KDTree<Point2D> tree;
    tree.setElement(points);
    tree.buildTree();
    std::cin >> cnt;
    auto px = int64_t();
    auto py = int64_t();
    auto pk = int();
    for (int i = 0; i < cnt; ++i) {
        std::cin >> px >> py >> pk;
        auto index = tree.findKthFarthest(Point2D{.x = px, .y = py}, pk);
        std::println("{}", index + 1);
    }
    return 0;
}