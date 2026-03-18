#pragma once
#include <gp/__algorithm/__config.hpp>
// Standard Library
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
concept Arithmetic = std::is_arithmetic_v<T>;

template <Arithmetic DistanceType>
class DirectedGraph {
public:
    static constexpr auto IndexMax    = std::numeric_limits<int32_t>::max();
    static constexpr auto IndexMin    = std::numeric_limits<int32_t>::max();
    static constexpr auto DistanceMax = std::numeric_limits<DistanceType>::max();
    static constexpr auto DistanceMin = std::numeric_limits<DistanceType>::max();

    struct Edge {
        int32_t      source;      // 源点
        int32_t      target;      // 目标点
        DistanceType cost;        // 路径花费
        DistanceType flow;        // 路径流量
        int32_t      outEdgeNext; // 下一条出边
        int32_t      inEdgeNext;  // 下一条入边
        int32_t      outEdgePre;  // 上一条出边
        int32_t      inEdgePre;   // 上一条入边
    };
    struct Vertex {
        int32_t      outEdge;   // 顶点的第一条出边
        int32_t      inEdge;    // 顶点的第一条入边
        int32_t      preVertex; // Dijkstra 中用于保存上一个顶点
        bool         visit;     // Dijkstra 中用于标记是否访问过
        DistanceType distance;  // Dijkstra 中用于保存距离
    };
    struct ErrorDistance {
        std::error_code errc;
        DistanceType    distance;
    };
    struct ErrorDistanceDistance {
        std::error_code errc;
        DistanceType    cost;
        DistanceType    flow;
    };
    struct HeapNode {
        DistanceType cost;
        int32_t      vertex;
        auto         operator<=>(const HeapNode &other) const = default;
    };

    void resize(int32_t vertexCnt);
    void reserve(int32_t vertexCnt);
    void clear();

    auto createEdge(int32_t source, int32_t target, DistanceType cost, DistanceType flow)
        -> std::error_code;
    auto removeEdge(int32_t source, int32_t target) -> std::error_code;

    auto removeOutEdges(int32_t source) -> std::error_code;
    auto removeInEdges(int32_t target) -> std::error_code;

    auto dijkstra(int32_t source, int32_t target) -> ErrorDistance;
    auto dinicOnDijkstra(int32_t source, int32_t target) -> ErrorDistanceDistance;

protected:
    std::vector<Vertex> _vertices;
    std::vector<Edge>   _edges;

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> _queue;
};

template <Arithmetic DistanceType>
void DirectedGraph<DistanceType>::resize(int32_t vertexCnt)
{
    _vertices.resize(vertexCnt);
}

template <Arithmetic DistanceType>
void DirectedGraph<DistanceType>::reserve(int32_t vertexCnt)
{
    _vertices.reserve(vertexCnt);
}

template <Arithmetic DistanceType>
void DirectedGraph<DistanceType>::clear()
{
    _vertices.clear();
    _edges.clear();
}

template <Arithmetic DistanceType>
auto DirectedGraph<DistanceType>::dijkstra(int32_t source, int32_t target) -> ErrorDistance
{
    if (source >= static_cast<int32_t>(_vertices.size()) ||
        target >= static_cast<int32_t>(_vertices.size())) {
        return {std::make_error_code(std::errc::invalid_argument), static_cast<DistanceType>(0)};
    }

    for (auto &vertex : _vertices) {
        vertex.visit    = false;
        vertex.distance = std::numeric_limits<DistanceType>::max();
    }
    _vertices[source].visit    = true;
    _vertices[source].distance = static_cast<DistanceType>(0);

    _queue.push(HeapNode{0, source});
    while (!_queue.empty()) {
        auto temp = _queue.top();
        _queue.pop();
        if (temp.vertex >= static_cast<int32_t>(_vertices.size()) && _vertices[temp.vertex].visit) {
            continue;
        }
        _vertices[temp.vertex].visit = true;
        for (int32_t i = _vertices[temp.vertex].outEdge; i != IndexMax; i = _edges[i].outEdgeNext) {
            int32_t to = _edges[i].target;
            if (to < static_cast<int32_t>(_vertices.size()) &&
                _vertices[to].distance > _edges[i].cost + temp.cost) {
                _vertices[to].distance = _edges[i].cost + temp.cost;
                _queue.push({_vertices[to].distance, to});
            }
        }
    }

    return {std::error_code(), _vertices[target].distance};
}

GP_ALGORITHM_END
GP_END