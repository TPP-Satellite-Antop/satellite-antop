#include <array>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Antop.h"
#include "Hypercube.h"
#include "distance.h"

struct Antop::Impl {
    virtual ~Impl() = default;
    virtual std::vector<H3Index>
    getHopCandidates(H3Index src, H3Index dst) = 0;
};

template <int Resolution>
class AntopImpl final : public Antop::Impl {
    static constexpr int cells = cellsPerRes[Resolution];

    uint8_t hypercubeLookup[cells][cells];
    std::array<Hypercube, pentagonsPerRes> hypercubes;
    std::unordered_map<H3Index, std::vector<H3Index>> neighborsByIdx;
    // std::unordered_map<H3Index, Cell> cellByIdx;
    // std::unordered_map<Address, H3Index> addresses;

public:
    AntopImpl() {
        // Initialize hypercubes with each pentagon as a hypercube center
        for (int i = 0; i < pentagonsPerRes; i++) {
            hypercubes[i] = Hypercube{};
            hypercubes[i].init(cellInfoByResolution[Resolution].pentagons[i], Resolution);
        }

        uint8_t distanceOffsets[cells][cells];
        std::fill_n(&distanceOffsets[0][0], cells * cells, 255);

        for (int i = 0; i < cells; i++) {
            for (int j = i; j < cells; j++) {
                const auto idxA = cellInfoByResolution[Resolution].cells[i];
                const auto idxB = cellInfoByResolution[Resolution].cells[j];

                const auto distanceH3 = h3Distance(idxA, idxB);

                for (int k = 0; k < pentagonsPerRes; k++) {
                    const auto distance = hypercubes[k].cellByIdx.at(idxA).distanceTo(hypercubes[k].cellByIdx.at(idxB));
                    const auto offset = abs(distanceH3 - distance);

                    if (offset < distanceOffsets[i][j]) {
                        distanceOffsets[i][j] = offset;
                        distanceOffsets[j][i] = offset;
                        hypercubeLookup[i][j] = k;
                        hypercubeLookup[j][i] = k;
                    }


                    distanceOffsets[i][j] = std::min(distanceOffsets[i][j], static_cast<uint8_t>());
                    distanceOffsets[j][i] = distanceOffsets[i][j];
                }
            }
        }
    }

    std::vector<H3Index> getHopCandidates(const H3Index src, const H3Index dst) override {
        // ToDo
        return {};
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
