// graph_generator.hpp
// Generador de grafos dirigidos ponderados aleatorios, con control de densidad,
// para reproducir el tipo de instancias usadas en Lewis (2023): grafos con
// pesos no negativos, variando el número de vértices (n) y la densidad (m/n).
//
// Se garantiza que el grafo generado es conexo desde el vértice fuente (0),
// añadiendo primero un esqueleto aleatorio (cada vértice v>0 recibe una arista
// desde algún vértice u<v ya alcanzable) y luego completando con aristas
// adicionales aleatorias hasta alcanzar la densidad deseada.

#pragma once

#include "graph.hpp"
#include <random>
#include <unordered_set>
#include <cstdint>

class GraphGenerator {
public:
    GraphGenerator(uint64_t seed) : rng_(seed) {}

    // n: número de vértices
    // density: razón m/n deseada (ej. 2.0 => grafo disperso, n*log2(n) => denso, etc.)
    // minWeight, maxWeight: rango de pesos no negativos (reales)
    Graph generateRandomDirected(int n, double density,
                                  double minWeight = 1.0, double maxWeight = 1000.0) {
        Graph g(n);
        std::uniform_real_distribution<double> weightDist(minWeight, maxWeight);
        std::uniform_int_distribution<int> vertexDist(0, n - 1);

        std::unordered_set<long long> existing; // codifica (u,v) como u*n+v
        auto edgeKey = [n](int u, int v) -> long long {
            return static_cast<long long>(u) * n + v;
        };

        // 1) Esqueleto: garantiza que todo vértice sea alcanzable desde el
        //    vértice 0. Para cada v en [1, n-1], se conecta desde un u
        //    aleatorio en [0, v-1], formando un árbol aleatorio con raíz 0.
        for (int v = 1; v < n; ++v) {
            std::uniform_int_distribution<int> parentDist(0, v - 1);
            int u = parentDist(rng_);
            double w = weightDist(rng_);
            g.addEdge(u, v, w);
            existing.insert(edgeKey(u, v));
        }

        long long targetEdges = static_cast<long long>(density * n);
        long long currentEdges = n - 1; // las del esqueleto

        long long attemptsLimit = targetEdges * 20 + 1000; // evita bucles infinitos en grafos casi completos
        long long attempts = 0;

        while (currentEdges < targetEdges && attempts < attemptsLimit) {
            ++attempts;
            int u = vertexDist(rng_);
            int v = vertexDist(rng_);
            if (u == v) continue;
            long long key = edgeKey(u, v);
            if (existing.count(key)) continue;
            double w = weightDist(rng_);
            g.addEdge(u, v, w);
            existing.insert(key);
            ++currentEdges;
        }

        return g;
    }

    // Grafo "denso": density ~ n (es decir, m ~ n^2 / c para c constante pequeña)
    Graph generateDense(int n, double minWeight = 1.0, double maxWeight = 1000.0) {
        double density = 0.3 * n; // ~30% de las posibles n*(n-1) aristas dirigidas
        return generateRandomDirected(n, density, minWeight, maxWeight);
    }

    // Grafo "disperso": density ~ constante pequeña (m = O(n))
    Graph generateSparse(int n, double avgOutDegree = 4.0,
                          double minWeight = 1.0, double maxWeight = 1000.0) {
        return generateRandomDirected(n, avgOutDegree, minWeight, maxWeight);
    }

private:
    std::mt19937_64 rng_;
};
