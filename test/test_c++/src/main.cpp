#include <gp/config.hpp>
//
#include <print>
#include <type_traits>
#include <concepts>
#include <limits>
#include <algorithm>
#include <vector>
#include <queue>
#include <span>

using IndexType     = int32_t;
using DimensionType = int32_t;

// template <typename T>
// inline constexpr bool IsArithmeticTuple = false;
// template <typename... Ts>
// inline constexpr bool IsArithmeticTuple<std::tuple<Ts...>> = (std::is_arithmetic_v<Ts> && ...);

// template <typename T>
// concept ArithmeticTuple = IsArithmeticTuple<T>;

template <typename T>
concept HasDistanceFunction =
    std::is_arithmetic_v<std::invoke_result_t<decltype(&T::distance), T, T>> &&
    requires(T vala, T valb) {
        { T::distance(vala, valb) };
    };

template <typename T>
concept HasMinusFunction =
    std::is_arithmetic_v<std::invoke_result_t<decltype(&T::minus), T, T, DimensionType>> &&
    requires(T vala, T valb, DimensionType dim) {
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

    static constexpr DimensionType Dimension   = T::Dimension;
    static constexpr auto          DistanceMax = std::numeric_limits<DistanceType>::max();
    static constexpr auto          DistanceMin = std::numeric_limits<DistanceType>::min();

public:
    void setElement(std::span<T> arr);
    void buildTree();
    auto findNearest(const T &ta, DistanceType disInf = DistanceMax) -> IndexType;
    auto findFarthest(const T &ta) -> IndexType;
    auto findKthNearest(const T &ta, int kth, DistanceType disInf = DistanceMax) -> IndexType;
    auto findKthFarthest(const T &ta, int kth) -> IndexType;

private:
    auto _buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType;
    auto _update(IndexType now) -> IndexType;
    void _findNearest(const T &ta, IndexType now, DistanceType disInf);
    void _findFarthest(const T &ta, IndexType now);

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
    static constexpr DimensionType Dimension = 3;

    static int distance(Point3D pa, Point3D pb)
    {
        return ((pa.a - pb.a) * (pa.a - pb.a)) + ((pa.b - pb.b) * (pa.b - pb.b)) +
               ((pa.c - pb.c) * (pa.c - pb.c));
    }
    static int minus(Point3D pa, Point3D pb, DimensionType dim)
    {
        switch (dim) {
        case 0:
            return pa.a - pb.a;
        case 1:
            return pa.b - pb.b;
        case 2:
            return pa.c - pb.c;
        default:
            return 0;
        }
    }

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
    aa.findKthNearest({1, 1, 1}, 1);
    aa.findKthFarthest({1, 1, 1}, 1);

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
    const IndexType num = arr.size();
    _pointNodes.resize(num);
    for (IndexType i = 0; i < IndexType(num); ++i) {
        _pointNodes[i] = {arr[i], i};
    }
    _treeNodes.resize(num);
    for (IndexType i = 0; i < IndexType(num); ++i) {
        _treeNodes[i] = {-1, -1, -1};
    }
    for (DimensionType i = 0; i < Dimension; ++i) {
        _dimensionMin[i].resize(num), _dimensionMin[i].resize(num);
    }
}

template <KDTreeNodeAble T>
void KDTree<T>::buildTree()
{
    _root = _buildTree(0, IndexType(_pointNodes.size()), 0);
}

// private

template <KDTreeNodeAble T>
auto KDTree<T>::_buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType
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

template <KDTreeNodeAble T>
auto KDTree<T>::_update(IndexType now) -> IndexType
{
    for (DimensionType i = 0; i < Dimension; ++i) {
        _dimensionMin[i][now] = _dimensionMax[i][now] = now;
    }
    IndexType lson = _treeNodes[now].lson;
    IndexType rson = _treeNodes[now].rson;
    if (lson >= 0) {
        for (DimensionType i = 0; i < Dimension; ++i) {
            if (T::minus(_pointNodes[_dimensionMin[i][now]].point,
                         _pointNodes[_dimensionMin[i][lson]].point, i) > 0) {
                _dimensionMin[i][now] = _dimensionMin[i][lson];
            }
            if (T::minus(_pointNodes[_dimensionMax[i][now]].point,
                         _pointNodes[_dimensionMax[i][lson]].point, i) < 0) {
                _dimensionMax[i][now] = _dimensionMax[i][lson];
            }
        }
    }
    if (rson >= 0) {
        for (DimensionType i = 0; i < Dimension; ++i) {
            if (T::minus(_pointNodes[_dimensionMin[i][now]].point,
                         _pointNodes[_dimensionMin[i][rson]].point, i) > 0) {
                _dimensionMin[i][now] = _dimensionMin[i][rson];
            }
            if (T::minus(_pointNodes[_dimensionMax[i][now]].point,
                         _pointNodes[_dimensionMax[i][rson]].point, i) < 0) {
                _dimensionMax[i][now] = _dimensionMax[i][rson];
            }
        }
    }
    return now;
}

template <KDTreeNodeAble T>
auto KDTree<T>::findNearest(const T &ta, DistanceType disInf) -> IndexType
{
    return findKthNearest(ta, 1, disInf);
}

template <KDTreeNodeAble T>
auto KDTree<T>::findFarthest(const T &ta) -> IndexType
{
    return findKthFarthest(ta, 1);
}

template <KDTreeNodeAble T>
auto KDTree<T>::findKthNearest(const T &ta, int kth, DistanceType disInf) -> IndexType
{
    while (!_qMax.empty()) {
        _qMax.pop();
    }
    while ((kth--) != 0) {
        _qMax.push({disInf, static_cast<IndexType>(-1)});
    }
    _findNearest(ta, _root, disInf);
    return _qMax.top().index;
}

template <KDTreeNodeAble T>
auto KDTree<T>::findKthFarthest(const T &ta, int kth) -> IndexType
{
    while (!_qMin.empty()) {
        _qMin.pop();
    }
    while ((kth--) != 0) {
        _qMin.push({DistanceMin, static_cast<IndexType>(-1)});
    }
    _findFarthest(ta, _root);
    return _qMin.top().index;
}

template <KDTreeNodeAble T>
void KDTree<T>::_findNearest(const T &ta, IndexType now, DistanceType disInf)
{
    ;
}

template <KDTreeNodeAble T>
void KDTree<T>::_findFarthest(const T &ta, IndexType now)
{
    ;
}