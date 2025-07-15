#include <iostream>
#include <queue>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "errors.h"
#include "resolution.h"

extern "C" {
    #include "h3lib/include/localij.h"
    #include "h3lib/include/coordijk.h"
}

#define MAX_NEIGHBORS 7
#define PENTAGON_VALUE 0
#define DISTANCE 1

int count = 0;
int countPrime = 0;

void initNeighbours(AddrIdxBiMap& allocd, AddrIdx origin) {
    std::queue<AddrIdx> cells_queue;
    cells_queue.push(origin);

    do {
        origin = cells_queue.front();
        cells_queue.pop();

        // Fetch current origin neighbours.
        H3Index out[MAX_NEIGHBORS];
        if (gridDisk(origin.idx, DISTANCE, out) != E_SUCCESS) {
            std::cerr << Errors::getNeighborsSearchError(origin.idx) << std::endl;
            return;
        }

        // Compute IJK coordinates from origin to origin.
        CoordIJK originOutput;
        if (const H3Error e = cellToLocalIjk(origin.idx, origin.idx, &originOutput); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            return;
        }

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

            output.i -= originOutput.i;
            output.j -= originOutput.j;
            output.k -= originOutput.k;
            _ijkNormalize(&output);

            if (origin.addr.prime()) {
                _ijkRotate60cw(&output);
            }

            Address newAddr = origin.addr.copy();
            newAddr.push(&output);

            // Address already allocated.
            if (allocd.tryGetIdx(newAddr).has_value()) {
                continue;
            }

            allocd.insert({h3, newAddr});
            cells_queue.push({h3, newAddr});
            if (origin.addr.prime())
                countPrime += 1;
            else
                count += 1;
        }
    } while (!cells_queue.empty());
}

void init(const int satellites) {
    AddrIdxBiMap allocd;
    AddrIdxBiMap allocdPrime;

    H3Index idx = getOriginForResolution(getResolution(satellites));
    count += 1;
    countPrime += 1;
    const Address addr(false);
    const Address addrPrime(true);

    allocd.insert({idx, addr});
    allocdPrime.insert({idx, addrPrime});

    std::cout << "Address " << std::hex << idx << ": " << addr << std::endl;
    initNeighbours(allocd, {idx, addr});

    std::cout << "Number of addresses: " << count << " - " << countPrime << std::endl;
}

