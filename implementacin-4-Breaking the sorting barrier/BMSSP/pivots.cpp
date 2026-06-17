#include "Pivots.h"
#include "LevelQueue.h"
#include <queue>
#include <vector>
#include <algorithm>

namespace bmssp {

    // Traduccion de pivots.go -> func findPivots(...)
    // Implementa la reduccion de frontera del paper:
    // - K rondas de relajaciones especulativas para construir W
    // - Calculo de tamanos de subarboles para elegir pivotes P
    void findPivots(
        const Graph& g,
        double B,
        const Set& S,
        const std::vector<double>& db,
        const std::vector<int>& pred,
        const Options& opt,
        Set& out_P,
        Set& out_W)
    {
        // Inicializar W con todos los vertices de S
        out_W.clear();
        for (int v : S) {
            out_W.add(v);
        }

        // Etiquetas temporales para no contaminar db durante las relajaciones
        std::vector<double> tmp = db;

        // K rondas de relajaciones acotadas
        Set current = S;
        for (int i = 0; i < opt.K; ++i) {
            Set next;
            for (int u : current) {
                double du = tmp[u];
                if (du >= B) continue;
                for (const auto& e : g.neighbors(u)) {
                    int    nv   = e.to;
                    double cand = du + e.w;
                    if (cand < tmp[nv] && cand < B) {
                        tmp[nv] = cand;
                        next.add(nv);
                        out_W.add(nv);
                    }
                }
            }
            // Salida temprana: el arbol crece demasiado -> todos S son pivotes
            int sSize = S.size();
            if (sSize < 1) sSize = 1;
            if (out_W.size() > opt.K * sSize) {
                out_P = S;
                return;
            }
            current = next;
            if (current.size() == 0) break;
        }

        // Construir estructura de subarbol de caminos mas cortos sobre W
        int totalVertices = g.numVertices();
        std::vector<std::vector<int>> children(totalVertices);
        std::vector<int> subtreeSize(totalVertices, 1);

        for (int v : out_W) {
            double dv = db[v];
            for (const auto& e : g.neighbors(v)) {
                int u = e.to;
                if (!out_W.has(u)) continue;
                // Arista de arbol de caminos mas cortos
                if (nearlyEqual(db[u], dv + e.w)) {
                    children[v].push_back(u);
                }
            }
        }

        // Ordenar por distancia para procesamiento bottom-up
        // (mayor distancia primero -> procesar hijos antes que padres)
        using PairDV = std::pair<double, int>;
        std::priority_queue<PairDV, std::vector<PairDV>, std::greater<PairDV>> pq;
        for (int v : out_W) {
            pq.push({ db[v], v });
        }
        std::vector<int> processed;
        processed.reserve(out_W.size());
        while (!pq.empty()) {
            processed.push_back(pq.top().second);
            pq.pop();
        }

        // Calcular tamanos de subarboles: bottom-up (reverso del orden por distancia)
        for (int i = static_cast<int>(processed.size()) - 1; i >= 0; --i) {
            int v = processed[i];
            for (int child : children[v]) {
                subtreeSize[v] += subtreeSize[child];
            }
        }

        // Extraer pivotes: vertices de S con subarbol >= K
        out_P.clear();
        for (int v : S) {
            if (subtreeSize[v] >= opt.K) {
                out_P.add(v);
            }
        }

        // Fallback: si no hay pivotes, usar toda la frontera S
        if (out_P.size() == 0) {
            out_P = S;
        }
    }

} // namespace bmssp
