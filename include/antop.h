#ifndef ANTOP_H
#define ANTOP_H

#include <functional>
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
    int resolution;
    static constexpr int INVALID_IDX = 0;

    bool isNewAddrValid(const Address& originAddr, H3Index idx);

    void buildNeighborGraph();

    void allocateBaseAddress(H3Index origin, H3Index idx, std::queue<H3Index> &cells_queue);

    void allocateSupplementaryAddresses();

    void allocateBaseAddresses(H3Index idx);

    int neighbors();

    void allocateAddresses();

    int distance(H3Index idx1, H3Index idx2);

public:
    Antop() = default; // ToDo: replace default to custom constructor. This constructor should reserve memory for all three maps to remove rehashing altogether.
    void init(int satellites);
    int getResolution() const;
    std::vector<H3Index> getHopCandidates(H3Index src, H3Index dst);
};

#endif //ANTOP_H