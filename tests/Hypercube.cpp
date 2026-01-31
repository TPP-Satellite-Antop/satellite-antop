#include <gtest/gtest.h>
#include "Hypercube.h"

#include "distance.h"

extern "C" {
#include "localij.h"
#include "coordijk.h"
}

#include <iomanip>
/*
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
*/
/*
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
}*/
