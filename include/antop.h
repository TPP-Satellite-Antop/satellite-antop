#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include "hex.h"

class Antop {
    std::map<H3Index, Hex> hexes;

public:
    Antop();
};

#endif //ANTOP_H
