// fibonacci_heap.hpp
// Implementación de un Fibonacci Heap con soporte para Decrease-Key en O(1)
// amortizado, tal como lo requiere la variante de Dijkstra estudiada en
// Lewis (2023). Cada nodo del heap está asociado a un vértice del grafo
// (handle), lo que permite hacer decrease-key directamente sobre el nodo
// correspondiente sin necesidad de buscarlo.
//
// Operaciones soportadas:
//   - insert(key, vertex) -> handle
//   - extractMin() -> (vertex, key)
//   - decreaseKey(handle, newKey)
//   - empty(), size()
//
// Complejidades amortizadas:
//   insert: O(1)
//   decreaseKey: O(1)
//   extractMin: O(log n)

#pragma once

#include <vector>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <unordered_map>

class FibonacciHeap {
public:
    using Handle = int; // índice estable dentro de nodes_

    FibonacciHeap() = default;

    bool empty() const { return size_ == 0; }
    int size() const { return size_; }

    Handle insert(double key, int vertex) {
        int idx = allocateNode(key, vertex);
        rootInsert(idx);
        if (minIdx_ == -1 || nodes_[idx].key < nodes_[minIdx_].key) {
            minIdx_ = idx;
        }
        ++size_;
        return idx;
    }

    // Devuelve (vertex, key) del mínimo y lo elimina del heap.
    std::pair<int, double> extractMin() {
        if (minIdx_ == -1) throw std::runtime_error("extractMin on empty heap");
        int z = minIdx_;

        // Mover todos los hijos de z a la lista de raíces.
        if (nodes_[z].child != -1) {
            int c = nodes_[z].child;
            int start = c;
            do {
                int next = nodes_[c].right;
                nodes_[c].parent = -1;
                rootInsert(c);
                c = next;
            } while (c != start);
        }

        rootRemove(z);

        if (z == nodes_[z].right) {
            // era el único nodo
            minIdx_ = -1;
        } else {
            minIdx_ = nodes_[z].right;
            consolidate();
        }

        --size_;
        int vertex = nodes_[z].vertex;
        double key = nodes_[z].key;
        freeNode(z);
        return {vertex, key};
    }

    void decreaseKey(Handle x, double newKey) {
        if (newKey > nodes_[x].key) {
            throw std::invalid_argument("decreaseKey: new key is greater than current key");
        }
        nodes_[x].key = newKey;
        int p = nodes_[x].parent;
        if (p != -1 && nodes_[x].key < nodes_[p].key) {
            cut(x, p);
            cascadingCut(p);
        }
        if (nodes_[x].key < nodes_[minIdx_].key) {
            minIdx_ = x;
        }
    }

private:
    struct Node {
        double key;
        int vertex;
        int degree = 0;
        bool mark = false;
        int parent = -1;
        int child = -1;
        int left = -1;
        int right = -1;
        bool active = false;
    };

    std::vector<Node> nodes_;
    std::vector<int> freeList_;
    int minIdx_ = -1;
    int size_ = 0;

    int allocateNode(double key, int vertex) {
        int idx;
        if (!freeList_.empty()) {
            idx = freeList_.back();
            freeList_.pop_back();
        } else {
            nodes_.emplace_back();
            idx = static_cast<int>(nodes_.size()) - 1;
        }
        Node& nd = nodes_[idx];
        nd.key = key;
        nd.vertex = vertex;
        nd.degree = 0;
        nd.mark = false;
        nd.parent = -1;
        nd.child = -1;
        nd.left = idx;
        nd.right = idx;
        nd.active = true;
        return idx;
    }

    void freeNode(int idx) {
        nodes_[idx].active = false;
        freeList_.push_back(idx);
    }

    // Inserta el nodo idx (aislado, left=right=idx) en la lista circular de raíces.
    void rootInsert(int idx) {
        if (minIdx_ == -1) {
            nodes_[idx].left = idx;
            nodes_[idx].right = idx;
            return;
        }
        int r = minIdx_;
        int rRight = nodes_[r].right;
        nodes_[r].right = idx;
        nodes_[idx].left = r;
        nodes_[idx].right = rRight;
        nodes_[rRight].left = idx;
    }

    void rootRemove(int idx) {
        int l = nodes_[idx].left;
        int r = nodes_[idx].right;
        nodes_[l].right = r;
        nodes_[r].left = l;
    }

    // Vincula child como hijo de parent (usado durante consolidate).
    void linkNodes(int child, int parent) {
        rootRemove(child);
        nodes_[child].parent = parent;
        if (nodes_[parent].child == -1) {
            nodes_[parent].child = child;
            nodes_[child].left = child;
            nodes_[child].right = child;
        } else {
            int c = nodes_[parent].child;
            int cRight = nodes_[c].right;
            nodes_[c].right = child;
            nodes_[child].left = c;
            nodes_[child].right = cRight;
            nodes_[cRight].left = child;
        }
        ++nodes_[parent].degree;
        nodes_[child].mark = false;
    }

    void consolidate() {
        // Recolectar raíces actuales.
        std::vector<int> roots;
        int start = minIdx_;
        int c = start;
        do {
            roots.push_back(c);
            c = nodes_[c].right;
        } while (c != start);

        int maxDegree = static_cast<int>(std::log2(std::max(1, size_))) + 2;
        std::vector<int> degreeTable(maxDegree + 1, -1);

        for (int w : roots) {
            int x = w;
            int d = nodes_[x].degree;
            while (d < static_cast<int>(degreeTable.size()) && degreeTable[d] != -1) {
                int y = degreeTable[d];
                if (nodes_[x].key > nodes_[y].key) std::swap(x, y);
                linkNodes(y, x);
                degreeTable[d] = -1;
                ++d;
                if (d >= static_cast<int>(degreeTable.size())) {
                    degreeTable.resize(d + 1, -1);
                }
            }
            if (d >= static_cast<int>(degreeTable.size())) {
                degreeTable.resize(d + 1, -1);
            }
            degreeTable[d] = x;
        }

        minIdx_ = -1;
        for (int idx : degreeTable) {
            if (idx == -1) continue;
            nodes_[idx].left = idx;
            nodes_[idx].right = idx;
            if (minIdx_ == -1) {
                minIdx_ = idx;
            } else {
                rootInsert(idx);
                if (nodes_[idx].key < nodes_[minIdx_].key) {
                    minIdx_ = idx;
                }
            }
        }
    }

    void cut(int x, int p) {
        // Quitar x de la lista de hijos de p.
        if (nodes_[p].child == x) {
            nodes_[p].child = (nodes_[x].right == x) ? -1 : nodes_[x].right;
        }
        int l = nodes_[x].left;
        int r = nodes_[x].right;
        nodes_[l].right = r;
        nodes_[r].left = l;
        --nodes_[p].degree;

        nodes_[x].left = x;
        nodes_[x].right = x;
        nodes_[x].parent = -1;
        nodes_[x].mark = false;
        rootInsert(x);
    }

    void cascadingCut(int y) {
        int z = nodes_[y].parent;
        if (z != -1) {
            if (!nodes_[y].mark) {
                nodes_[y].mark = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }
};
