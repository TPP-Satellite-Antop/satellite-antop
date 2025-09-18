#include <map>

#include "resolution.h"
#include "errors.h"

namespace H3ResolutionLimits {
    constexpr int MAX = 2;
}

const std::map<int, H3Index> ORIGIN_BY_RES = {
    {0, 0x8047fffffffffff},
    {1, 0x81463ffffffffff},
    {2, 0x824607fffffffff},
};

H3Index getOriginForResolution(const int res) {
    const auto it = ORIGIN_BY_RES.find(res);
    if (it == ORIGIN_BY_RES.end())
        throw Errors::RESOLUTION_NOT_SUPPORTED;
    return it->second;
}

int findResolution(const int satellites) {
    for (auto const& [res, max] : CELLS_BY_RESOLUTION) {
        if (satellites <= max)
            return res;
    }
    return H3ResolutionLimits::MAX;
}
