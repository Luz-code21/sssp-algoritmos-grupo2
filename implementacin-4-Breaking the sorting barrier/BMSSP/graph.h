#pragma once

#include <vector>
#include <stdexcept>

namespace bmssp {

    struct Edge {
        int to;
        double w;

        Edge(int t, double weight)
            : to(t), w(weight) {
        }
    };

    class Graph {
    private:
        int n_;
        std::vector<std::vector<Edge>> adj_;

    public:
        explicit Graph(int n);

        void addEdge(int u, int v, double w);

        int numVertices() const;

        const std::vector<Edge>& neighbors(int u) const;
    };

} // namespace bmssp