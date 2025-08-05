#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include <unordered_map>
#include "cell.h"
#include "latLng.h"
#include "addrIdxBiMap.h"

class Antop {
private:
    std::unordered_map<H3Index, Cell> cellByIdx;
    int count;
    static const int MAX_NEIGHBORS = 7;
    static const int PENTAGON_VALUE = 0;
    static const int DISTANCE = 1;

    void initNeighbours(AddrIdx origin);
    static H3Index getOriginForResolution(int res);
    int neighbours() const;

public:
    Antop() : count(0) {}
    void init(LatLng ref, int res);
};

#endif //ANTOP_H