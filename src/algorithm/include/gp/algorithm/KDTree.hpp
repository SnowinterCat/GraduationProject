#pragma once
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

template <typename T, typename U>
concept CanExplicitConvertTo = requires { static_cast<U>(std::declval<T>()); };

template <typename T>
concept CanExplicitConvertToPointType =
    CanExplicitConvertTo<std::remove_cvref_t<T>,
                         std::array<typename T::DistanceType, T::Dimension>> &&
    CanExplicitConvertTo<const std::remove_cvref_t<T>,
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
concept KDTreeNodeAble = HasDistanceType<T> && HasDimensionValue<T> &&
                         CanExplicitConvertToPointType<T> && HasDistanceFunction<T>;

template <KDTreeNodeAble T>
class KDTree {
public:
    using ValueType    = T;
    using DistanceType = T::DistanceType;
    using PointType    = std::array<DistanceType, T::Dimension>;

    struct PointNode {
        T         point; // 数据点
        IndexType index; // 记录点中的数据点在源视图中的索引
        auto      operator<=>(const PointNode &other) const = default;
    };
    struct TreeNode {
        IndexType     lson; // 树节点的左孩子
        IndexType     rson; // 树节点的右孩子
        DimensionType dim;  // 树节点通过 dim 维度对孩子进行拆分
        auto          operator<=>(const TreeNode &other) const = default;
    };
    struct HeapNode {
        DistanceType distance; // 距离/费用
        IndexType    index;    // 记录点中的数据点在源视图中的索引
        auto         operator<=>(const HeapNode &other) const = default;
    };

    static constexpr auto DistanceMax = std::numeric_limits<DistanceType>::max();
    static constexpr auto DistanceMin = std::numeric_limits<DistanceType>::min();

public:
    /**
     * @brief 设置需要使用 KDTree 查询的数据
     *
     * @param arr T 类型的一组数据点的视图
     * @details 由于需要对数据点排序，视图中的数据点会被完全复制一份。
                但请不要清理源视图指向的数据，查询时返回的索引是源视图中的索引
     */
    void setElement(std::span<T> arr);
    /**
     * @brief 构建 KDTree
     *
     * @details 需要使用 setElement 函数设置完数据之后，再调用此函数
     */
    void buildTree();
    /**
     * @brief 查询与 ta 数据点最临近的数据点在源视图中的索引号
     *
     * @param ta 一个 T 类型的任意数据点
     * @param disMax 距离上限，默认是 std::numeric_limits<DistanceType>::max()
     * @return 最临近的数据点在源视图中的索引号
     */
    auto findNearest(const T &ta, DistanceType disMax = DistanceMax) -> IndexType;
    /**
     * @brief 查询与 ta 数据点最疏远的数据点在源视图中的索引号
     *
     * @param ta 一个 T 类型的任意数据点
     * @param disMin 距离下限，默认是 std::numeric_limits<DistanceType>::min()
     * @return 最疏远的数据点在源视图中的索引号
     */
    auto findFarthest(const T &ta, DistanceType disMin = DistanceMin) -> IndexType;
    /**
     * @brief 查询与 ta 数据点第 kth 临近的数据点在源视图中的索引号
     *
     * @param ta 一个 T 类型的任意数据点
     * @param kth 查询第 kth 临近
     * @param disMax 距离上限，默认是 std::numeric_limits<DistanceType>::max()
     * @return 第 kth 临近的数据点在源视图中的索引号
     */
    auto findKthNearest(const T &ta, int kth, DistanceType disMax = DistanceMax) -> IndexType;
    /**
     * @brief 查询与 ta 数据点第 kth 疏远的数据点在源视图中的索引号
     *
     * @param ta 一个 T 类型的任意数据点
     * @param kth 查询第 kth 疏远
     * @param disMin 距离下限，默认是 std::numeric_limits<DistanceType>::min()
     * @return 第 kth 疏远的数据点在源视图中的索引号
     */
    auto findKthFarthest(const T &ta, int kth, DistanceType disMin = DistanceMin) -> IndexType;

protected:
    auto _buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType;
    auto _update(IndexType now) -> IndexType;
    void _findNearest(const T &ta, IndexType now, DistanceType disMax);
    void _findFarthest(const T &ta, IndexType now, DistanceType disMin);
    auto _minDis(const T &ta, IndexType now) -> DistanceType;
    auto _maxDis(const T &ta, IndexType now) -> DistanceType;

protected:
    IndexType              _root;       // KDTree 根节点索引号
    std::vector<PointNode> _pointNodes; // KDTree 记录点信息
    std::vector<TreeNode>  _treeNodes;  // KDTree 树节点信息

    std::array<std::vector<IndexType>, T::Dimension> _dimensionMin; // KDTree 树节点各维度最小记录点
    std::array<std::vector<IndexType>, T::Dimension> _dimensionMax; // KDTree 树节点各维度最大记录点

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> _qMin; // 临近点查询的堆
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::less<>>    _qMax; // 疏远点查询的堆
};

// public

template <KDTreeNodeAble T>
void KDTree<T>::setElement(std::span<T> arr)
{
    const auto num = static_cast<IndexType>(arr.size());
    _pointNodes.resize(num);
    for (IndexType i = 0; i < IndexType(num); ++i) {
        _pointNodes[i] = {arr[i], i};
    }
    _treeNodes.resize(num);
    for (IndexType i = 0; i < IndexType(num); ++i) {
        _treeNodes[i] = {-1, -1, -1};
    }
    for (DimensionType i = 0; i < T::Dimension; ++i) {
        _dimensionMin.at(i).resize(num);
        _dimensionMax.at(i).resize(num);
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

// protected

template <KDTreeNodeAble T>
auto KDTree<T>::_buildTree(IndexType left, IndexType right, IndexType dim) -> IndexType
{
    auto lessCompare = [dim](const PointNode &aa, const PointNode &bb) -> bool {
        return static_cast<PointType>(aa.point).at(dim) < static_cast<PointType>(bb.point).at(dim);
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
        _dimensionMin.at(i)[now] = _dimensionMax.at(i)[now] = now;
    }
    IndexType lson = _treeNodes[now].lson;
    IndexType rson = _treeNodes[now].rson;
    if (lson >= 0) {
        for (DimensionType i = 0; i < T::Dimension; ++i) {
            const auto &nowMin = _pointNodes[_dimensionMin.at(i)[now]].point;
            const auto &nowMax = _pointNodes[_dimensionMax.at(i)[now]].point;
            const auto &lMin   = _pointNodes[_dimensionMin.at(i)[lson]].point;
            const auto &lMax   = _pointNodes[_dimensionMax.at(i)[lson]].point;
            if (static_cast<PointType>(nowMin).at(i) - static_cast<PointType>(lMin).at(i) > 0) {
                _dimensionMin.at(i)[now] = _dimensionMin.at(i)[lson];
            }
            if (static_cast<PointType>(nowMax).at(i) - static_cast<PointType>(lMax).at(i) < 0) {
                _dimensionMax.at(i)[now] = _dimensionMax.at(i)[lson];
            }
        }
    }
    if (rson >= 0) {
        for (DimensionType i = 0; i < T::Dimension; ++i) {
            const auto &nowMin = _pointNodes[_dimensionMin.at(i)[now]].point;
            const auto &nowMax = _pointNodes[_dimensionMax.at(i)[now]].point;
            const auto &rMin   = _pointNodes[_dimensionMin.at(i)[rson]].point;
            const auto &rMax   = _pointNodes[_dimensionMax.at(i)[rson]].point;
            if (static_cast<PointType>(nowMin).at(i) - static_cast<PointType>(rMin).at(i) > 0) {
                _dimensionMin.at(i)[now] = _dimensionMin.at(i)[rson];
            }
            if (static_cast<PointType>(nowMax).at(i) - static_cast<PointType>(rMax).at(i) < 0) {
                _dimensionMax.at(i)[now] = _dimensionMax.at(i)[rson];
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
    DistanceType nowDis =
        T::distance(static_cast<PointType>(ta), static_cast<PointType>(_pointNodes[now].point));
    if (HeapNode(nowDis, _pointNodes[now].index) < _qMax.top()) {
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
    DistanceType nowDis =
        T::distance(static_cast<PointType>(ta), static_cast<PointType>(_pointNodes[now].point));
    if (HeapNode(nowDis, _pointNodes[now].index) > _qMin.top()) {
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
        disP =
            std::max(static_cast<PointType>(ta).at(i) -
                         static_cast<PointType>(_pointNodes[_dimensionMax.at(i)[now]].point).at(i),
                     DistanceType(0));
        disQ = std::max(static_cast<PointType>(_pointNodes[_dimensionMin.at(i)[now]].point).at(i) -
                            static_cast<PointType>(ta).at(i),
                        DistanceType(0));
        temp.at(i) = disP + disQ;
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
        disP       = static_cast<PointType>(ta).at(i) -
                     static_cast<PointType>(_pointNodes[_dimensionMin.at(i)[now]].point).at(i);
        disQ       = static_cast<PointType>(ta).at(i) -
                     static_cast<PointType>(_pointNodes[_dimensionMax.at(i)[now]].point).at(i);
        temp.at(i) = std::abs(disP) > std::abs(disQ) ? disP : disQ;
    }
    return T::distance(temp, PointType{});
}

GP_ALGORITHM_END
GP_END