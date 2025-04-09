#include <iostream>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"

void init(LatLng ref, int res) {
    AddrIdxBiMap allocd;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != 0) {
        std::cerr << "Error converting coordinate to H3 index." << std::endl;
        return;
    }

    Address addr(false);

    allocd.insert(idx, addr);

    H3Index* out;
    if gridDisk(idx, res, out) != 0 {
        std::cerr << "Error finding index " << idx << " neigbhours." << std::endl;
        return;
    }

    int l = out.size();
    for (int i = 0; i < l; i++) {
        H3Index h3 = out[i];
        if (h3 == 0) {
            continue;
        }

        addr = addr(false);
        allocd.insert(h3, addr);
    }



    



}
