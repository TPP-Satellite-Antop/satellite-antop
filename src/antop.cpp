#include <iostream>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "errors.h"

extern "C" {
    #include "h3lib/include/localij.h"
}

#define MAX_NEIGHBORS 7 //including itself
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

void initNeighbors(AddrIdxBiMap allocd, const AddrIdx &origin) {
  	H3Index out[MAX_NEIGHBORS];
  	if (gridDisk(origin.idx, 1, out) != E_SUCCESS) {
   		std::cerr << Errors::getNeighborsSearchError(origin.idx) << std::endl;
    	return;
  	}

    std::vector<AddrIdx> addrIdxArray;

    for (const unsigned long h3 : out) {
        if (h3 == origin.idx) {
            continue;
        }
        // Invalid index or hex already mapped with an address.
        if (h3 == PENTAGON_VALUE || allocd.tryGetAddr(h3)) {
            continue;
        }

        CoordIJK output;
        if (const H3Error e = cellToLocalIjk(origin.idx, h3, &output); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            continue;
        }

        Address newAddr = origin.addr.copy();
        newAddr.push(&output);

        // Address already allocated.
        if (allocd.tryGetIdx(newAddr)) {
            continue;
        }

        allocd.insert(h3, newAddr);
        addrIdxArray.push_back({h3, newAddr});
    }

    for (const AddrIdx& val : addrIdxArray) {
        initNeighbors(allocd, val);
    }

    std::cout <<"Finished a grid" << std::endl;
}

void init(const LatLng ref, const int res) {
    AddrIdxBiMap allocd;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    const Address addr(false);
    allocd.insert(idx, addr);
    initNeighbors(allocd, {idx, addr});
}

