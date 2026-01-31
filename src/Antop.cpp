#include <iostream>
#include <ranges>
#include <algorithm>

#include "Antop.h"
#include "Hypercube.h"
#include "resolution.h"

void Antop::init(const int satellites) {
    uint8_t distances[122][122];

    std::fill_n(&distances[0][0], 122 * 122, 255);

    for (int i = 0; i < 122; i++) {
        for (int j = 0; j < 122; j++) {
            const auto idxA = indexes[i];
            const auto idxB = indexes[j];

            const auto distanceH3 = h3Distance(idxA, idxB);
            //std::cout << "[" << std::hex << idxA << "," << idxB << "]: " << std::dec << distanceH3 << std::endl;

            for (const auto& a : aaa) {
                const auto distance = a.cellByIdx.at(idxA).distanceTo(a.cellByIdx.at(idxB));

                distances[i][j] = std::min(distances[i][j], static_cast<uint8_t>(abs(distanceH3 - distance)));
            }
        }
    }





    resolution = findResolution(satellites);
    allocateAddresses(origin);

    std::cout << "Resolution: " << std::dec << resolution << std::endl;
    std::cout << "Unique Cells: " << std::dec << cellByIdx.size() << std::endl;
    std::cout << "Number of addresses: " << std::dec << addresses.size() << std::endl;
    std::cout << std::dec << "Missing neighbors: " << (CELLS_BY_RESOLUTION[resolution] - 12) * 6 + 60 - neighbors() << std::endl << std::endl;
}

