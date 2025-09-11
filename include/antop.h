#ifndef ANTOP_H
#define ANTOP_H

#include <queue>
#include <unordered_map>
#include "cell.h"
#include "latLng.h"

class Antop {
    std::unordered_map<H3Index, Cell> cellByIdx;
    std::unordered_map<Address, H3Index> addresses;
    static constexpr int MAX_NEIGHBORS = 7;
    static constexpr int INVALID_IDX = 0;
    static constexpr int DISTANCE = 1;

    static CoordIJK getNeighborCoordinates(H3Index origin, H3Index neighbor);

    bool isNewAddrValid(const Address& originAddr, H3Index idx);

    void allocateBaseAddress(H3Index origin, H3Index idx, std::queue<H3Index> &cells_queue);

    void allocateSupplementaryAddresses();

    void allocateBaseAddresses(H3Index idx);

    int neighbours();

    void allocateAddresses(int res);

public:
    Antop() = default;
    void init(int satellites);
    H3Index getNextHopId(H3Index src, H3Index dst);
    /*TODO
    std::string load(H3Index dir);
    H3Index getDir(std::string id);
    */
};

#endif //ANTOP_H