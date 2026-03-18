#include <gp/config.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/algorithm/KDTree.hpp>

struct Point3D {
    static constexpr gp::alg::DimensionType Dimension = 3;

    using DistanceType = float;
    using PointType    = std::array<DistanceType, Dimension>;

    explicit operator PointType() const
    {
        PointType temp = {
            static_cast<float>(a[0]),
            static_cast<float>(a[1]),
            static_cast<float>(a[2]),
        };
        return temp;
    }

    static auto distance(PointType pa, PointType pb) -> DistanceType
    {
        const PointType temp = {pa[0] - pb[0], pa[1] - pb[1], pa[2] - pb[2]};
        return (temp[0] * temp[0]) + (temp[1] * temp[1]) + (temp[2] * temp[2]);
    }

    std::array<int, 3> a;
};

int main()
{
    gp::alg::KDTree<Point3D> aa;

    std::vector<Point3D> points = {
        {1, 1, 1},
        {2, 2, 2}
    };

    aa.setElement(points);
    aa.buildTree();
    std::println("Nearest Point's index: {}", aa.findKthNearest({1, 2, 1}, 1));
    std::println("Farthest Point's index: {}", aa.findKthFarthest({1, 2, 1}, 1));
    std::println("3th Nearest Point's index: {}", aa.findKthNearest({1, 2, 1}, 3));
    return 0;
}
