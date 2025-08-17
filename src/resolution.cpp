#include "resolution.h"
#include "errors.h"
#include <stdexcept>

namespace H3Origins {
    constexpr H3Index ORIGIN_RES_0 = 0x8047fffffffffff;
    constexpr H3Index ORIGIN_RES_1 = 0x81463ffffffffff;
    constexpr H3Index ORIGIN_RES_2 = 0x824607fffffffff;
}

constexpr int MAX_HEX_BY_RES[] = {
    122,
    842,
    5882,
};

namespace H3ResolutionLimits {
    constexpr int MIN = 0;
    constexpr int MAX = 2;
}

H3Index getOriginForResolution(const int res) {
    switch (res) {
        case H3ResolutionLimits::MIN: return H3Origins::ORIGIN_RES_0;
        case 1: return H3Origins::ORIGIN_RES_1;
        case H3ResolutionLimits::MAX: return H3Origins::ORIGIN_RES_2;
        default:
            throw std::out_of_range{Errors::RESOLUTION_NOT_SUPPORTED};
    }
}

//TODO hablar con hamelin
int getResolution(const int satellites) {
    for (int res = 0; res < H3ResolutionLimits::MAX; res++) {
        if (satellites <= MAX_HEX_BY_RES[res])
            return res;
    }

    return H3ResolutionLimits::MAX;
}