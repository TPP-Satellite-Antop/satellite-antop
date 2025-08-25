#include <map>
#include <stdexcept>
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

const std::map<int, int> MAX_HEX_BY_RES = {
    {0, 122},
    {1, 842},
    {2, 5882},
};

H3Index getOriginForResolution(const int res) {
    const auto it = ORIGIN_BY_RES.find(res);
    if (it == ORIGIN_BY_RES.end())
        throw Errors::RESOLUTION_NOT_SUPPORTED;
    return it->second;
}

int getResolution(const int satellites) {
    for (auto const& [res, max] : MAX_HEX_BY_RES) {
        if (satellites <= max)
            return res;
    }
    return H3ResolutionLimits::MAX;
}
