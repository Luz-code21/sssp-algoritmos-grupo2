#include "bmssp.h"
#include "dijkstra.h"
#include "graph.h"
#include "Util.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

// Imprime el vector de distancias
static void printDistances(const std::vector<double>& dist) {
    std::cout << "[";
    for (int i = 0; i < static_cast<int>(dist.size()); ++i) {
        if (i > 0) std::cout << " ";
        if (dist[i] == bmssp::Inf)
            std::cout << "Inf";
        else
            std::cout << std::fixed << std::setprecision(1) << dist[i];
    }
    std::cout << "]\n";
}

// Imprime el vector de predecesores
static void printPredecessors(const std::vector<int>& pred) {
    std::cout << "[";
    for (int i = 0; i < static_cast<int>(pred.size()); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << pred[i];
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "=== BMSSP - Breaking the Sorting Barrier SSSP ===\n\n";

    // -------------------------------------------------------------------
    // Ejemplo basico (equivalente a examples/basic/main.go)
    // Grafo con 4 vertices:
    //   0->1 (2)  0->2 (4)  1->2 (1)  2->3 (3)
    // -------------------------------------------------------------------
    {
        bmssp::Graph g(4);
        g.addEdge(0, 1, 2.0);
        g.addEdge(0, 2, 4.0);
        g.addEdge(1, 2, 1.0);
        g.addEdge(2, 3, 3.0);

        std::cout << "--- Ejemplo basico (4 vertices) ---\n";
        std::cout << "Aristas: 0->1(2), 0->2(4), 1->2(1), 2->3(3)\n";
        std::cout << "Fuente: 0\n\n";

        auto [distBmssp, predBmssp] = bmssp::SSSP(g, 0);
        std::cout << "BMSSP Distancias:   "; printDistances(distBmssp);
        std::cout << "BMSSP Predecesores: "; printPredecessors(predBmssp);

        auto [distDijk, predDijk] = bmssp::dijkstra(g, 0);
        std::cout << "\nDijkstra Distancias:   "; printDistances(distDijk);
        std::cout << "Dijkstra Predecesores: "; printPredecessors(predDijk);
        std::cout << "\n";
    }

    // -------------------------------------------------------------------
    // Ejemplo mas grande (equivalente a examples/compare_with_dijkstra/main.go)
    // -------------------------------------------------------------------
    {
        bmssp::Graph g(6);
        g.addEdge(0, 1, 3.0);
        g.addEdge(0, 2, 8.0);
        g.addEdge(1, 2, 2.0);
        g.addEdge(1, 3, 5.0);
        g.addEdge(2, 4, 4.0);
        g.addEdge(3, 4, 1.0);
        g.addEdge(3, 5, 6.0);
        g.addEdge(4, 5, 2.0);

        std::cout << "--- Ejemplo 6 vertices ---\n";
        std::cout << "Fuente: 0\n\n";

        auto [distBmssp, predBmssp] = bmssp::SSSP(g, 0);
        std::cout << "BMSSP Distancias:   "; printDistances(distBmssp);

        auto [distDijk, predDijk] = bmssp::dijkstra(g, 0);
        std::cout << "Dijkstra Distancias:"; printDistances(distDijk);

        // Verificar que los resultados coinciden
        bool ok = true;
        for (int i = 0; i < 6; ++i) {
            double diff = distBmssp[i] - distDijk[i];
            if (diff < 0) diff = -diff;
            if (diff > 1e-9) { ok = false; break; }
        }
        std::cout << "\nResultados identicos: " << (ok ? "SI" : "NO") << "\n\n";
    }

    std::cout << "Presione ENTER para salir...";
    std::cin.get();
    return 0;
}
