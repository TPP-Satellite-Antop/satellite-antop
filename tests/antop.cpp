#include <gtest/gtest.h>
#include "antop.h"

TEST(AntopTest, NeighborRouting) {
    Antop antop{};
    antop.init(1);

    ASSERT_EQ(antop.getHopCandidates(0x8081fffffffffff, 0x8057fffffffffff, 0), 0x8057fffffffffff);
}

TEST(AntopTest, BestAvailablePathRouting) {
    Antop antop{};
    antop.init(1);

    ASSERT_EQ(antop.getHopCandidates(0x8081fffffffffff, 0x8035fffffffffff, 0), 0x808bfffffffffff);
}
