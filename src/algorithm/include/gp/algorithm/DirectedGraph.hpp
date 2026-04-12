#pragma once
#include <cstddef>
#include <gp/__algorithm/__config.hpp>
// Standard Library
#include <new>
#include <type_traits>
// #include <concepts>
#include <system_error>
#include <vector>
#include <queue>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_ALGORITHM_BEGIN

template <typename T>
concept IsVoid = std::is_void_v<T>;

template <typename T>
concept NotVoid = !std::is_void_v<T>;

template <typename T, typename U>
concept AdditionAble = requires(T aa, U bb) { aa + bb; };

template <NotVoid CostType, typename DefaultFlowType = void>
class DirectedGraph {
public:
    using FlowType =
        std::conditional_t<std::is_same_v<DefaultFlowType, void>, std::monostate, DefaultFlowType>;

    static constexpr auto CostMax = std::numeric_limits<CostType>::max();
    static constexpr auto CostMin = std::numeric_limits<CostType>::max();

    struct Edge {
        int32_t  source;      // 源点
        int32_t  target;      // 目标点
        Edge    *outEdgeNext; // 下一条出边
        Edge    *outEdgePre;  // 上一条出边
        Edge    *inEdgeNext;  // 下一条入边
        Edge    *inEdgePre;   // 上一条入边
        CostType cost;        // 路径花费
        [[no_unique_address]] //
        FlowType flow;        // 路径流量
        auto     operator<=>(const Edge &other) const
        {
            if (auto cmp = source <=> other.source; cmp != 0) {
                return cmp;
            }
            if (auto cmp = target <=> other.target; cmp != 0) {
                return cmp;
            }
            if (auto cmp = cost <=> other.cost; cmp != 0) {
                return cmp;
            }
            return flow <=> other.flow;
        }
    };
    struct Vertex {
        Edge *outEdge; // 顶点的第一条出边
        Edge *inEdge;  // 顶点的第一条入边
    };
    using HeapNode = std::tuple<CostType, int32_t>; // Dijkstra 中的堆节点(cost, vertex)

    using DijkstraStruct =
        std::tuple<std::vector<bool>, std::vector<CostType>, std::priority_queue<HeapNode>>;

    ~DirectedGraph();
    DirectedGraph(const DirectedGraph &) noexcept = default;
    DirectedGraph(DirectedGraph &&) noexcept      = default;

    auto operator=(const DirectedGraph &rhs) noexcept -> DirectedGraph & = default;
    auto operator=(DirectedGraph &&rhs) noexcept -> DirectedGraph &      = default;

    auto resize(size_t size) -> void;

    auto createEdge(int32_t source, int32_t target, CostType cost,
                    FlowType flow = FlowType{}) noexcept -> std::error_code;
    auto removeEdge(int32_t source, int32_t target, CostType cost,
                    FlowType flow = FlowType{}) noexcept -> std::error_code;
    auto removeOutEdges(int32_t source) noexcept -> std::error_code;
    auto removeInEdges(int32_t target) noexcept -> std::error_code;

    auto dijkstra(int32_t source, int32_t target) -> std::tuple<std::error_code, CostType>;

protected:
    std::vector<Vertex> _vertices;
};

template <NotVoid CostType, typename DefaultFlowType>
DirectedGraph<CostType, DefaultFlowType>::~DirectedGraph()
{
    for (const auto &vertex : _vertices) {
        for (auto *edge = vertex.outEdge; edge != nullptr; edge = edge->outEdgeNext) {
            delete edge;
        }
    }
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::resize(size_t size) -> void
{
    _vertices.resize(size);
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::createEdge(int32_t source, int32_t target,
                                                          CostType cost, FlowType flow) noexcept
    -> std::error_code
{
    auto *sout = _vertices[source].outEdge;
    auto *tin  = _vertices[target].inEdge;
    auto *edge = new (std::nothrow) Edge{.source      = source,
                                         .target      = target,
                                         .outEdgeNext = sout,
                                         .outEdgePre  = nullptr,
                                         .inEdgeNext  = tin,
                                         .inEdgePre   = nullptr,
                                         .cost        = cost,
                                         .flow        = flow};
    if (edge == nullptr) {
        return std::make_error_code(std::errc::not_enough_memory);
    }
    if (_vertices[source].outEdge != nullptr) {
        _vertices[source].outEdge->outEdgePre = edge;
    }
    if (_vertices[target].inEdge != nullptr) {
        _vertices[target].inEdge->inEdgePre = edge;
    }
    _vertices[source].outEdge = edge;
    _vertices[target].inEdge  = edge;
    return {};
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::removeEdge(int32_t source, int32_t target,
                                                          CostType cost, FlowType flow) noexcept
    -> std::error_code
{
    auto tempEdge = Edge{.source = source, .target = target, .cost = cost, .flow = flow};
    for (auto *edge = _vertices[source].outEdge; edge != nullptr; edge = edge->outEdgeNext) {
        if (*edge == tempEdge) {
            if (edge->outEdgePre != nullptr) {
                edge->outEdgePre->outEdgeNext = edge->outEdgeNext;
            }
            else {
                _vertices[source].outEdge = edge->outEdgeNext;
            }
            if (edge->inEdgePre != nullptr) {
                edge->inEdgePre->inEdgeNext = edge->inEdgeNext;
            }
            else {
                _vertices[target].inEdge = edge->inEdgeNext;
            }
            delete edge;
            return {};
        }
    }
    return std::make_error_code(std::errc::no_such_file_or_directory);
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::removeOutEdges(int32_t source) noexcept
    -> std::error_code
{
    for (auto *edge = _vertices[source].outEdge; edge != nullptr; edge = edge->outEdgeNext) {
        if (edge->inEdgePre != nullptr) {
            edge->inEdgePre->inEdgeNext = edge->inEdgeNext;
        }
        else {
            _vertices[edge->target].inEdge = edge->inEdgeNext;
        }
        if (edge->inEdgeNext != nullptr) {
            edge->inEdgeNext->inEdgePre = edge->inEdgePre;
        }
        delete edge;
    }
    _vertices[source].outEdge = nullptr;
    return {};
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::removeInEdges(int32_t target) noexcept
    -> std::error_code
{
    for (auto *edge = _vertices[target].inEdge; edge != nullptr; edge = edge->inEdgeNext) {
        if (edge->outEdgePre != nullptr) {
            edge->outEdgePre->outEdgeNext = edge->outEdgeNext;
        }
        else {
            _vertices[edge->source].outEdge = edge->outEdgeNext;
        }
        if (edge->outEdgeNext != nullptr) {
            edge->outEdgeNext->outEdgePre = edge->outEdgePre;
        }
        delete edge;
    }
    _vertices[target].inEdge = nullptr;
    return {};
}

template <NotVoid CostType, typename DefaultFlowType>
auto DirectedGraph<CostType, DefaultFlowType>::dijkstra(int32_t source, int32_t target)
    -> std::tuple<std::error_code, CostType>
{
    if (source >= static_cast<int32_t>(_vertices.size()) ||
        target >= static_cast<int32_t>(_vertices.size())) {
        return {std::make_error_code(std::errc::invalid_argument), static_cast<CostType>(0)};
    }

    // for (auto &vertex : _vertices) {
    //     vertex.visit    = false;
    //     vertex.distance = std::numeric_limits<CostType>::max();
    // }
    // _vertices[source].visit    = true;
    // _vertices[source].distance = static_cast<CostType>(0);

    // _queue.push(HeapNode{0, source});
    // while (!_queue.empty()) {
    //     auto temp = _queue.top();
    //     _queue.pop();
    //     if (temp.vertex >= static_cast<int32_t>(_vertices.size()) &&
    //     _vertices[temp.vertex].visit) {
    //         continue;
    //     }
    //     _vertices[temp.vertex].visit = true;
    //     for (int32_t i = _vertices[temp.vertex].outEdge; i != IndexMax; i =
    //     _edges[i].outEdgeNext) {
    //         int32_t to = _edges[i].target;
    //         if (to < static_cast<int32_t>(_vertices.size()) &&
    //             _vertices[to].distance > _edges[i].cost + temp.cost) {
    //             _vertices[to].distance = _edges[i].cost + temp.cost;
    //             _queue.push({_vertices[to].distance, to});
    //         }
    //     }
    // }

    // return {std::error_code(), _vertices[target].distance};
}

GP_ALGORITHM_END
GP_END