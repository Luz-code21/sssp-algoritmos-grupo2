// dijkstra_bst.hpp
// Variante de Dijkstra usando un árbol binario autobalanceado como cola
// de prioridad, tal como se describe en Lewis (2023). En C++ esta
// estructura es provista por std::set (típicamente implementado como
// un red-black tree).
//
// A diferencia de std::priority_queue, std::set permite eliminar un
// elemento específico en O(log n), lo que habilita un Decrease-Key real:
// se borra el par (distancia_vieja, vértice) y se inserta el nuevo
// (distancia_nueva, vértice).
//
// Complejidad resultante: O(m log n).

#pragma once

#include "graph.hpp"
#include <set>
#include <vector>
#include <limits>

inline DijkstraResult dijkstraBST(const Graph& g, int source) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = g.numVertices();
    std::vector<double> dist(n, INF);

    using PQItem = std::pair<double, int>; // (distancia, vértice)
    std::set<PQItem> pq;

    dist[source] = 0.0;
    pq.insert({0.0, source});

    while (!pq.empty()) {
        auto it = pq.begin();
        double d = it->first;
        int u = it->second;
        pq.erase(it);

        for (const Edge& e : g.neighbors(u)) {
            int v = e.to;
            double nd = d + e.weight;
            if (nd < dist[v]) {
                if (dist[v] != INF) {
                    pq.erase({dist[v], v}); // decrease-key real: borrar entrada vieja
                }
                dist[v] = nd;
                pq.insert({nd, v});
            }
        }
    }

    return {dist};
}
