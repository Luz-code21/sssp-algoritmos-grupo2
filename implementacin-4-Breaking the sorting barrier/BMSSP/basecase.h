#pragma once
#include "graph.h"
#include "Set.h"
#include "Params.h"
#include "Util.h"
#include <vector>
#include <utility>

namespace bmssp {

    // Dijkstra acotado desde un singleton S={x}.
    // Devuelve (B', U) donde U son los vertices procesados y B' el nuevo limite.
    std::pair<double, Set> baseCase(
        const Graph& g,
        double B,
        const Set& S,
        std::vector<double>& db,
        std::vector<int>& pred,
        const Options& opt
    );

} // namespace bmssp
