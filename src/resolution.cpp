#include "resolution.h"
#include "errors.h"
#include <stdexcept>

namespace H3Origins {
    constexpr H3Index ORIGIN_RES_1 = 0x81463ffffffffff;
    constexpr H3Index ORIGIN_RES_2 = 0x824607fffffffff;
    constexpr H3Index ORIGIN_RES_3 = 0x834600fffffffff;
    constexpr H3Index ORIGIN_RES_4 = 0x8446001ffffffff;
    constexpr H3Index ORIGIN_RES_5 = 0x85460003fffffff;
}

constexpr int MAX_HEX_BY_RES[] = {
    110,
    830,
    5870,
    41150,
    288110
};

namespace H3ResolutionLimits {
    constexpr int MIN = 1;
    constexpr int MAX = 5;
}

H3Index getOriginForResolution(const int res) {
    switch (res) {
        case H3ResolutionLimits::MIN: return H3Origins::ORIGIN_RES_1;
        case 2: return H3Origins::ORIGIN_RES_2;
        case 3: return H3Origins::ORIGIN_RES_3;
        case 4: return H3Origins::ORIGIN_RES_4;
        case H3ResolutionLimits::MAX: return H3Origins::ORIGIN_RES_5;
        default:
            throw std::out_of_range{Errors::RESOLUTION_NOT_SUPPORTED};
    }
}


int getResolution(const int satellites) {
    for (int res = 0; res < H3ResolutionLimits::MAX; res++) {
        if (satellites <= MAX_HEX_BY_RES[res])
            return res+1;
    }

    throw std::out_of_range("Too many satellites for supported resolutions");
}