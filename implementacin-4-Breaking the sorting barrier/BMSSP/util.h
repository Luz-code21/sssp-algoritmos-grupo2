#pragma once
#include <limits>

namespace bmssp {

    // Infinito real correspondiente a math.MaxFloat64 de Go
    const double Inf = std::numeric_limits<double>::infinity();

    // Implementa intPow2(e) de util.go
    inline int intPow2(int e) {
        if (e <= 0) {
            return 1;
        }
        return 1 << e;
    }

} // namespace bmssp