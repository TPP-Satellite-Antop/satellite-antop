#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cmath>
#include <iomanip>

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
        return { Hypercube(cellInfoByRes[Resolution].pentagons[I])... };
    }

public:
    AntopImpl() : hypercubes(buildHypercubes(std::make_index_sequence<pentagonsPerRes>{})) {
        std::unordered_map<H3Index, std::unordered_set<H3Index> > neighborsSetByIdx;
        std::vector<uint8_t> distanceOffsets;

        hypercubeLookup.resize(cells * cells);
        distanceOffsets.resize(cells * cells);
        neighborsByIdx.reserve(cells);
        neighborsSetByIdx.reserve(cells);

        std::fill_n(&distanceOffsets[0], cells * cells, 255);

        for (int i = 0; i < cells; i++) {
            const auto idxA = cellInfoByRes[Resolution].cells[i];
            cellIdByIdx[idxA] = i;

            for (int j = i; j < cells; j++) {
                const auto tableIdx = i * cells + j;
                const auto tableIdxInv = j * cells + i;

                const auto idxB = cellInfoByRes[Resolution].cells[j];
                const auto distanceH3 = h3Distance(idxA, idxB);

                for (int k = 0; k < pentagonsPerRes; k++) {
                    const auto distance = hypercubes[k].distance(idxA, idxB);
                    const auto offset = distance - distanceH3;
                    const auto offsetAbs = std::abs(offset);

                    // Prefer hypercubes that increase routing accuracy or that keep accuracy the same but overestimate distances.
                    if (offsetAbs < distanceOffsets[tableIdx] || (offsetAbs == distanceOffsets[tableIdx] && offset > 0)) {
                        distanceOffsets[tableIdx] = offsetAbs;
                        distanceOffsets[tableIdxInv] = offsetAbs;
                        hypercubeLookup[tableIdx] = static_cast<uint8_t>(k);
                        hypercubeLookup[tableIdxInv] = static_cast<uint8_t>(k);

                        if (offsetAbs == 0 && distanceH3 == 1) {
                            neighborsSetByIdx[idxA].insert(idxB);
                            neighborsSetByIdx[idxB].insert(idxA);
                        }
                    }
                }
            }
        }

        std::vector<double> errors;
        size_t neighbors = 0;
        double mre = 0;
        double median = 0;

        for (const auto &[key, set]: neighborsSetByIdx) {
            neighborsByIdx.insert({key, std::vector(set.begin(), set.end())});
            neighbors += set.size();
        }

        // Accuracy metrics
        for (int i = 0; i < cells; i++) {
            const auto idxA = cellInfoByRes[Resolution].cells[i];

            for (int j = i+1; j < cells; j++) {
                const auto idxB = cellInfoByRes[Resolution].cells[j];
                const auto distance = h3Distance(idxA, idxB);
                const auto error = static_cast<double>(distanceOffsets[i * cells + j]) / static_cast<double>(distance);
                mre += error;
                errors.push_back(error);
            }
        }

        const size_t n = errors.size();
        std::ranges::sort(errors);
        mre /= static_cast<double>(n);

        if (n % 2 == 0) median = 0.5 * (errors[n/2 - 1] + errors[n/2]);
        else median = errors[n/2];

        auto percentile = [&](const double p) {
            const size_t idx = static_cast<size_t>(std::ceil(p * static_cast<double>(n))) - 1;
            return errors[std::min(idx, n - 1)];
        };

        std::cout << "Neighbors - Target: " << (cellsPerRes[Resolution] - 12) * 6 + 12 * 5 << " /// Actual: " << neighbors << std::endl;
        std::cout << "Mean: " << (1 - mre) << std::endl;
        std::cout << "Median: " << median << "" << std::endl;
        std::cout << "P90: " << percentile(0.90)  << "" << std::endl;
        std::cout << "P95: " << percentile(0.95)  << "" << std::endl;
        std::cout << "P99: " << percentile(0.99)  << "" << std::endl;
        std::cout << "P100 " << errors.back()  << "" << std::endl;
    }

    std::vector<H3Index> getHopCandidates(const H3Index src, const H3Index dst) override {
        const auto& hypercube = hypercubes[hypercubeLookup[cellIdByIdx[src] * cells + cellIdByIdx[dst]]];
        std::vector<H3Index> neighbors = neighborsByIdx.at(src);

        std::ranges::sort(neighbors, [&](const H3Index a, const H3Index b) {
            return hypercube.distance(a, dst) < hypercube.distance(b, dst);
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
