#include "dijkstra.h"
#include "Util.h"
#include <queue>
#include <vector>

namespace bmssp {

    // Traduccion de dijkstra.go -> func dijkstra(g *Graph, s int)
    // Dijkstra clasico con min-heap binario (priority_queue invertida).
    std::pair<std::vector<double>, std::vector<int>> dijkstra(
        const Graph& g, int s)
    {
        int n = g.numVertices();
        std::vector<double> dist(n, Inf);
        std::vector<int>    pred(n, -1);
        dist[s] = 0.0;

        // Par (distancia, vertice) - min-heap
        using PairDV = std::pair<double, int>;
        std::priority_queue<PairDV, std::vector<PairDV>, std::greater<PairDV>> pq;
        pq.push({ 0.0, s });

        while (!pq.empty()) {
            auto [du, u] = pq.top();
            pq.pop();

            // Entrada obsoleta (lazy deletion)
            if (du > dist[u]) continue;

            for (const auto& e : g.neighbors(u)) {
                double cand = du + e.w;
                if (cand < dist[e.to]) {
                    dist[e.to] = cand;
                    pred[e.to] = u;
                    pq.push({ cand, e.to });
                }
            }
        }
        return { dist, pred };
    }

} // namespace bmssp
