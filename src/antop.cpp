#include <iostream>
#include <queue>

#include "antop.h"
#include "address.h"
#include "addrIdxBiMap.h"
#include "cell.h"
#include "errors.h"

extern "C" {
    #include "h3lib/include/localij.h"
    #include "h3lib/include/coordijk.h"
}

#define MAX_NEIGHBORS 7
#define PENTAGON_VALUE 0
#define DISTANCE 1

int count = 0;

void initNeighbours(std::unordered_map<H3Index, Cell>& cellByIdx, AddrIdx origin) {
    std::unordered_map<Address, bool> addresses;

    std::queue<AddrIdx> cells_queue;
    cells_queue.push(origin);

    H3Index out[MAX_NEIGHBORS];

    do {
        origin = cells_queue.front();
        cells_queue.pop();

        //std::cout << std::hex << origin.idx << std::endl;

        // Fetch current origin neighbours.

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
            // Invalid index or cell already allocated.
            if (h3 == PENTAGON_VALUE || h3 == origin.idx || cellByIdx.contains(h3)) {
                continue;
            }

            Cell& cell = cellByIdx[h3];

            /*if (cell.distanceTo(&cellByIdx[origin.idx]) < std::numeric_limits<int>::max()) {
                continue;
            }*/

            // ToDo: Check if the cell is already allocated.

            CoordIJK output;
            if (const H3Error e = cellToLocalIjk(origin.idx, h3, &output); e != 0) {
                std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
                continue;
            }

            output.i -= originOutput.i;
            output.j -= originOutput.j;
            output.k -= originOutput.k;

            CoordIJK primeOutput = output;

            _ijkNormalize(&output);
            _ijkRotate60cw(&primeOutput);

            Address newAddr = origin.addr.copy();
            Address newAddrPrime = origin.addrPrime.copy();

            newAddr.push(&output);
            newAddrPrime.push(&primeOutput);

            // Address already allocated.
            if (addresses.contains(newAddr) || addresses.contains(newAddrPrime)) {
                continue;
            }

            addresses.insert_or_assign(newAddr, true);
            addresses.insert_or_assign(newAddrPrime, true);

            cell.addAddress(newAddr);
            cell.addAddress(newAddrPrime);

            cells_queue.push({h3, newAddr, newAddrPrime});
            count += 1;
        }
    } while (!cells_queue.empty());

    for (const auto& [idx, cell1] : cellByIdx) {
        if (gridDisk(idx, DISTANCE, out) != E_SUCCESS) {
            exit(2);
        }

        CoordIJK originOutput;
        if (const H3Error e = cellToLocalIjk(idx, idx, &originOutput); e != 0) {
            exit(3);
        }

        for (const unsigned long h3 : out) {
            auto cell2 = cellByIdx[h3];
            if (h3 == PENTAGON_VALUE || h3 == idx || cell1.distanceTo(&cell2) <= 2) {
                continue;
            }


            CoordIJK output;
            if (const H3Error e = cellToLocalIjk(idx, h3, &output); e != 0) {
                exit(4);
            }

            output.i -= originOutput.i;
            output.j -= originOutput.j;
            output.k -= originOutput.k;

            for (const auto& addr : cell1.addresses) {
                auto a = addr.copy();
                if (a.prime()) {
                    _ijkRotate60cw(&output);
                } else {
                    _ijkNormalize(&output);
                }

                a.push(&output);
                if (!addresses.contains(a)) {
                    addresses.insert_or_assign(a, true);
                    cell2.addAddress(a);
                    count += 1;
                    break;
                }
            }
        }
    }
}

H3Index getOriginForResolution(const int res) {
    switch (res) {
        case 1:
            return 0x81463ffffffffff;
        case 2:
            return 0x824607fffffffff;
        case 3:
            return 0x834600fffffffff;
        case 4:
            return 0x8446001ffffffff;
        case 5:
            return 0x85460003fffffff;
        default:
            throw std::out_of_range{Errors::RESOLUTION_NOT_SUPPORTED};
    }
}


int neighbours(const std::unordered_map<H3Index, Cell>& cellByIdx) {
    int neighbourCount = 0;
    for (const auto& [idx1, cell1] : cellByIdx) {
        for (const auto& [idx2, cell2] : cellByIdx) {
            if (idx1 == idx2) {
                continue;
            }

            if (cell1.distanceTo(&cell2) == 1) {
                //std::cout << std::hex << idx1 << " - " << idx2 << std::endl;
                neighbourCount++;
            }
        }
    }
    return neighbourCount;
}

void init(const LatLng ref, const int res) {
    H3Index idx = 0;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    if (idx == 0) {
        idx = getOriginForResolution(res);
    }

    std::cout << "Origin: " << std::hex << idx << std::endl;

    auto baseCell = Cell();

    const Address addr(false);
    const Address addrPrime(true);

    baseCell.addAddress(addr);
    baseCell.addAddress(addrPrime);

    count += 1;

    std::unordered_map<H3Index, Cell> cellByIdx;

    cellByIdx.insert({idx, baseCell});

    initNeighbours(cellByIdx, {idx, addr, addrPrime});

    std::cout << "Unique Cells: " << cellByIdx.size() << std::endl;
    std::cout << "Number of addresses: " << std::dec << count << std::endl;

    std::cout << "Average neighbours: " << static_cast<float>(neighbours(cellByIdx) /*+ neighbours(allocdPrime)*/) / static_cast<float>(cellByIdx.size()) << std::endl;

    count = 0;
}


//

// Addresses --> Boolean    para saber qué direcciones están ya usadas
// Idxs (H3) --> Cells      para saber qué hexágonos de H3 tienen ya al menos una dirección


// Idxs (H3) --> Cells
// Addresses --> Cells
// Addresses --> Idxs
// Cells     --> Idxs (H3)

