#include <iostream>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "errors.h"

extern "C" {
    #include "h3lib/include/localij.h"
}

#define MAX_NEIGHBORS 6
#define PENTAGON_VALUE 0


std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << static_cast<int>(vec[i]);
        if (i < vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

void initNeighbors(AddrIdxBiMap allocd, H3Index idx, Address addr, int res) {
  	H3Index out[MAX_NEIGHBORS];
  	if (gridDisk(idx, 1, out) != E_SUCCESS) {
   		std::cerr << Errors::getNeighborsSearchError(idx) << std::endl;
    	return;
  	}

    std::cout << "Origin: " << idx << std::endl;


    for (ssize_t i = 0; i < MAX_NEIGHBORS; i++) {
        H3Index h3 = out[i];
        if (h3 == PENTAGON_VALUE) {
            continue;
        }

        CoordIJK output;
        if (const H3Error e = cellToLocalIjk(idx, h3, &output); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            continue;
        }

        Address newAddr = addr.copy();
        newAddr.push(&output);

        std::cout << newAddr.data() << std::endl;

        // allocd.insert(h3, Address(false));
    }
}

void init(LatLng ref, int res) {
    AddrIdxBiMap allocd;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    const Address addr(false);
    allocd.insert(idx, addr);
    initNeighbors(allocd, idx, addr, res);
}

