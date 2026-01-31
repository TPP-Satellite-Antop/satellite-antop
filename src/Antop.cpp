#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

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

    CellID hypercubeLookup[cells][cells]{};
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
        neighborsByIdx.reserve(cells);
        neighborsSetByIdx.reserve(cells);

        uint8_t distanceOffsets[cells][cells];
        std::fill_n(&distanceOffsets[0][0], cells * cells, 255);

        for (int i = 0; i < cells; i++) {
            const auto idxA = cellInfoByRes[Resolution].cells[i];
            cellIdByIdx[idxA] = i;

            for (int j = i; j < cells; j++) {
                const auto idxB = cellInfoByRes[Resolution].cells[j];
                const auto distanceH3 = h3Distance(idxA, idxB);

                for (int k = 0; k < pentagonsPerRes; k++) {
                    const auto distance = hypercubes[k].distance(idxA, idxB);
                    const auto offset = std::abs(distanceH3 - distance);

                    if (offset < distanceOffsets[i][j]) {
                        distanceOffsets[i][j] = offset;
                        distanceOffsets[j][i] = offset;
                        hypercubeLookup[i][j] = static_cast<uint8_t>(k);
                        hypercubeLookup[j][i] = static_cast<uint8_t>(k);

                        // ToDo: validate
                        if (offset == 0 && distanceH3 == 1) {
                            neighborsSetByIdx[idxA].insert(idxB);
                            neighborsSetByIdx[idxB].insert(idxA);
                        }
                    }
                }
            }
        }

        for (const auto &[key, set]: neighborsSetByIdx)
            neighborsByIdx.insert({key, std::vector(set.begin(), set.end())});
    }

    std::vector<H3Index> getHopCandidates(const H3Index src, const H3Index dst) override {
        const auto& hypercube = hypercubes[hypercubeLookup[cellIdByIdx[src]][cellIdByIdx[dst]]];
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
