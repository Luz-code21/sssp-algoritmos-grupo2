#pragma once
#include <cmath>
#include <algorithm>

namespace bmssp {

    struct Options {
        int K;
        int T;

        // Implementa exactamente defaultOptions(n) de params.go
        static Options defaultOptions(int n) {
            double ln = std::log(std::max(2, n));
            int k = static_cast<int>(std::floor(std::pow(ln, 1.0 / 3.0)));
            if (k < 2) k = 2;
            int t = static_cast<int>(std::floor(std::pow(ln, 2.0 / 3.0)));
            if (t < 2) t = 2;
            return Options{ k, t };
        }
    };

} // namespace bmssp