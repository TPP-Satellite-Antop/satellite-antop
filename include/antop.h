#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include "cell.h"

class Antop {
    std::map<H3Index, Cell> hexes;

public:
    Antop();
};

void init(LatLng ref, int res);

#endif //ANTOP_H
