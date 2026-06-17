#include "graph.h"

namespace bmssp {

    Graph::Graph(int n)
        : n_(n), adj_(n) {
    }

    void Graph::addEdge(int u, int v, double w) {
        if (u < 0 || u >= n_ || v < 0 || v >= n_) {
            throw std::out_of_range("vertex out of range");
        }

        if (w < 0.0) {
            throw std::invalid_argument(
                "negative weights are not supported");
        }

        adj_[u].emplace_back(v, w);
    }

    int Graph::numVertices() const {
        return n_;
    }

    const std::vector<Edge>& Graph::neighbors(int u) const {
        return adj_[u];
    }

} // namespace bmssp