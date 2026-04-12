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
    decltype(dg)::DijkstraStruct  dijStruct;

    auto errc = std::error_code{};
    dg.resize(6);                       // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(0, 2, 5.0F);   // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(0, 4, 30.0F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(0, 5, 100.0F); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(1, 2, 5.0F);   // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(2, 3, 50.0F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(3, 5, 10.0F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(4, 3, 20.0F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    errc = dg.createEdge(4, 5, 60.0F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    std::println("{}", std::get<1>(dg.dijkstra(0, 5, dijStruct)));
    return 0;
}