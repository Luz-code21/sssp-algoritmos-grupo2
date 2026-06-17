#include "LevelQueue.h"
#include <algorithm>

namespace bmssp {

    // Constructor: inicializa con limite superior dado
    LevelQueue::LevelQueue(double bound)
        : minKey(Inf), upperBound(bound) {
    }

    // Insert: encola un vertice con su clave (distancia)
    void LevelQueue::insert(int v, double key) {
        pq_.push(Item{ v, key });
        if (key < minKey) {
            minKey = key;
        }
    }

    // pull: extrae la sub-frontera con la clave minima.
    // Agrupa todos los items con la misma clave minima (epsilon-igual).
    // out_bi se establece con el limite actual (upperBound).
    // Equivalente a func (d *LevelQueue) Pull() (Si []int, Bi float64) en Go.
    std::vector<int> LevelQueue::pull(double& out_bi) {
        out_bi = upperBound;

        if (pq_.empty()) {
            minKey = Inf;
            return {};
        }

        const double eps = 1e-12;

        // Si el minimo supera el limite, no extraer nada
        double smallest = pq_.top().key;
        if (smallest > upperBound + eps) {
            return {};
        }

        // Agrupar todos los items con la misma clave minima
        double baseKey = smallest;
        std::vector<int> Si;
        while (!pq_.empty()) {
            double topKey = pq_.top().key;
            if (topKey > baseKey + eps) break;
            Si.push_back(pq_.top().v);
            pq_.pop();
        }

        // Actualizar minKey
        if (pq_.empty())
            minKey = Inf;
        else
            minKey = pq_.top().key;

        return Si;
    }

    // batchPrepend: insercion eficiente de multiples candidatos.
    // Equivalente a func (d *LevelQueue) BatchPrepend(candidates []kv) en Go.
    void LevelQueue::batchPrepend(const std::vector<KV>& candidates) {
        if (candidates.empty()) return;

        double minCandKey = Inf;
        for (const auto& cand : candidates) {
            pq_.push(Item{ cand.v, cand.key });
            if (cand.key < minCandKey) {
                minCandKey = cand.key;
            }
        }
        if (minCandKey < minKey) {
            minKey = minCandKey;
        }
    }

    // nonEmpty: true si la cola tiene elementos
    bool LevelQueue::nonEmpty() const {
        return !pq_.empty();
    }

} // namespace bmssp
