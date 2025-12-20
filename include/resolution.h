#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <map>
#include <h3api.h>

namespace H3ResolutionLimits {
    constexpr int MAX_SUPPORTED = 3;
}

constexpr int CELLS_BY_RESOLUTION[H3ResolutionLimits::MAX_SUPPORTED] = {
    122,
    842,
    5882,
};

H3Index getOriginForResolution(int res);
int findResolution(int satellites);

#endif //RESOLUTION_H
