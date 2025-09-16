#include <iostream>
#include <queue>
#include <ranges>
#include "antop.h"
#include "address.h"
#include "errors.h"
#include "resolution.h"

extern "C" {
    #include "localij.h"
    #include "coordijk.h"
}

CoordIJK getNeighborCoordinates(const H3Index origin, const H3Index neighbor) {
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

std::array<H3Index, MAX_NEIGHBORS> getNeighbors(const H3Index idx) {
    std::array<H3Index, MAX_NEIGHBORS> neighbors{};
    if (const H3Error err = gridDisk(idx, DISTANCE, neighbors.data()); err != E_SUCCESS)
        throw Errors::fetchNeighbors(err, idx);
    return neighbors;
}

bool Antop::isNewAddrValid(const Address& addr, const H3Index idx) {
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

void Antop::buildNeighborGraph() {
    std::unordered_map<H3Index, std::unordered_set<H3Index>> neighborsSetByIdx{};

    for (const auto &idx: cellByIdx | std::views::keys) {
        for (const std::array<H3Index, MAX_NEIGHBORS> neighbors = getNeighbors(idx); const H3Index neighbor : neighbors) {
            if (neighbor != idx && cellByIdx[idx].distanceTo(&cellByIdx[neighbor]) == 1) {
                neighborsSetByIdx[idx].insert(neighbor);
                neighborsSetByIdx[neighbor].insert(idx);
            }
        }
    }

    for (const auto& [key, set] : neighborsSetByIdx)
        neighborsByIdx.insert({key, std::vector(set.begin(), set.end())});
}

void Antop::allocateBaseAddress(const H3Index origin, const H3Index idx, std::queue<H3Index>& cells_queue) {
    if (idx == INVALID_IDX || idx == origin || cellByIdx.contains(idx))
        return;

    const CoordIJK output = getNeighborCoordinates(origin, idx);
    CoordIJK primeOutput = output;

    _ijkRotate60cw(&primeOutput);

    Address newAddr = cellByIdx[origin].addresses[0].copy();
    Address newAddrPrime = cellByIdx[origin].addresses[1].copy();

    newAddr.push(&output);
    newAddrPrime.push(&primeOutput);

    if (!isNewAddrValid(newAddr, idx) || !isNewAddrValid(newAddrPrime, idx)) return;

    addresses.insert_or_assign(newAddr, idx);
    addresses.insert_or_assign(newAddrPrime, idx);

    Cell& cell = cellByIdx[idx];
    cell.addAddress(newAddr);
    cell.addAddress(newAddrPrime);
    cells_queue.push(idx);
}

void Antop::allocateSupplementaryAddresses() {
    for (const auto& [idx, cell1] : cellByIdx) {
        for (std::array<H3Index, MAX_NEIGHBORS> out = getNeighbors(idx); const unsigned long h3 : out) {
            if (h3 == INVALID_IDX || h3 == idx)
                continue;

            Cell& cell2 = cellByIdx[h3];
            if (cell1.distanceTo(&cell2) <= 1)
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

void Antop::allocateBaseAddresses(H3Index idx) {
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

// ToDo: move to tests/antop.cpp.
int Antop::neighbors() {
    int neighborCount = 0;
    for (const auto& [idx1, cell1] : cellByIdx) {
        std::array<H3Index, MAX_NEIGHBORS> neighbors = getNeighbors(idx1);

        for (const auto& [idx2, cell2] : cellByIdx) {
            if (idx1 == idx2)
                continue;

            if (cell1.distanceTo(&cell2) == 1) {
                bool isNeighbor = false;
                for (const auto& neighbor : neighbors) {
                    if (neighbor == idx2) {
                        isNeighbor = true;
                        break;
                    }
                }

                if (!isNeighbor)
                    std::cerr << "Cell " << std::hex << idx1 << " is not a neighbor of cell " << std::hex << idx2 << std::dec << std::endl;
                neighborCount++;
            }
        }
    }
    return neighborCount;
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

    buildNeighborGraph();
}

void Antop::init(const int satellites) {
    const int res = getResolution(satellites);
    allocateAddresses(res);

    std::cout << "Resolution: " << std::dec << res << std::endl;
    std::cout << "Unique Cells: " << std::dec << cellByIdx.size() << std::endl;
    std::cout << "Number of addresses: " << std::dec << addresses.size() << std::endl;
    std::cout << std::dec << "Missing neighbors: " << (CELLS_BY_RESOLUTION.at(res) - 12) * 6 + 60 - neighbors() << std::endl << std::endl;
}

// ToDo:
// - Test heap.
// - Update dst to include information about the packet (such as number of hops thus far).
// - There's a "small" edge case that will break this routing algorithm. Let's say cell A receives a packet and the shortest routes to its destination are through either
//   cell B or cell C. Cell A then forwards it to cell B, but it turns out that cell B has no option but to return the packet to cell A due to missing satellites. Now,
//   cell A holds the same packet but lastHop has been updated to cell B. Naturally, it then forwards it to cell C, but cell C experiences the same shortcoming as cell B.
//   Cell A receives the same packet for a third time, but this time lastHop has been updated to cell C. Therefore, cell A will once again try its luck with cell B, turning
//   this into an endless loop, until the packet is lost due to the number of hops.
H3Index Antop::getNextHopId(const H3Index src, const H3Index dst, const H3Index lastHop, bool isNextHopValid(H3Index idx)) const {
    const std::vector<H3Index> neighbors = neighborsByIdx.at(src);

    std::priority_queue<
        std::pair<int, H3Index>,
        std::vector<std::pair<int, H3Index>>,
        std::greater<>
    > heap;

    for (const H3Index neighbor : neighbors) {
        if (neighbor == src) // I'm trusting that neighborGraph is correctly built. If not, this won't work as intended.
            continue;

        int key = std::numeric_limits<int>::max();
        if (neighbor != lastHop)
            key = cellByIdx.at(neighbor).distanceTo(&cellByIdx.at(dst));

        heap.emplace(key, neighbor);
    }

    H3Index nextHop = INVALID_IDX;
    do {
        if (heap.empty())
            throw Errors::unreachableDestination(src, dst);
        nextHop = heap.top().second;
    } while (!isNextHopValid(nextHop));

    return nextHop;
}
