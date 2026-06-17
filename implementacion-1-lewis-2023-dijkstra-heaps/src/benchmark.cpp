// benchmark.cpp
//
// Replica el diseño experimental de Lewis (2023): "A Comparison of
// Dijkstra's Algorithm Using Fibonacci Heaps, Binary Heaps, and
// Self-Balancing Binary Trees".
//
// Para cada combinación de (n, densidad, semilla) se genera un grafo
// dirigido ponderado y se ejecutan las tres variantes de Dijkstra:
//   1. Binary heap      (dijkstraBinaryHeap)
//   2. Fibonacci heap   (dijkstraFibonacciHeap)
//   3. Árbol balanceado (dijkstraBST)
//
// Se verifica que las tres produzcan las mismas distancias (corrección),
// se mide el tiempo de ejecución de cada una con std::chrono, y se
// exportan los resultados a un archivo CSV para su posterior análisis
// (gráficas, tablas) en el capítulo de experimentación.
//
// Uso:
//   ./benchmark > results/benchmark_results.csv
//
// Columnas del CSV:
//   tipo_grafo, n, m, densidad_objetivo, semilla, algoritmo, tiempo_ms, distancia_total

#include "graph.hpp"
#include "graph_generator.hpp"
#include "dijkstra_binary_heap.hpp"
#include "dijkstra_fibonacci_heap.hpp"
#include "dijkstra_bst.hpp"
#include "verify.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>

using Clock = std::chrono::high_resolution_clock;

// Ejecuta fn() y devuelve el tiempo transcurrido en milisegundos (double).
template <typename Func>
double timeIt(Func fn) {
    auto start = Clock::now();
    fn();
    auto end = Clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double sumFiniteDistances(const std::vector<double>& dist) {
    double total = 0.0;
    for (double d : dist) {
        if (!std::isinf(d)) total += d;
    }
    return total;
}

struct ExperimentConfig {
    std::string graphType; // "sparse" o "dense"
    int n;
    double density;        // razón m/n objetivo (para sparse) o factor (para dense)
};

int main(int argc, char** argv) {
    // Tamaños de n a evaluar. Pueden ajustarse según el tiempo disponible
    // para correr el benchmark; valores grandes en BST/binary heap pueden
    // tardar más en grafos densos.
    std::vector<int> sparseSizes   = {100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    std::vector<int> denseSizesArg = {100, 200, 500, 1000, 2000};

    // Número de repeticiones por configuración (para promediar y reducir ruido).
    int repetitions = 5;

    // Semilla base; cada repetición usa una semilla distinta pero reproducible.
    uint64_t baseSeed = 42;

    std::vector<ExperimentConfig> configs;

    // Grafos dispersos: grado de salida promedio fijo (m = O(n)).
    for (int n : sparseSizes) {
        configs.push_back({"sparse", n, 4.0}); // avg out-degree = 4
    }

    // Grafos densos: m ~ 0.3 * n^2 (siguiendo la idea de "denso" usada en
    // el generador), expresado aquí como factor multiplicado por n dentro
    // de generateDense().
    for (int n : denseSizesArg) {
        configs.push_back({"dense", n, 0.3}); // 30% de aristas posibles
    }

    // Encabezado CSV
    std::cout << "tipo_grafo,n,m,densidad_objetivo,semilla,algoritmo,tiempo_ms,distancia_total,correcto\n";

    for (const auto& cfg : configs) {
        for (int rep = 0; rep < repetitions; ++rep) {
            uint64_t seed = baseSeed + static_cast<uint64_t>(rep) * 1000003ULL
                              + static_cast<uint64_t>(cfg.n);

            GraphGenerator gen(seed);
            Graph g = (cfg.graphType == "sparse")
                          ? gen.generateSparse(cfg.n, cfg.density)
                          : gen.generateDense(cfg.n);

            int source = 0;

            DijkstraResult resBinary, resFib, resBST;
            double tBinary = timeIt([&]() { resBinary = dijkstraBinaryHeap(g, source); });
            double tFib    = timeIt([&]() { resFib    = dijkstraFibonacciHeap(g, source); });
            double tBST    = timeIt([&]() { resBST    = dijkstraBST(g, source); });

            bool okFib = resultsMatch(resBinary.dist, resFib.dist);
            bool okBST = resultsMatch(resBinary.dist, resBST.dist);

            if (!okFib) {
                std::cerr << "[ADVERTENCIA] Discrepancia binary vs fibonacci en n="
                          << cfg.n << " tipo=" << cfg.graphType << " semilla=" << seed << "\n";
            }
            if (!okBST) {
                std::cerr << "[ADVERTENCIA] Discrepancia binary vs bst en n="
                          << cfg.n << " tipo=" << cfg.graphType << " semilla=" << seed << "\n";
            }

            double sumBinary = sumFiniteDistances(resBinary.dist);
            double sumFib    = sumFiniteDistances(resFib.dist);
            double sumBST    = sumFiniteDistances(resBST.dist);

            long long m = g.numEdges();

            std::cout << cfg.graphType << "," << cfg.n << "," << m << ","
                      << cfg.density << "," << seed << ","
                      << "binary_heap" << "," << tBinary << "," << sumBinary << ","
                      << (true ? 1 : 0) << "\n";

            std::cout << cfg.graphType << "," << cfg.n << "," << m << ","
                      << cfg.density << "," << seed << ","
                      << "fibonacci_heap" << "," << tFib << "," << sumFib << ","
                      << (okFib ? 1 : 0) << "\n";

            std::cout << cfg.graphType << "," << cfg.n << "," << m << ","
                      << cfg.density << "," << seed << ","
                      << "bst" << "," << tBST << "," << sumBST << ","
                      << (okBST ? 1 : 0) << "\n";
        }
    }

    return 0;
}
