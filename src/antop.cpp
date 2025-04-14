#include <iostream>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "errors.h"

#define MAX_NEIGHBORS 6
#define PENTAGON_VALUE 0

void initNeighbors(AddrIdxBiMap allocd, H3Index idx, int res) {
  	H3Index* out = nullptr;
  	if (gridDisk(idx, res, out) != E_SUCCESS) {
   		std::cerr << Errors::getNeighborsSearchError(idx) << std::endl;
    	return;
  	}

    for (ssize_t i = 0; i < MAX_NEIGHBORS; i++) {
        H3Index h3 = out[i];
        if (h3 == PENTAGON_VALUE) {
            continue;
        }

        allocd.insert(h3, Address(false));
    }
}

void init(LatLng ref, int res) {
    AddrIdxBiMap allocd;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    Address addr(false);
    allocd.insert(idx, addr);
    initNeighbors(allocd, idx, res);
}

