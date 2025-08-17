#include <map>
#include <stdexcept>
#include "resolution.h"
#include "errors.h"

namespace H3Origins {
    constexpr H3Index ORIGIN_RES_0 = 0x8047fffffffffff;
    constexpr H3Index ORIGIN_RES_1 = 0x81463ffffffffff;
    constexpr H3Index ORIGIN_RES_2 = 0x824607fffffffff;
}

namespace H3ResolutionLimits {
    constexpr int MAX = 2;
}

const std::map<int, H3Index> ORIGIN_BY_RES = {
    {0, H3Origins::ORIGIN_RES_0},
    {1, H3Origins::ORIGIN_RES_1},
    {2, H3Origins::ORIGIN_RES_2},
};

const std::map<int, int> MAX_HEX_BY_RES = {
    {0, 122},
    {1, 842},
    {2, 5882},
};

H3Index getOriginForResolution(const int res) {
    const auto it = ORIGIN_BY_RES.find(res);
    if (it == ORIGIN_BY_RES.end())
        throw std::out_of_range{Errors::RESOLUTION_NOT_SUPPORTED};
    return it->second;
}

int getResolution(const int satellites) {
    for (auto const& [res, max] : MAX_HEX_BY_RES) {
        if (satellites <= max)
            return res;
    }
    return H3ResolutionLimits::MAX;
}
