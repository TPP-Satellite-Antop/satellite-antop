#include <iostream>
#include <queue>
#include "antop.h"
#include "address.h"
#include "errors.h"
#include "mathExtensions.h"

extern "C" {
    #include "h3lib/include/localij.h"
    #include "h3lib/include/coordijk.h"
}

H3Error Antop::getNeighborCoordinates(const H3Index origin, const H3Index neighbor, CoordIJK& output) {
    CoordIJK originOutput;
    H3Error e = cellToLocalIjk(origin, origin, &originOutput);
    if (e != 0) return e;

    e = cellToLocalIjk(origin, neighbor, &output);
    if (e != 0) return e;

    output.i -= originOutput.i;
    output.j -= originOutput.j;
    output.k -= originOutput.k;
    
    return E_SUCCESS;
}

bool Antop::tryAddAddress(const Address& addr, Cell& cell, std::unordered_map<Address, H3Index>& addresses, H3Index idx) {
    if (addresses.contains(addr))
        return false;
    addresses.insert_or_assign(addr, idx);
    cell.addAddress(addr);
    count += 1;
    return true;
}

bool Antop::isNewAddrValid(const Address& addr, const H3Index idx, std::unordered_map<Address, H3Index>& addresses) {
    H3Index neighbours[MAX_NEIGHBORS];

    if (gridDisk(idx, DISTANCE, neighbours) != E_SUCCESS) {
        std::cerr << Errors::getNeighborsSearchError(idx) << std::endl;
        return false;
    }

    auto aux = addr.copy();

    for (int i = static_cast<int>(aux._data.size()) - 1; i >= 0; i--) {
        const uint8_t originalByte = aux._data[i];

        for (int bitPos = 0; bitPos < 6; bitPos++) {
            const uint8_t mask = 1 << bitPos;

            aux._data[i] = originalByte ^ mask;

            if (addresses.contains(aux) && std::find(neighbours, neighbours + MAX_NEIGHBORS, addresses[aux]) == neighbours + MAX_NEIGHBORS) {
                return false;
            }

            aux._data[i] = originalByte;
        }
    }

    return true;
}

void Antop::processNeighbor(const H3Index neighborIdx, const AddrIdx& origin, std::unordered_map<Address, H3Index>& addresses, std::queue<AddrIdx>& cells_queue) {
    if (neighborIdx == INVALID_IDX || neighborIdx == origin.idx || cellByIdx.contains(neighborIdx)) {
        return;
    }

    CoordIJK output;
    if (const H3Error e = getNeighborCoordinates(origin.idx, neighborIdx, output); e != 0) {
        std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
        return;
    }

    CoordIJK primeOutput = output;
    _ijkNormalize(&output);
    _ijkRotate60cw(&primeOutput);

    Address newAddr = origin.addr.copy();
    Address newAddrPrime = origin.addrPrime.copy();

    newAddr.push(&output);
    newAddrPrime.push(&primeOutput);

    if (addresses.contains(newAddr) || addresses.contains(newAddrPrime) || !isNewAddrValid(newAddr, neighborIdx, addresses) || !isNewAddrValid(newAddrPrime, neighborIdx, addresses)) {
        return;
    }

    addresses.insert_or_assign(newAddr, neighborIdx);
    addresses.insert_or_assign(newAddrPrime, neighborIdx);

    Cell& cell = cellByIdx[neighborIdx];
    cell.addAddress(newAddr);
    cell.addAddress(newAddrPrime);

    cells_queue.push({neighborIdx, newAddr, newAddrPrime});

    count += 1;
}

void Antop::processFarNeighbors(std::unordered_map<Address, H3Index>& addresses) {
    H3Index out[MAX_NEIGHBORS];
    
    for (const auto& [idx, cell1] : cellByIdx) {
        if (gridDisk(idx, DISTANCE, out) != E_SUCCESS) {
            exit(2);
        }

        for (const unsigned long h3 : out) {
            if (h3 == INVALID_IDX || h3 == idx) {
                continue;
            }

            Cell& cell2 = cellByIdx[h3];
            if (cell1.distanceTo(&cell2) <= 1) {
                continue;
            }

            CoordIJK output;
            if (const H3Error e = getNeighborCoordinates(idx, h3, output); e != 0) {
                exit(4);
            }

            for (const auto& addr : cell1.addresses) {
                if (addr.prime()) {
                    _ijkRotate60cw(&output);
                } else {
                    _ijkNormalize(&output);
                }

                auto a = addr.copy();
                a.push(&output);

                if (!isNewAddrValid(a, h3, addresses)) {
                    continue;
                }

                if (tryAddAddress(a, cell2, addresses, h3)) {
                    break;
                }
            }
        }
    }
}

void Antop::initNeighbours(AddrIdx origin) {
    std::unordered_map<Address, H3Index> addresses;
    std::queue<AddrIdx> cells_queue;
    cells_queue.push(origin);
    H3Index neighbours[MAX_NEIGHBORS];

    while (!cells_queue.empty()) {
        origin = cells_queue.front();
        cells_queue.pop();

        if (gridDisk(origin.idx, DISTANCE, neighbours) != E_SUCCESS) {
            std::cerr << Errors::getNeighborsSearchError(origin.idx) << std::endl;
            return;
        }

        CoordIJK originOutput;
        if (const H3Error e = cellToLocalIjk(origin.idx, origin.idx, &originOutput); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            return;
        }

        for (const unsigned long h3 : neighbours) {
            processNeighbor(h3, origin, addresses, cells_queue);
        }
    }

    processFarNeighbors(addresses);
}

H3Index Antop::getOriginForResolution(const int res) {
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

int Antop::neighbours() const {
    int neighbourCount = 0;
    for (const auto& [idx1, cell1] : cellByIdx) {
        H3Index neighbours[MAX_NEIGHBORS];
        if (gridDisk(idx1, DISTANCE, neighbours) != E_SUCCESS) {
            std::cerr << Errors::getNeighborsSearchError(idx1) << std::endl;
            return 0;
        }

        for (const auto& [idx2, cell2] : cellByIdx) {
            if (idx1 == idx2) {
                continue;
            }

            if (cell1.distanceTo(&cell2) == 1) {
                bool isNeighbor = false;
                for (const auto& neighbor : neighbours) {
                    if (neighbor == idx2) {
                        isNeighbor = true;
                        break;
                    }
                }

                if (!isNeighbor) {
                    std::cout << "Cell " << std::hex << idx1 << " is not a neighbor of cell " << std::hex << idx2 << std::dec << std::endl;
                }
                neighbourCount++;
            }
        }
    }
    return neighbourCount;
}

void Antop::init(const LatLng ref, const int res) {
    H3Index idx = 0;
    if (latLngToCell(&ref, res, &idx) != E_SUCCESS) {
        std::cerr << Errors::COORD_CONVERTING_ERROR << std::endl;
        return;
    }

    if (idx == 0) {
        idx = getOriginForResolution(res);
    }

    auto baseCell = Cell();

    const Address addr(false);
    const Address addrPrime(true);

    baseCell.addAddress(addr);
    baseCell.addAddress(addrPrime);

    count = 1;

    cellByIdx.insert({idx, baseCell});

    initNeighbours({idx, addr, addrPrime});

    std::cout << "Unique Cells: " << std::dec << cellByIdx.size() << std::endl;
    std::cout << "Number of addresses: " << std::dec << count << std::endl;

    const auto n = neighbours();

    std::cout << std::dec << "Neighbours: " << n << " - " << cellByIdx.size() << std::endl;
    std::cout << "Average neighbours: " << static_cast<float>(n) / static_cast<float>(cellByIdx.size()) << std::endl;

    count = 0;
}