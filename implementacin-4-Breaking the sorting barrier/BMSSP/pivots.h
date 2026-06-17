#pragma once
#include "graph.h"
#include "Set.h"
#include "Params.h"
#include "Util.h"
#include <vector>
#include <cmath>

namespace bmssp {

    inline bool nearlyEqual(double a, double b) {
        const double epsilon = 1e-12;
        return std::abs(a - b) <= epsilon;
    }

    void findPivots(const Graph& g, double B, const Set& S, const std::vector<double>& db,
        const std::vector<int>& pred, const Options& opt, Set& out_P, Set& out_W);

} // namespace bmssp