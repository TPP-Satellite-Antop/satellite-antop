#include <queue>
#include <algorithm>
#include <functional>

#include "Hypercube.h"
#include "Address.h"
#include "h3util.h"
#include "errors.h"

extern "C" {
    #include "localij.h"
    #include "coordijk.h"
}

static constexpr int MAX_NEIGHBORS = 7;
static constexpr int DISTANCE = 1;
static constexpr int INVALID_IDX = 0;

Hypercube::Hypercube(const H3Index origin) {
    allocateAddresses(origin);
}

int Hypercube::distance(const H3Index idx1, const H3Index idx2) const {
    return cellByIdx.at(idx1).distanceTo(cellByIdx.at(idx2));
}

// Returns IJK coordinates according to the origin
CoordIJK getNeighborCoordinates(const H3Index origin, const H3Index neighbor) {
    CoordIJK offset, originOffset;

    H3Error e = cellToLocalIjk(origin, origin, &originOffset);
    if (e != E_SUCCESS) throw Errors::localCoordIJK(e, origin, origin);

    e = cellToLocalIjk(origin, neighbor, &offset);
    if (e != E_SUCCESS) throw Errors::localCoordIJK(e, origin, neighbor);

    offset.i -= originOffset.i;
    offset.j -= originOffset.j;
    offset.k -= originOffset.k;
    _ijkNormalize(&originOffset); // Potentially redundant, but not harmful to have

    return offset;
}

std::array<H3Index, MAX_NEIGHBORS> getNeighbors(const H3Index idx) {
    std::array<H3Index, MAX_NEIGHBORS> neighbors{};
    if (const H3Error err = gridDisk(idx, DISTANCE, neighbors.data()); err != E_SUCCESS)
        throw Errors::fetchNeighbors(err, idx);
    return neighbors;
}

bool Hypercube::isNewAddrValid(const Address& addr, const H3Index idx) {
    if (addresses.contains(addr)) return false;

    std::array<H3Index, MAX_NEIGHBORS> neighbors = getNeighbors(idx);
    Address aux = addr.copy();

    for (int i = static_cast<int>(aux._data.size()) - 1; i >= 0; i--) {
        const uint8_t originalByte = aux._data[i];

        for (int bitPos = 0; bitPos < 6; bitPos++) {
            const uint8_t mask = 1 << bitPos;

            aux._data[i] = originalByte ^ mask;

            if (addresses.contains(aux) && std::ranges::find(neighbors, addresses[aux]) == neighbors.end())
                return false;

            aux._data[i] = originalByte;
        }
    }

    return true;
}

void Hypercube::allocateBaseAddress(const H3Index origin, const H3Index idx, std::queue<H3Index>& cells_queue) {
    if (idx == INVALID_IDX || idx == origin || cellByIdx.contains(idx))
        return;

    const CoordIJK ijkOffset = getNeighborCoordinates(origin, idx);
    CoordIJK primeIjkOffset = ijkOffset;

    _ijkRotate60cw(&primeIjkOffset);

    Address newAddr = cellByIdx[origin].addresses[0].copy();
    Address newAddrPrime = cellByIdx[origin].addresses[1].copy();

    newAddr.push(&ijkOffset);
    newAddrPrime.push(&primeIjkOffset);

    if (!isNewAddrValid(newAddr, idx) || !isNewAddrValid(newAddrPrime, idx)) return;

    addresses.insert_or_assign(newAddr, idx);
    addresses.insert_or_assign(newAddrPrime, idx);

    Cell& cell = cellByIdx[idx];
    cell.addAddress(newAddr);
    cell.addAddress(newAddrPrime);
    cells_queue.push(idx);
}

void Hypercube::allocateSupplementaryAddresses() {
    for (const auto& [idx, cell1] : cellByIdx) {
        for (std::array<H3Index, MAX_NEIGHBORS> out = getNeighbors(idx); const unsigned long h3 : out) {
            if (h3 == INVALID_IDX || h3 == idx)
                continue;

            Cell& cell2 = cellByIdx[h3];
            if (cell1.distanceTo(cell2) <= 1)
                continue;

            const CoordIJK output = getNeighborCoordinates(idx, h3);

            for (const auto& addr : cell1.addresses) {
                CoordIJK auxOutput = output;
                if (addr.prime())
                    _ijkRotate60cw(&auxOutput);

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

void Hypercube::allocateBaseAddresses(H3Index idx) {
    std::queue<H3Index> cells_queue;
    cells_queue.push(idx);

    while (!cells_queue.empty()) {
        idx = cells_queue.front();
        cells_queue.pop();

        std::array<H3Index, MAX_NEIGHBORS> neighbors = getNeighbors(idx);

        CoordIJK originOutput;
        if (const H3Error err = cellToLocalIjk(idx, idx, &originOutput); err != E_SUCCESS)
            throw Errors::localCoordIJK(err, idx, idx);

        for (const unsigned long h3 : neighbors)
            allocateBaseAddress(idx, h3, cells_queue);
    }
}

void Hypercube::allocateAddresses(H3Index origin) {
    auto originCell = Cell();

    const Address addr(false);
    const Address addrPrime(true);

    originCell.addAddress(addr);
    originCell.addAddress(addrPrime);

    cellByIdx.insert({origin, originCell});

    allocateBaseAddresses(origin);
    allocateSupplementaryAddresses();
}
