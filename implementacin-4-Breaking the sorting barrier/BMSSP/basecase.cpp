#include "basecase.h"
#include "graph.h"
#include "Set.h"
#include "Params.h"
#include "Util.h"
#include <queue>
#include <vector>
#include <algorithm>

namespace bmssp {

    // Traduccion de basecase.go -> func baseCase(...)
    // Dijkstra acotado desde un singleton S={x}.
    // Procesa hasta K+1 vertices con distancia < B.
    // Devuelve (B', U):
    //   - si proceso <= K vertices: devuelve B sin cambios, U = todos procesados
    //   - si proceso K+1 vertices: devuelve la (K+1)-esima distancia como B',
    //     y U = los K vertices con menor distancia.
    std::pair<double, Set> baseCase(
        const Graph& g,
        double B,
        const Set& S,
        std::vector<double>& db,
        std::vector<int>& pred,
        const Options& opt)
    {
        // baseCase requiere singleton S (igual que el Go: panic si Size != 1)
        int x = -1;
        for (int v : S) {
            x = v;
            break;
        }

        int n = g.numVertices();
        std::vector<bool> processed(n, false);
        std::vector<int>  U0;

        // Min-heap: (distancia, vertice)
        using PairDV = std::pair<double, int>;
        std::priority_queue<PairDV, std::vector<PairDV>, std::greater<PairDV>> pq;
        pq.push({ db[x], x });

        // Procesar hasta K+1 vertices o hasta agotar la cola
        while (!pq.empty() && static_cast<int>(U0.size()) < opt.K + 1) {
            auto [du, u] = pq.top();
            pq.pop();

            if (processed[u] || du >= B) continue;

            processed[u] = true;
            U0.push_back(u);

            // Relajar aristas salientes
            for (const auto& e : g.neighbors(u)) {
                double cand = du + e.w;
                if (cand < db[e.to] && cand < B) {
                    db[e.to]   = cand;
                    pred[e.to] = u;
                    if (!processed[e.to]) {
                        pq.push({ cand, e.to });
                    }
                }
            }
        }

        // Si procesamos <= K vertices: devolver todos con B sin cambios
        if (static_cast<int>(U0.size()) <= opt.K) {
            Set U;
            for (int v : U0) U.add(v);
            return { B, U };
        }

        // Si procesamos K+1 vertices: regla "K+1 then step back" del paper
        // Ordenar por distancia y tomar los K menores;
        // la (K+1)-esima distancia es el nuevo limite B'.
        std::sort(U0.begin(), U0.end(), [&](int a, int b) {
            return db[a] < db[b];
        });

        double Bprime = db[U0[opt.K]]; // distancia del (K+1)-esimo vertice

        Set U;
        for (int i = 0; i < opt.K; ++i) {
            U.add(U0[i]);
        }
        return { Bprime, U };
    }

} // namespace bmssp
