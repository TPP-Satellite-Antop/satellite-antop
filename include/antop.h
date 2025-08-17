#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include <queue>
#include <unordered_map>
#include "cell.h"
#include "latLng.h"
#include "addrIdxBiMap.h"

class Antop {
    std::unordered_map<H3Index, Cell> cellByIdx;
    std::unordered_map<Address, H3Index> addresses;
    static constexpr int MAX_NEIGHBORS = 7;
    static constexpr int INVALID_IDX = 0;
    static constexpr int DISTANCE = 1;

    static CoordIJK getNeighborCoordinates(H3Index origin, H3Index neighbor) ;

    bool tryAddAddress(const Address &addr, Cell &cell, H3Index idx);

    bool isNewAddrValid(const Address& originAddr, H3Index idx);

    void allocateBaseAddress(H3Index origin, H3Index idx, std::queue<H3Index> &cells_queue);

    void allocateSupplementaryAddresses();

    void allocateBaseAddresses(H3Index idx);
    static H3Index getOriginForResolution(int res);
    int neighbours() ;

    void allocateAddresses(LatLng ref, int res);

public:
    Antop() = default;
    void init(LatLng ref, int res);
};

#endif //ANTOP_H