// verify.hpp
// Verifica que dos resultados de Dijkstra (vectores de distancias) coinciden,
// dentro de una tolerancia numérica razonable para comparaciones de double.

#pragma once

#include <vector>
#include <cmath>
#include <limits>

inline bool resultsMatch(const std::vector<double>& a, const std::vector<double>& b,
                          double eps = 1e-6) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        bool infA = std::isinf(a[i]);
        bool infB = std::isinf(b[i]);
        if (infA != infB) return false;
        if (infA && infB) continue;
        if (std::fabs(a[i] - b[i]) > eps) return false;
    }
    return true;
}
