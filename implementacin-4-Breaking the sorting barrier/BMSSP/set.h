#pragma once
#include <unordered_set>
#include <vector>

namespace bmssp {

    class Set {
    private:
        std::unordered_set<int> m_;

    public:
        Set() = default;

        void add(int v) {
            m_.insert(v);
        }

        bool has(int v) const {
            return m_.find(v) != m_.end();
        }

        int size() const {
            return static_cast<int>(m_.size());
        }

        std::vector<int> toSlice() const {
            return std::vector<int>(m_.begin(), m_.end());
        }

        void clear() {
            m_.clear();
        }

        // Exponer el iterador interno para iterar directamente en bucles rango-for
        auto begin() const { return m_.begin(); }
        auto end() const { return m_.end(); }
    };

} // namespace bmssp