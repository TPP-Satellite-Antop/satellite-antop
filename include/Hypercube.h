#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <queue>
#include <unordered_map>

#include "Cell.h"
#include "latLng.h"

class Hypercube {
    std::unordered_map<H3Index, Cell> cellByIdx;
    std::unordered_map<Address, H3Index> addresses; // ToDo: move as function parameter as it's only needed when building the hypercube.

    bool isNewAddrValid(const Address& addr, H3Index idx);

    void allocateBaseAddress(H3Index origin, H3Index idx, std::queue<H3Index> &cells_queue);

    void allocateSupplementaryAddresses();

    void allocateBaseAddresses(H3Index idx);

    void allocateAddresses(H3Index origin);

public:
    explicit Hypercube(H3Index origin);

    int distance(H3Index idx1, H3Index idx2) const;
};

#endif //HYPERCUBE_H