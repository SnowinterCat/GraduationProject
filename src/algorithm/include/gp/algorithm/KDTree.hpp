#pragma once
#include <gp/config.hpp>
#include <gp/__algorithm/__config.hpp>
// Standard Library
#include <type_traits>
#include <concepts>
#include <algorithm>
#include <limits>
#include <vector>
#include <array>
#include <queue>
#include <span>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_ALGORITHM_BEGIN

using IndexType     = int32_t;
using DimensionType = int32_t;

template <typename T>
concept HasDistanceType =
    std::is_arithmetic_v<typename T::DistanceType> && requires { typename T::DistanceType; };

template <typename T>
concept HasDimensionValue = std::integral<decltype(T::Dimension)> && T::Dimension > 0;

template <typename T>
concept CanConvertToPointType =
    std::convertible_to<std::remove_cvref_t<T>,
                        std::array<typename T::DistanceType, T::Dimension>> &&
    std::convertible_to<const std::remove_cvref_t<T>,
                        std::array<typename T::DistanceType, T::Dimension>>;

template <typename T>
concept HasDistanceFunction =
    std::is_arithmetic_v<std::invoke_result_t<
        decltype(&T::distance), std::array<typename T::DistanceType, T::Dimension>,
        std::array<typename T::DistanceType, T::Dimension>>> &&
    std::is_same_v<typename T::DistanceType,
                   std::invoke_result_t<decltype(&T::distance),
                                        std::array<typename T::DistanceType, T::Dimension>,
                                        std::array<typename T::DistanceType, T::Dimension>>>;

template <typename T>
concept KDTreeNodeAble = HasDistanceType<T> && HasDimensionValue<T> && CanConvertToPointType<T> &&
                         HasDistanceFunction<T>;

template <KDTreeNodeAble T>
class KDTree {
public:
    using ValueType    = T;
    using DistanceType = T::DistanceType;
    using PointType    = std::array<DistanceType, T::Dimension>;

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

    static constexpr auto DistanceMax = std::numeric_limits<DistanceType>::max();
    static constexpr auto DistanceMin = std::numeric_limits<DistanceType>::min();

public:
    void setElement(std::span<T> arr);
    void buildTree();
    auto findNearest(const T &ta, DistanceType disMax = DistanceMax) -> IndexType;
    auto findFarthest(const T &ta, DistanceType disMin = DistanceMin) -> IndexType;
    auto findKthNearest(const T &ta, int kth, DistanceType disMax = DistanceMax) -> IndexType;
    auto findKthFarthest(const T &ta, int kth, DistanceType disMin = DistanceMin) -> IndexType;

private:
    auto _buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType;
    auto _update(IndexType now) -> IndexType;
    void _findNearest(const T &ta, IndexType now, DistanceType disMax);
    void _findFarthest(const T &ta, IndexType now, DistanceType disMin);
    auto _minDis(const T &ta, IndexType now) -> DistanceType;
    auto _maxDis(const T &ta, IndexType now) -> DistanceType;

private:
    IndexType              _root;
    std::vector<PointNode> _pointNodes;
    std::vector<TreeNode>  _treeNodes;

    std::vector<IndexType> _dimensionMin[T::Dimension];
    std::vector<IndexType> _dimensionMax[T::Dimension];

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> _qMin;
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::less<>>    _qMax;
};

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
    for (DimensionType i = 0; i < T::Dimension; ++i) {
        _dimensionMin[i].resize(num);
        _dimensionMax[i].resize(num);
    }
}

template <KDTreeNodeAble T>
void KDTree<T>::buildTree()
{
    _root = _buildTree(0, IndexType(_pointNodes.size()), 0);
}

template <KDTreeNodeAble T>
auto KDTree<T>::findNearest(const T &ta, DistanceType disMax) -> IndexType
{
    return findKthNearest(ta, 1, disMax);
}

template <KDTreeNodeAble T>
auto KDTree<T>::findFarthest(const T &ta, DistanceType disMin) -> IndexType
{
    return findKthFarthest(ta, 1, disMin);
}

template <KDTreeNodeAble T>
auto KDTree<T>::findKthNearest(const T &ta, int kth, DistanceType disMax) -> IndexType
{
    while (!_qMax.empty()) {
        _qMax.pop();
    }
    while ((kth--) != 0) {
        _qMax.push({disMax, static_cast<IndexType>(-1)});
    }
    _findNearest(ta, _root, disMax);
    return _qMax.top().index;
}

template <KDTreeNodeAble T>
auto KDTree<T>::findKthFarthest(const T &ta, int kth, DistanceType disMin) -> IndexType
{
    while (!_qMin.empty()) {
        _qMin.pop();
    }
    while ((kth--) != 0) {
        _qMin.push({DistanceMin, static_cast<IndexType>(-1)});
    }
    _findFarthest(ta, _root, disMin);
    return _qMin.top().index;
}

// private

template <KDTreeNodeAble T>
auto KDTree<T>::_buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType
{
    auto lessCompare = [dim](const PointNode &aa, const PointNode &bb) {
        return static_cast<PointType>(aa.point)[dim] - static_cast<PointType>(bb.point)[dim] < 0;
    };
    IndexType mid = (left + right) >> 1;
    std::nth_element(_pointNodes.begin() + left, _pointNodes.begin() + mid,
                     _pointNodes.begin() + right, lessCompare);

    _treeNodes[mid].dim = dim;
    if (left < mid) {
        _treeNodes[mid].lson = _buildTree(left, mid, (dim + 1) % T::Dimension);
    }
    if (mid + 1 < right) {
        _treeNodes[mid].rson = _buildTree(mid + 1, right, (dim + 1) % T::Dimension);
    }
    return _update(mid);
}

template <KDTreeNodeAble T>
auto KDTree<T>::_update(IndexType now) -> IndexType
{
    for (DimensionType i = 0; i < T::Dimension; ++i) {
        _dimensionMin[i][now] = _dimensionMax[i][now] = now;
    }
    IndexType lson = _treeNodes[now].lson;
    IndexType rson = _treeNodes[now].rson;
    if (lson >= 0) {
        for (DimensionType i = 0; i < T::Dimension; ++i) {
            const auto &nowMin = _pointNodes[_dimensionMin[i][now]].point;
            const auto &nowMax = _pointNodes[_dimensionMax[i][now]].point;
            const auto &lMin   = _pointNodes[_dimensionMin[i][lson]].point;
            const auto &lMax   = _pointNodes[_dimensionMax[i][lson]].point;
            if (static_cast<PointType>(nowMin)[i] - static_cast<PointType>(lMin)[i] > 0) {
                _dimensionMin[i][now] = _dimensionMin[i][lson];
            }
            if (static_cast<PointType>(nowMax)[i] - static_cast<PointType>(lMax)[i] < 0) {
                _dimensionMax[i][now] = _dimensionMax[i][lson];
            }
        }
    }
    if (rson >= 0) {
        for (DimensionType i = 0; i < T::Dimension; ++i) {
            const auto &nowMin = _pointNodes[_dimensionMin[i][now]].point;
            const auto &nowMax = _pointNodes[_dimensionMax[i][now]].point;
            const auto &rMin   = _pointNodes[_dimensionMin[i][rson]].point;
            const auto &rMax   = _pointNodes[_dimensionMax[i][rson]].point;
            if (static_cast<PointType>(nowMin)[i] - static_cast<PointType>(rMin)[i] > 0) {
                _dimensionMin[i][now] = _dimensionMin[i][rson];
            }
            if (static_cast<PointType>(nowMax)[i] - static_cast<PointType>(rMax)[i] < 0) {
                _dimensionMax[i][now] = _dimensionMax[i][rson];
            }
        }
    }
    return now;
}

template <KDTreeNodeAble T>
void KDTree<T>::_findNearest(const T &ta, IndexType now, DistanceType disMax)
{
    if (now < 0) {
        return;
    }
    DistanceType nowDis = T::distance(ta, _pointNodes[now].point);
    if (nowDis < _qMax.top().distance) {
        _qMax.pop();
        _qMax.push({nowDis, _pointNodes[now].index});
    }
    IndexType    lson = _treeNodes[now].lson;
    IndexType    rson = _treeNodes[now].rson;
    DistanceType lDis = disMax;
    DistanceType rDis = disMax;
    if (lson != -1) {
        lDis = _minDis(ta, lson);
    }
    if (rson != -1) {
        rDis = _minDis(ta, rson);
    }
    if (lDis < rDis) {
        if (lDis <= _qMax.top().distance) {
            _findNearest(ta, lson, disMax);
        }
        if (rDis <= _qMax.top().distance) {
            _findNearest(ta, rson, disMax);
        }
    }
    else {
        if (rDis <= _qMax.top().distance) {
            _findNearest(ta, rson, disMax);
        }
        if (lDis <= _qMax.top().distance) {
            _findNearest(ta, lson, disMax);
        }
    }
}

template <KDTreeNodeAble T>
void KDTree<T>::_findFarthest(const T &ta, IndexType now, DistanceType disMin)
{
    if (now < 0) {
        return;
    }
    DistanceType nowDis = T::distance(ta, _pointNodes[now].point);
    if (nowDis > _qMin.top().distance) {
        _qMin.pop();
        _qMin.push({nowDis, _pointNodes[now].index});
    }
    IndexType    lson = _treeNodes[now].lson;
    IndexType    rson = _treeNodes[now].rson;
    DistanceType lDis = disMin;
    DistanceType rDis = disMin;
    if (lson != -1) {
        lDis = _maxDis(ta, lson);
    }
    if (rson != -1) {
        rDis = _maxDis(ta, rson);
    }
    if (lDis > rDis) {
        if (lDis >= _qMin.top().distance) {
            _findFarthest(ta, lson, disMin);
        }
        if (rDis >= _qMin.top().distance) {
            _findFarthest(ta, rson, disMin);
        }
    }
    else {
        if (rDis >= _qMin.top().distance) {
            _findFarthest(ta, rson, disMin);
        }
        if (lDis >= _qMin.top().distance) {
            _findFarthest(ta, lson, disMin);
        }
    }
}

template <KDTreeNodeAble T>
auto KDTree<T>::_minDis(const T &ta, IndexType now) -> DistanceType
{
    PointType    temp;
    DistanceType disP;
    DistanceType disQ;
    for (DimensionType i = 0; i < T::Dimension; ++i) {
        disP    = std::max(static_cast<PointType>(ta)[i] -
                               static_cast<PointType>(_pointNodes[_dimensionMax[i][now]].point)[i],
                           DistanceType(0));
        disQ    = std::max(static_cast<PointType>(_pointNodes[_dimensionMin[i][now]].point)[i] -
                               static_cast<PointType>(ta)[i],
                           DistanceType(0));
        temp[i] = disP + disQ;
    }
    return T::distance(temp, PointType{});
}

template <KDTreeNodeAble T>
auto KDTree<T>::_maxDis(const T &ta, IndexType now) -> DistanceType
{
    PointType    temp;
    DistanceType disP;
    DistanceType disQ;
    for (DimensionType i = 0; i < T::Dimension; ++i) {
        disP = static_cast<PointType>(ta)[i] -
               static_cast<PointType>(_pointNodes[_dimensionMin[i][now]].point)[i];
        disQ = static_cast<PointType>(ta)[i] -
               static_cast<PointType>(_pointNodes[_dimensionMax[i][now]].point)[i];
        temp[i] = std::abs(disP) > std::abs(disQ) ? disP : disQ;
    }
    return T::distance(temp, PointType{});
}

GP_ALGORITHM_END
GP_END