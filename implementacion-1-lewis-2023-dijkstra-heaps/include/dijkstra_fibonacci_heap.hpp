// dijkstra_fibonacci_heap.hpp
// Variante de Dijkstra usando un Fibonacci Heap con Decrease-Key real
// en O(1) amortizado, tal como se describe en Lewis (2023).
//
// A diferencia del binary heap, aquí se mantiene un handle (índice) por
// cada vértice insertado en el heap, lo que permite hacer decrease-key
// genuino en lugar de lazy deletion.
//
// Complejidad resultante: O(m + n log n).

#pragma once

#include "graph.hpp"
#include "fibonacci_heap.hpp"
#include <vector>
#include <limits>

inline DijkstraResult dijkstraFibonacciHeap(const Graph& g, int source) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = g.numVertices();
    std::vector<double> dist(n, INF);
    std::vector<bool> visited(n, false);
    std::vector<FibonacciHeap::Handle> handle(n, -1);

    FibonacciHeap heap;
    dist[source] = 0.0;
    handle[source] = heap.insert(0.0, source);

    while (!heap.empty()) {
        auto [u, d] = heap.extractMin();
        if (visited[u]) continue; // por seguridad, no debería ocurrir
        visited[u] = true;

        for (const Edge& e : g.neighbors(u)) {
            int v = e.to;
            if (visited[v]) continue;
            double nd = d + e.weight;
            if (nd < dist[v]) {
                dist[v] = nd;
                if (handle[v] == -1) {
                    handle[v] = heap.insert(nd, v);
                } else {
                    heap.decreaseKey(handle[v], nd);
                }
            }
        }
    }

    return {dist};
}
