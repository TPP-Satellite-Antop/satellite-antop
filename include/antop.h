#ifndef ANTOP_H
#define ANTOP_H

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "cell.h"
#include "latLng.h"

static constexpr int MAX_NEIGHBORS = 7;
static constexpr int DISTANCE = 1;

class Antop {
    std::unordered_map<H3Index, std::vector<H3Index>> neighborsByIdx{};
    std::unordered_map<H3Index, Cell> cellByIdx;
    std::unordered_map<Address, H3Index> addresses;
    static constexpr int INVALID_IDX = 0;

    bool isNewAddrValid(const Address& originAddr, H3Index idx);

    void buildNeighborGraph();

    void allocateBaseAddress(H3Index origin, H3Index idx, std::queue<H3Index> &cells_queue);

    void allocateSupplementaryAddresses();

    void allocateBaseAddresses(H3Index idx);

    int neighbors();

    void allocateAddresses(int res);

public:
    Antop() = default; // ToDo: replace default to custom constructor. This constructor should reserve memory for all three maps to remove rehashing altogether.
    void init(int satellites);
    H3Index getNextHopId(H3Index src, H3Index dst, H3Index lastHop, bool isDstValid(H3Index idx)) const;
    /*TODO
    std::string load(H3Index dir);
    H3Index getDir(std::string id);
    */
};

#endif //ANTOP_H