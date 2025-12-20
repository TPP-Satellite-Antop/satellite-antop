#include <map>

#include "resolution.h"
#include "errors.h"

namespace H3ResolutionLimits {
    constexpr int MAX_SUPPORTED = 3;
}

const std::map<int, H3Index> ORIGIN_BY_RES = {
    {0, 0x8047fffffffffff},
    {1, 0x81463ffffffffff},
    {2, 0x824607fffffffff},
};

// Minimum number of nodes recommended to run simulations at each resolution. Values represent a potential coverage
// of 80%, assuming perfect distribution.
constexpr int NODE_THRESHOLD[H3ResolutionLimits::MAX_SUPPORTED] = {
    0,
    674,
    4706,
};

H3Index getOriginForResolution(const int res) {
    const auto it = ORIGIN_BY_RES.find(res);
    if (it == ORIGIN_BY_RES.end())
        throw Errors::RESOLUTION_NOT_SUPPORTED;
    return it->second;
}

int findResolution(const int satellites) {
    for (int i = H3ResolutionLimits::MAX_SUPPORTED - 1; i >= 0; i--) {
        if (satellites >= NODE_THRESHOLD[i])
            return i;
    }
    return 0;
}
