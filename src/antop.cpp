#include <iostream>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "errors.h"

extern "C" {
    #include "h3lib/include/localij.h"
}

#define MAX_NEIGHBORS 7
#define PENTAGON_VALUE 0
#define DISTANCE 1

int count = 0;
int countPrime = 0;

void initNeighbors(AddrIdxBiMap& allocd, const AddrIdx& origin) {
    H3Index out[MAX_NEIGHBORS];
  	if (gridDisk(origin.idx, DISTANCE, out) != E_SUCCESS) {
   		std::cerr << Errors::getNeighborsSearchError(origin.idx) << std::endl;
    	return;
  	}

    std::vector<AddrIdx> addrIdxArray;

    for (const unsigned long h3 : out) {
        // Invalid index or hex already mapped with an address.
        if (h3 == PENTAGON_VALUE || h3 == origin.idx || allocd.tryGetAddr(h3).has_value()) {
            continue;
        }

        CoordIJK output;
        if (const H3Error e = cellToLocalIjk(origin.idx, h3, &output); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            continue;
        }

        if (origin.addr.prime()) {
            _ijkRotate60cw(&output);
        }

        Address newAddr = origin.addr.copy();
        newAddr.push(&output);

        // Address already allocated.
        if (allocd.tryGetIdx(newAddr).has_value()) {
            std::cout << "Address already allocd for: " << h3 << std::endl;
            continue;
        }

        allocd.insert({h3, newAddr});
        addrIdxArray.push_back({h3, newAddr});
        if (origin.addr.prime())
            countPrime += 1;
        else
            count += 1;
    }

    for (const AddrIdx &val : addrIdxArray) {
        initNeighbors(allocd, val);
    }
}

void init(const LatLng ref, const int res) {
    AddrIdxBiMap allocd;
    AddrIdxBiMap allocdPrime;

    H3Index idx;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    CoordIJK output;
    if (const H3Error e = cellToLocalIjk(idx, idx, &output); e != 0) {
        std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
    }

    count += 1;
    countPrime += 1;
    const Address addr(false);
    const Address addrPrime(true);

    allocd.insert({idx, addr});
    allocdPrime.insert({idx, addrPrime});

    initNeighbors(allocd, {idx, addr});
    initNeighbors(allocdPrime, {idx, addrPrime});
    std::cout << "Number of addresses: " << count << " - " << countPrime << std::endl;
}

