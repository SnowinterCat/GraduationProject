#include <gp/config.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/algorithm/DirectedGraph.hpp>
#include <system_error>

auto main() -> int
{
    gp::alg::DirectedGraph<float> dg;

    auto errc = std::error_code{};
    dg.resize(6);
    errc = dg.createEdge(0, 2, 5.0F);
    errc = dg.createEdge(0, 4, 30.0F);
    errc = dg.createEdge(0, 5, 100.0F);
    errc = dg.createEdge(1, 2, 5.0F);
    errc = dg.createEdge(2, 3, 50.0F);
    errc = dg.createEdge(3, 5, 10.0F);
    errc = dg.createEdge(4, 3, 20.0F);
    errc = dg.createEdge(4, 5, 60.0F);
    // dg.dijkstra(0, 1);

    return 0;
}