#include <gtest/gtest.h>
#include "antop.h"

TEST(AntopTest, NeighborRouting) {
    Antop antop{};
    antop.init(1);

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
