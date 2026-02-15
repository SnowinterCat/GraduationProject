#include <gp/config.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/algorithm/DirectedGraph.hpp>

auto main() -> int
{
    gp::alg::DirectedGraph<float> dg;
    dg.dijkstra(0, 1);

    return 0;
}