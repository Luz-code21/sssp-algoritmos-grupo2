// graph.hpp
// Representación de un grafo dirigido ponderado mediante lista de adyacencia.
// Replica las condiciones descritas en Lewis (2023): vértices etiquetados
// de 0 a n-1, grafo cargado completamente en memoria (RAM) antes de ejecutar.

#pragma once

#include <vector>
#include <cstdint>

struct Edge {
    int to;
    double weight;
};

class Graph {
public:
    explicit Graph(int n) : n_(n), adj_(n) {}

    int numVertices() const { return n_; }

    long long numEdges() const { return m_; }

    void addEdge(int u, int v, double w) {
        adj_[u].push_back(Edge{v, w});
        ++m_;
    }

    const std::vector<Edge>& neighbors(int u) const { return adj_[u]; }

private:
    int n_;
    long long m_ = 0;
    std::vector<std::vector<Edge>> adj_;
};
