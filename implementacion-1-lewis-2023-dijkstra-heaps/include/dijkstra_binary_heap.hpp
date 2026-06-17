// dijkstra_binary_heap.hpp
// Variante de Dijkstra usando un binary heap (std::priority_queue).
//
// Tal como señala Lewis (2023), std::priority_queue en C++ NO soporta
// la operación Decrease-Key de forma nativa. La solución estándar (y la
// que se asume en el paper) es la técnica de "lazy deletion": en lugar de
// decrementar la clave de un elemento existente, se inserta un nuevo par
// (distancia_actualizada, vértice) en el heap, dejando la entrada antigua
// obsoleta. Al extraer el mínimo, se descartan las entradas obsoletas
// comparando contra dist[] ya confirmado.
//
// Complejidad resultante: O(m log m) en el peor caso (puede haber hasta
// m inserciones), igual a la complejidad reportada por Lewis para esta
// variante.

#pragma once

#include "graph.hpp"
#include <vector>
#include <queue>
#include <limits>

struct DijkstraResult {
    std::vector<double> dist;
};

inline DijkstraResult dijkstraBinaryHeap(const Graph& g, int source) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = g.numVertices();
    std::vector<double> dist(n, INF);
    std::vector<bool> visited(n, false);
    dist[source] = 0.0;

    using PQItem = std::pair<double, int>; // (distancia, vértice)
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited[u]) continue; // entrada obsoleta (lazy deletion)
        visited[u] = true;

        for (const Edge& e : g.neighbors(u)) {
            double nd = d + e.weight;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                pq.push({nd, e.to});
            }
        }
    }

    return {dist};
}
