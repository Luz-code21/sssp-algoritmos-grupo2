#pragma once
#include "graph.h"
#include <vector>
#include <utility>

namespace bmssp {

    // Dijkstra clasico con min-heap binario.
    // Equivalente a dijkstra.go -> func dijkstra(g *Graph, s int)
    std::pair<std::vector<double>, std::vector<int>> dijkstra(
        const Graph& g, int s
    );

} // namespace bmssp
