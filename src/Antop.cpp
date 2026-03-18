#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <omp.h>
#include <sstream>

#include "Antop.h"
#include "Hypercube.h"
#include "distance.h"

struct Antop::Impl {
    virtual ~Impl() = default;
    virtual std::vector<H3Index> getHopCandidates(H3Index src, H3Index dst) = 0;
    virtual int getResolution() = 0;
};

template <int Resolution>
class AntopImpl final : public Antop::Impl {
    using CellID = uint8_t;

    static constexpr int cells = cellsPerRes[Resolution];

    std::vector<CellID> hypercubeLookup; // Size = cells * cells
    std::array<Hypercube, pentagonsPerRes> hypercubes;
    std::unordered_map<H3Index, std::vector<H3Index>> neighborsByIdx;
    std::unordered_map<H3Index, CellID> cellIdByIdx;

    template<std::size_t... I>
    static std::array<Hypercube, pentagonsPerRes> buildHypercubes(std::index_sequence<I...>) {
        // ToDo: add concurrency / parallelism.
        return { Hypercube(cellInfoByRes[Resolution].pentagons[I])... };
    }

public:
    AntopImpl() : hypercubes(buildHypercubes(std::make_index_sequence<pentagonsPerRes>{})) {
        std::unordered_map<H3Index, std::unordered_set<H3Index>> neighborsSetByIdx;
        std::vector<uint8_t> distanceOffsets;
        std::vector<uint16_t> h3Distances;

        hypercubeLookup.resize(cells * cells);
        distanceOffsets.resize(cells * cells);
        h3Distances.resize(cells * cells);
        neighborsByIdx.reserve(cells);
        neighborsSetByIdx.reserve(cells);

        std::fill_n(distanceOffsets.begin(), cells * cells, 255);

        // 🔒 Build this BEFORE parallel region (unordered_map is NOT thread-safe)
        for (int i = 0; i < cells; i++) {
            const auto idx = cellInfoByRes[Resolution].cells[i];
            cellIdByIdx[idx] = i;
        }

        std::string filename = "distance_errors_res" + std::to_string(Resolution) + ".csv";
        std::ofstream file(filename);
        file << "distance_h3,absolute_error,relative_error\n";

        #pragma omp parallel
        {
            std::stringstream local_buffer;
            std::unordered_map<H3Index, std::unordered_set<H3Index>> local_neighbors;

            #pragma omp for schedule(dynamic, 8)
            for (int i = 0; i < cells; i++) {
                const auto idxA = cellInfoByRes[Resolution].cells[i];

                for (int j = i + 1; j < cells; j++) {
                    const auto tableIdx = i * cells + j;
                    const auto tableIdxInv = j * cells + i;

                    const auto idxB = cellInfoByRes[Resolution].cells[j];
                    const auto distanceH3 = h3Distance(idxA, idxB);

                    h3Distances[tableIdx] = distanceH3;
                    h3Distances[tableIdxInv] = distanceH3;

                    int bestOffsetAbs = 255;
                    int bestOffset = 0;
                    uint8_t bestK = 0;

                    for (int k = 0; k < pentagonsPerRes; k++) {
                        const auto distance = hypercubes[k].distance(idxA, idxB);
                        const int offset = distance - distanceH3;
                        const int offsetAbs = std::abs(offset);

                        if (offsetAbs < bestOffsetAbs || (offsetAbs == bestOffsetAbs && offset > 0)) {
                            bestOffsetAbs = offsetAbs;
                            bestOffset = offset;
                            bestK = static_cast<uint8_t>(k);

                            if (offsetAbs == 0 && distanceH3 == 1) {
                                local_neighbors[idxA].insert(idxB);
                                local_neighbors[idxB].insert(idxA);
                            }
                        }
                    }

                    distanceOffsets[tableIdx] = bestOffsetAbs;
                    distanceOffsets[tableIdxInv] = bestOffsetAbs;
                    hypercubeLookup[tableIdx] = bestK;
                    hypercubeLookup[tableIdxInv] = bestK;

                    const double relativeError =
                        distanceH3 == 0 ? 0.0 :
                        static_cast<double>(std::abs(bestOffset)) /
                        static_cast<double>(distanceH3);

                    local_buffer << distanceH3 << ","
                                 << bestOffset << ","
                                 << relativeError << "\n";
                }

                if (i % 100 == 0) {
                    #pragma omp critical
                    std::cout << "Processed " << i << " cells" << std::endl;
                }
            }

            // 🔒 Merge results (minimal critical section)
            #pragma omp critical
            {
                file << local_buffer.str();

                for (auto &[k, v] : local_neighbors) {
                    auto &globalSet = neighborsSetByIdx[k];
                    globalSet.insert(v.begin(), v.end());
                }
            }
        }

        // Convert sets → vectors
        for (const auto &[key, set] : neighborsSetByIdx) {
            neighborsByIdx.insert({key, std::vector(set.begin(), set.end())});
        }

        file.close();
    }

    std::vector<H3Index> getHopCandidates(const H3Index src, const H3Index dst) override {
        std::vector<H3Index> neighbors = neighborsByIdx.at(src);

        std::ranges::sort(neighbors, [&](const H3Index a, const H3Index b) {
            const auto& hypercubeA = hypercubes[hypercubeLookup[cellIdByIdx[a] * cells + cellIdByIdx[dst]]];
            const auto& hypercubeB = hypercubes[hypercubeLookup[cellIdByIdx[b] * cells + cellIdByIdx[dst]]];

            return hypercubeA.distance(a, dst) < hypercubeB.distance(b, dst);
        });

        return neighbors;
    }

    int getResolution() override {
        return Resolution;
    }
};

Antop::Antop(const int satellites) {
    switch (findResolution(satellites)) {
        case 0:
            impl = std::make_unique<AntopImpl<0>>();
            break;
        case 1:
            impl = std::make_unique<AntopImpl<1>>();
            break;
        case 2:
            impl = std::make_unique<AntopImpl<2>>();
            break;
        default:
            throw std::logic_error("Unsupported resolution");
    }
}

Antop::~Antop() = default;

std::vector<H3Index> Antop::getHopCandidates(const H3Index src, const H3Index dst) const {
    return impl->getHopCandidates(src, dst);
}

int Antop::getResolution() const {
    return impl->getResolution();
}
