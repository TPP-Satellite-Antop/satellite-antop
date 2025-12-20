#include <map>

#include "resolution.h"
#include "errors.h"

constexpr H3Index ORIGIN_BY_RES[H3ResolutionLimits::MAX_SUPPORTED] = {
    0x8047fffffffffff,
    0x81463ffffffffff,
    0x824607fffffffff,
};

// Minimum number of nodes recommended to run simulations at each resolution. Values represent a potential coverage
// of 80%, assuming perfect distribution.
constexpr int RES_NODE_THRESHOLD[H3ResolutionLimits::MAX_SUPPORTED] = {
    0,
    674,
    4706,
};

H3Index getOriginForResolution(const int res) {
    if (res < 0 || res >= H3ResolutionLimits::MAX_SUPPORTED)
        throw Errors::RESOLUTION_NOT_SUPPORTED;
    return ORIGIN_BY_RES[res];
}

int findResolution(const int satellites) {
    for (int i = H3ResolutionLimits::MAX_SUPPORTED - 1; i >= 0; i--) {
        if (satellites >= RES_NODE_THRESHOLD[i])
            return i;
    }
    return 0;
}
