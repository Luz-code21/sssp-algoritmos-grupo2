// test_correctness.cpp
//
// Verifica la corrección de las tres implementaciones de Dijkstra:
//   1. Sobre un grafo pequeño de referencia, con distancias calculadas a mano.
//   2. Sobre varios grafos aleatorios (dispersos y densos), comparando que
//      las tres variantes (binary heap, Fibonacci heap, BST) coincidan
//      exactamente entre sí.
//
// Si todos los tests pasan, imprime "TODOS LOS TESTS PASARON".

#include "graph.hpp"
#include "graph_generator.hpp"
#include "dijkstra_binary_heap.hpp"
#include "dijkstra_fibonacci_heap.hpp"
#include "dijkstra_bst.hpp"
#include "verify.hpp"

#include <iostream>
#include <vector>
#include <cmath>

bool testKnownGraph() {
    // Grafo de referencia (dirigido):
    // 0 -> 1 (4), 0 -> 2 (1)
    // 1 -> 3 (1)
    // 2 -> 1 (2), 2 -> 3 (5)
    // Distancias esperadas desde 0: d0=0, d1=3 (via 0->2->1), d2=1, d3=4 (via 0->2->1->3)
    Graph g(4);
    g.addEdge(0, 1, 4.0);
    g.addEdge(0, 2, 1.0);
    g.addEdge(1, 3, 1.0);
    g.addEdge(2, 1, 2.0);
    g.addEdge(2, 3, 5.0);

    std::vector<double> expected = {0.0, 3.0, 1.0, 4.0};

    auto rb = dijkstraBinaryHeap(g, 0);
    auto rf = dijkstraFibonacciHeap(g, 0);
    auto rs = dijkstraBST(g, 0);

    bool ok = true;
    if (!resultsMatch(rb.dist, expected)) {
        std::cerr << "FALLO: binary_heap no coincide con valores esperados en grafo de referencia\n";
        ok = false;
    }
    if (!resultsMatch(rf.dist, expected)) {
        std::cerr << "FALLO: fibonacci_heap no coincide con valores esperados en grafo de referencia\n";
        ok = false;
    }
    if (!resultsMatch(rs.dist, expected)) {
        std::cerr << "FALLO: bst no coincide con valores esperados en grafo de referencia\n";
        ok = false;
    }
    return ok;
}

bool testRandomGraphsAgree(int trials) {
    bool allOk = true;
    for (int t = 0; t < trials; ++t) {
        uint64_t seed = 1000 + t;
        GraphGenerator gen(seed);

        int n = 50 + (t % 5) * 30; // varía un poco el tamaño
        bool sparse = (t % 2 == 0);
        Graph g = sparse ? gen.generateSparse(n, 4.0) : gen.generateDense(n);

        auto rb = dijkstraBinaryHeap(g, 0);
        auto rf = dijkstraFibonacciHeap(g, 0);
        auto rs = dijkstraBST(g, 0);

        bool okFib = resultsMatch(rb.dist, rf.dist);
        bool okBST = resultsMatch(rb.dist, rs.dist);

        if (!okFib) {
            std::cerr << "FALLO: discrepancia binary vs fibonacci en trial " << t
                      << " (n=" << n << ", sparse=" << sparse << ")\n";
            allOk = false;
        }
        if (!okBST) {
            std::cerr << "FALLO: discrepancia binary vs bst en trial " << t
                      << " (n=" << n << ", sparse=" << sparse << ")\n";
            allOk = false;
        }
    }
    return allOk;
}

int main() {
    bool ok1 = testKnownGraph();
    std::cout << (ok1 ? "[OK] " : "[FALLO] ") << "Grafo de referencia con distancias conocidas\n";

    bool ok2 = testRandomGraphsAgree(40);
    std::cout << (ok2 ? "[OK] " : "[FALLO] ") << "40 grafos aleatorios: las 3 variantes coinciden\n";

    // Stress test: grafos más grandes y densos para forzar muchas operaciones
    // de consolidate/cascading-cut en el Fibonacci heap.
    bool ok3 = true;
    {
        GraphGenerator gen(777);
        Graph gBig = gen.generateDense(800);
        auto rb = dijkstraBinaryHeap(gBig, 0);
        auto rf = dijkstraFibonacciHeap(gBig, 0);
        auto rs = dijkstraBST(gBig, 0);
        ok3 = resultsMatch(rb.dist, rf.dist) && resultsMatch(rb.dist, rs.dist);
        if (!ok3) std::cerr << "FALLO: stress test con grafo denso n=800\n";
    }
    std::cout << (ok3 ? "[OK] " : "[FALLO] ") << "Stress test: grafo denso n=800\n";

    if (ok1 && ok2 && ok3) {
        std::cout << "\nTODOS LOS TESTS PASARON\n";
        return 0;
    } else {
        std::cout << "\nHAY TESTS FALLIDOS, REVISAR IMPLEMENTACION\n";
        return 1;
    }
}
