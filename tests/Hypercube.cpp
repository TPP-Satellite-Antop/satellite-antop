#include <gtest/gtest.h>
#include "Hypercube.h"

#include "util.h"

extern "C" {
#include "localij.h"
#include "coordijk.h"
}

#include <iomanip>

TEST(AntopTest, NeighborRouting) {
    Hypercube antop{};
    antop.init(0x8047fffffffffff, 1);

    auto candidates = antop.getHopCandidates(0x80bffffffffffff, 0x800ffffffffffff);

    ASSERT_EQ(candidates.size(), 6);
    std::vector<H3Index> exp = {
        0x80dbfffffffffff,
        0x80bbfffffffffff,
        0x809dfffffffffff,
        0x80b9fffffffffff,
        0x809ffffffffffff,
        0x80d9fffffffffff,
    };

    ASSERT_EQ(candidates, exp);
}

std::array<H3Index, 12> pentagons{
    0x8091fffffffffff, 0x807ffffffffffff, 0x80a7fffffffffff, 0x80d7fffffffffff, 0x80c3fffffffffff, 0x8031fffffffffff,
    0x8063fffffffffff, 0x804dfffffffffff, 0x8075fffffffffff, 0x80ebfffffffffff, 0x801dfffffffffff, 0x8009fffffffffff
};

std::array<H3Index, 122> indexes{
    0x8063fffffffffff,
    0x8061fffffffffff,
    0x8085fffffffffff,
    0x8043fffffffffff,
    0x807bfffffffffff,
    0x8053fffffffffff,
    0x80abfffffffffff,
    0x8021fffffffffff,
    0x8087fffffffffff,
    0x803dfffffffffff,
    0x8065fffffffffff,
    0x8097fffffffffff,
    0x80a3fffffffffff,
    0x802dfffffffffff,
    0x806bfffffffffff,
    0x803ffffffffffff,
    0x80cbfffffffffff,
    0x80cdfffffffffff,
    0x80affffffffffff,
    0x8025fffffffffff,
    0x8011fffffffffff,
    0x800bfffffffffff,
    0x808dfffffffffff,
    0x8041fffffffffff,
    0x8069fffffffffff,
    0x8083fffffffffff,
    0x80adfffffffffff,
    0x80bdfffffffffff,
    0x801ffffffffffff,
    0x8059fffffffffff,
    0x8039fffffffffff,
    0x80d7fffffffffff,
    0x80e1fffffffffff,
    0x8009fffffffffff,
    0x80e5fffffffffff,
    0x80a7fffffffffff,
    0x80c9fffffffffff,
    0x8031fffffffffff,
    0x8015fffffffffff,
    0x8001fffffffffff,
    0x8005fffffffffff,
    0x8095fffffffffff,
    0x8073fffffffffff,
    0x804bfffffffffff,
    0x8075fffffffffff,
    0x8099fffffffffff,
    0x80c1fffffffffff,
    0x80d1fffffffffff,
    0x8019fffffffffff,
    0x8055fffffffffff,
    0x8035fffffffffff,
    0x80e7fffffffffff,
    0x80f1fffffffffff,
    0x8007fffffffffff,
    0x80edfffffffffff,
    0x80d9fffffffffff,
    0x80b9fffffffffff,
    0x802ffffffffffff,
    0x8017fffffffffff,
    0x8003fffffffffff,
    0x800dfffffffffff,
    0x809dfffffffffff,
    0x8077fffffffffff,
    0x80c7fffffffffff,
    0x8051fffffffffff,
    0x80bbfffffffffff,
    0x8045fffffffffff,
    0x8047fffffffffff,
    0x8029fffffffffff,
    0x809ffffffffffff,
    0x8027fffffffffff,
    0x80d5fffffffffff,
    0x805ffffffffffff,
    0x809bfffffffffff,
    0x8037fffffffffff,
    0x8071fffffffffff,
    0x80cffffffffffff,
    0x80d3fffffffffff,
    0x805dfffffffffff,
    0x8049fffffffffff,
    0x80bffffffffffff,
    0x8091fffffffffff,
    0x801bfffffffffff,
    0x8093fffffffffff,
    0x801dfffffffffff,
    0x80b3fffffffffff,
    0x8079fffffffffff,
    0x80effffffffffff,
    0x8067fffffffffff,
    0x80ddfffffffffff,
    0x80a1fffffffffff,
    0x802bfffffffffff,
    0x806dfffffffffff,
    0x80e3fffffffffff,
    0x806ffffffffffff,
    0x8089fffffffffff,
    0x8013fffffffffff,
    0x80b1fffffffffff,
    0x803bfffffffffff,
    0x80b7fffffffffff,
    0x808ffffffffffff,
    0x808bfffffffffff,
    0x80b5fffffffffff,
    0x80e9fffffffffff,
    0x804dfffffffffff,
    0x80c3fffffffffff,
    0x80dffffffffffff,
    0x80a9fffffffffff,
    0x8033fffffffffff,
    0x8081fffffffffff,
    0x805bfffffffffff,
    0x807ffffffffffff,
    0x8023fffffffffff,
    0x80dbfffffffffff,
    0x80ebfffffffffff,
    0x800ffffffffffff,
    0x80f3fffffffffff,
    0x807dfffffffffff,
    0x8057fffffffffff,
    0x80c5fffffffffff,
    0x804ffffffffffff,
    0x80a5fffffffffff,
};

void printDistances(const uint8_t distances[122][122]) {
    constexpr int N = 122;
    constexpr int width = 3;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << std::setw(width)
                      << static_cast<int>(distances[i][j])
                      << ' ';
        }
        std::cout << '\n';
    }
}

TEST(AntopTest, Aaa) {
    std::array<Hypercube, 12> aaa{};

    for (int i = 0; i < 12; i++) {
        aaa[i] = Hypercube{};
        aaa[i].init(pentagons.at(i), 1);
    }

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

    printDistances(distances);
}
