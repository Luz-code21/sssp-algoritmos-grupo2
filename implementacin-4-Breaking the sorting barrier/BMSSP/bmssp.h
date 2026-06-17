#pragma once
#include "graph.h"
#include "Params.h"
#include "Set.h"
#include "Util.h"
#include <vector>
#include <utility>

namespace bmssp {

    // -------------------------------------------------------------------------
    // SSSP: punto de entrada publico del algoritmo BMSSP
    // Equivalente a func SSSP(g *Graph, s int, opts ...Option) en bmssp.go
    // Devuelve (distancias[], predecesores[])
    // -------------------------------------------------------------------------
    std::pair<std::vector<double>, std::vector<int>> SSSP(
        const Graph& g, int source, const Options* custom_opts = nullptr
    );

    // -------------------------------------------------------------------------
    // dijkstra: Dijkstra clasico para comparacion/validacion
    // Equivalente a func dijkstra(g *Graph, s int) en dijkstra.go
    // -------------------------------------------------------------------------
    std::pair<std::vector<double>, std::vector<int>> dijkstra(
        const Graph& g, int s
    );

    // -------------------------------------------------------------------------
    // bmsspCore: nucleo recursivo del algoritmo
    // Equivalente a func bmssp(...) en bmssp.go
    // Devuelve Bprime; escribe en out_U el conjunto de vertices completados
    // -------------------------------------------------------------------------
    double bmsspCore(
        const Graph& g,
        int level,
        double B,
        const Set& S,
        std::vector<double>& db,
        std::vector<int>& pred,
        const Options& opt,
        Set& out_U
    );

} // namespace bmssp
