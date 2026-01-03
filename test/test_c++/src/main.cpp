#include <gp/config.hpp>
//
#include <print>
#include <type_traits>
#include <concepts>
#include <algorithm>
#include <vector>
#include <queue>
#include <span>

using IndexType     = int32_t;
using DimensionType = int32_t;

template <typename T>
inline constexpr bool IsArithmeticTuple = false;

template <typename... Ts>
inline constexpr bool IsArithmeticTuple<std::tuple<Ts...>> = (std::is_arithmetic_v<Ts> && ...);

template <typename T>
concept ArithmeticTuple = IsArithmeticTuple<T>;

template <typename T>
concept HasDistanceFunction = requires(T vala, T valb) {
    { T::distance(vala, valb) };
};

template <typename T>
concept HasMinusFunction = requires(T vala, T valb, DimensionType dim) {
    { T::minus(vala, valb, dim) };
};

template <typename T>
concept SameReturnOfDistanceAndMinusFunction =
    std::is_same_v<std::invoke_result_t<decltype(&T::distance), T, T>,
                   std::invoke_result_t<decltype(&T::minus), T, T, DimensionType>>;

template <typename T>
concept HasStaticDimensionValue = T::Dimension > 0;

template <typename T>
concept KDTreeNodeAble = HasDistanceFunction<T> && HasStaticDimensionValue<T> &&
                         HasMinusFunction<T> && SameReturnOfDistanceAndMinusFunction<T>;

template <KDTreeNodeAble T>
class KDTree {
    using DistanceType = std::invoke_result_t<decltype(&T::distance), T, T>;
    // using PointNode = std::tuple<PointType, IndexType>; // point, index
    // using TreeNode = std::tuple<IndexType, IndexType, DimensionType>; // lson, rson, dimension
    // using HeapNode = std::tuple<DistanceType, IndexType>; // distance, index

public:
    struct PointNode {
        T         point;
        IndexType index;
        auto      operator<=>(const PointNode &other) const = default;
    };
    struct TreeNode {
        IndexType     lson;
        IndexType     rson;
        DimensionType dim; // dimension
        auto          operator<=>(const TreeNode &other) const = default;
    };
    struct HeapNode {
        DistanceType distance;
        IndexType    index;
        auto         operator<=>(const HeapNode &other) const = default;
    };

    static constexpr DimensionType Dimension = T::Dimension;

public:
    void setElement(std::span<T> arr);
    void buildTree();

private:
    auto _buildTree(int left, int right, int dim) -> IndexType;

private:
    IndexType              _root;
    std::vector<PointNode> _pointNodes;
    std::vector<TreeNode>  _treeNodes;

    std::vector<IndexType> _dimensionMin[Dimension];
    std::vector<IndexType> _dimensionMax[Dimension];

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::less<>>    _qMin;
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> _qMax;
};

struct Point3D {
    static constexpr int Dimension = 3;

    static int distance(Point3D, Point3D);
    static int minus(Point3D, Point3D, DimensionType);

    // private:
    int a, b, c;
};

int main()
{
    KDTree<Point3D> aa;

    std::vector<Point3D> points = {
        {1, 1, 1},
        {2, 2, 2}
    };
    aa.setElement(points);
    aa.buildTree();

    std::vector<int> bb = {1, 5, 4, 2, 3, 7, 6};
    std::nth_element(bb.begin(), bb.begin() + 4, bb.end());
    for (const auto &bbb : bb) {
        std::print("{} ", bbb);
    }
    std::println();

    std::println("{}", decltype(aa)::Dimension);
    using A1 = std::tuple<int &, int>;
    using A2 = std::tuple<int, int>;
    std::println("A1->A2: {}, A2->A1: {}", std::convertible_to<A1, A2>,
                 std::convertible_to<A2, A1>);
    return 0;
}

// public

template <KDTreeNodeAble T>
void KDTree<T>::setElement(std::span<T> arr)
{
    _pointNodes.resize(arr.size());
    for (IndexType i = 0; i < arr.size(); ++i) {
        _pointNodes[i] = {arr[i], i};
    }

    _treeNodes.resize(arr.size());
    for (IndexType i = 0; i < arr.size(); ++i) {
        _treeNodes[i] = {-1, -1, -1};
    }
}

template <KDTreeNodeAble T>
void KDTree<T>::buildTree()
{
    const IndexType n = _pointNodes.size(); // NOLINT

    _root = _buildTree(0, n, 0);
}

// private

template <KDTreeNodeAble T>
auto KDTree<T>::_buildTree(int left, int right, int dim) -> IndexType
{
    auto lessCompare = [dim](const PointNode &aa, const PointNode &bb) {
        return T::minus(aa.point, bb.point, dim) < 0;
    };
    IndexType mid = (left + right) >> 1;
    std::nth_element(_pointNodes.begin() + left, _pointNodes.begin() + mid,
                     _pointNodes.begin() + right, lessCompare);

    _treeNodes[mid].dim = dim;
    if (left < mid) {
        _treeNodes[mid].lson = _buildTree(left, mid, (dim + 1) % Dimension);
    }
    if (mid + 1 < right) {
        _treeNodes[mid].rson = _buildTree(mid + 1, right, (dim + 1) % Dimension);
    }
    return Update(mid);
}