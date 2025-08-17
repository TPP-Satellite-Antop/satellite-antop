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
    int count;
    static constexpr int MAX_NEIGHBORS = 7;
    static constexpr int INVALID_IDX = 0;
    static constexpr int DISTANCE = 1;

    static H3Error getNeighborCoordinates(H3Index origin, H3Index neighbor, CoordIJK &output) ;

    bool tryAddAddress(const Address &addr, Cell &cell, H3Index idx);

    static bool isNewAddrValid(const Address& originAddr, H3Index idx);

    void processNeighbor(H3Index neighborIdx, const AddrIdx &origin, std::queue<AddrIdx> &cells_queue);

    void processFarNeighbors();

    void initNeighbours(AddrIdx origin);
    static H3Index getOriginForResolution(int res);
    int neighbours() const;

public:
    Antop() : count(0) {}
    void init(LatLng ref, int res);
};

#endif //ANTOP_H