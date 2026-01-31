#include "h3util.h"

// Minimum number of nodes recommended to run simulations at each resolution. Values represent a potential coverage
// of 80%, assuming perfect distribution.
constexpr std::array thresholdByRes = { 0, 674, 4706 };

int findResolution(const size_t satellites) {
    for (int i = maxRes - 1; i >= 0; i--) {
        if (satellites >= thresholdByRes[i])
            return i;
    }
    return 0; // Unreachable
}