#include <iostream>
#include <queue>
#include "antop.h"
#include "address.h"
#include "errors.h"
#include "resolution.h"

extern "C" {
    #include "h3lib/include/localij.h"
    #include "h3lib/include/coordijk.h"
}

CoordIJK Antop::getNeighborCoordinates(const H3Index origin, const H3Index neighbor) {
    CoordIJK output, originOutput;

    H3Error e = cellToLocalIjk(origin, origin, &originOutput);
    if (e != E_SUCCESS) throw Errors::localCoordIJK(e, origin, origin);

    e = cellToLocalIjk(origin, neighbor, &output);
    if (e != E_SUCCESS) throw Errors::localCoordIJK(e, origin, neighbor);

    output.i -= originOutput.i;
    output.j -= originOutput.j;
    output.k -= originOutput.k;
    _ijkNormalize(&output); // ToDo: validate if it's even necessary.

    return output;
}

bool Antop::isNewAddrValid(const Address& addr, const H3Index idx) {
    if (addresses.contains(addr)) return false;

    H3Index neighbours[MAX_NEIGHBORS];

    if (const H3Error err = gridDisk(idx, DISTANCE, neighbours); err != E_SUCCESS) {
        throw Errors::fetchNeighbors(err, idx);
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

void Antop::allocateBaseAddress(const H3Index origin, const H3Index idx, std::queue<H3Index>& cells_queue) {
    if (idx == INVALID_IDX || idx == origin || cellByIdx.contains(idx)) {
        return;
    }

    const CoordIJK output = getNeighborCoordinates(origin, idx);
    CoordIJK primeOutput = output;

    _ijkRotate60cw(&primeOutput);

    Address newAddr = cellByIdx[origin].addresses[0].copy();
    Address newAddrPrime = cellByIdx[origin].addresses[1].copy();

    newAddr.push(&output);
    newAddrPrime.push(&primeOutput);

    if (!isNewAddrValid(newAddr, idx) || !isNewAddrValid(newAddrPrime, idx)) {
        return;
    }

    addresses.insert_or_assign(newAddr, idx);
    addresses.insert_or_assign(newAddrPrime, idx);

    Cell& cell = cellByIdx[idx];
    cell.addAddress(newAddr);
    cell.addAddress(newAddrPrime);

    cells_queue.push(idx);
}

void Antop::allocateSupplementaryAddresses() {
    H3Index out[MAX_NEIGHBORS];
    
    for (const auto& [idx, cell1] : cellByIdx) {
        if (const H3Error err = gridDisk(idx, DISTANCE, out); err != E_SUCCESS) {
            throw Errors::fetchNeighbors(err, idx);
        }

        for (const unsigned long h3 : out) {
            if (h3 == INVALID_IDX || h3 == idx) {
                continue;
            }

            Cell& cell2 = cellByIdx[h3];
            if (cell1.distanceTo(&cell2) <= 1) {
                continue;
            }

            const CoordIJK output = getNeighborCoordinates(idx, h3);

            for (const auto& addr : cell1.addresses) {
                CoordIJK auxOutput = output;
                if (addr.prime()) {
                    _ijkRotate60cw(&auxOutput);
                }

                if (const Direction direction = _unitIjkToDigit(&auxOutput); direction != I_AXES_DIGIT && direction != J_AXES_DIGIT && direction != K_AXES_DIGIT)
                    continue;

                auto newAddr = addr.copy();
                newAddr.push(&auxOutput);

                if (isNewAddrValid(newAddr, h3)) {
                    addresses.insert_or_assign(newAddr, h3);
                    cell2.addAddress(newAddr);
                    break;
                }
            }
        }
    }
}

void Antop::allocateBaseAddresses(H3Index idx) {
    std::queue<H3Index> cells_queue;
    cells_queue.push(idx);
    H3Index neighbours[MAX_NEIGHBORS];

    while (!cells_queue.empty()) {
        idx = cells_queue.front();
        cells_queue.pop();

        if (const H3Error err = gridDisk(idx, DISTANCE, neighbours); err != E_SUCCESS) {
            throw Errors::fetchNeighbors(err, idx);
        }

        CoordIJK originOutput;
        if (const H3Error err = cellToLocalIjk(idx, idx, &originOutput); err != E_SUCCESS) {
            throw Errors::localCoordIJK(err, idx, idx);
        }

        for (const unsigned long h3 : neighbours) {
            allocateBaseAddress(idx, h3, cells_queue);
        }
    }
}

// ToDo: remove altogether when we are certain the algorithm works flawlessly, or keep as a safeguard to validate under a config flag.
int Antop::neighbours() {
    int neighbourCount = 0;
    for (const auto& [idx1, cell1] : cellByIdx) {
        H3Index neighbours[MAX_NEIGHBORS];
        if (const H3Error err = gridDisk(idx1, DISTANCE, neighbours); err != E_SUCCESS) {
            throw Errors::fetchNeighbors(err, idx1);
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
                    std::cerr << "Cell " << std::hex << idx1 << " is not a neighbor of cell " << std::hex << idx2 << std::dec << std::endl;
                }
                neighbourCount++;
            }
        }
    }
    return neighbourCount;
}

void Antop::allocateAddresses(const int res) {
    H3Index idx = getOriginForResolution(res);

    auto baseCell = Cell();

    const Address addr(false);
    const Address addrPrime(true);

    baseCell.addAddress(addr);
    baseCell.addAddress(addrPrime);

    cellByIdx.insert({idx, baseCell});

    allocateBaseAddresses(idx);
    allocateSupplementaryAddresses();
}

void Antop::init(const int satellites) {
    const int res = getResolution(satellites);
    allocateAddresses(res);

    std::cout << "Resolution: " << std::dec << res << std::endl;
    std::cout << "Unique Cells: " << std::dec << cellByIdx.size() << std::endl;
    std::cout << "Number of addresses: " << std::dec << addresses.size() << std::endl;
    std::cout << std::dec << "Missing neighbors: " << (CELLS_BY_RESOLUTION.at(res) - 12) * 6 + 60 - neighbours() << std::endl << std::endl;
}