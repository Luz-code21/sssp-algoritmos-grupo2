#pragma once
#include "Util.h"
#include <queue>
#include <vector>

namespace bmssp {

    struct Item {
        int v;
        double key;

        // Sobrecarga de operador para construir el MinPQ
        bool operator>(const Item& other) const {
            return key > other.key;
        }
    };

    struct KV {
        int v;
        double key;
    };

    class LevelQueue {
    private:
        std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq_;

    public:
        double minKey;
        double upperBound;

        explicit LevelQueue(double bound = Inf);

        void insert(int v, double key);
        std::vector<int> pull(double& out_bi);
        void batchPrepend(const std::vector<KV>& candidates);
        bool nonEmpty() const;
    };

} // namespace bmssp