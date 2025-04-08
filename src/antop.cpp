#include <iostream>

#include "antop.h"
#include "address.h"

void init(LatLng ref, int res) {
    std::map<H3Index, Address> allocdIdx;
    std::map<std::vector<uint8_t>, H3Index> allocdAddr;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != 0) {
        std::cerr << "Error converting coordinate to H3 index!" << std::endl;
        return;
    }

    Address addr(false);

    allocdIdx.insert_or_assign(idx, addr);
    allocdAddr.insert_or_assign(addr.data(), idx);

    



}
