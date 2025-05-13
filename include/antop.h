#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include "hex.h"

class Antop {
    std::map<H3Index, Hex> hexes;

public:
    Antop();
};

void init(LatLng ref, int res);

#endif //ANTOP_H
