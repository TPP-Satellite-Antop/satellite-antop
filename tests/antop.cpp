#include <gtest/gtest.h>
#include "antop.h"

TEST(AntopTest, NeighborRouting) {
    Antop antop{};
    antop.init(1);

    ASSERT_EQ(antop.getNextHopId(0x8081fffffffffff, 0x8057fffffffffff, 0, [](H3Index) { return true; }), 0x8057fffffffffff);
}

TEST(AntopTest, BestAvailablePathRouting) {
    Antop antop{};
    antop.init(1);

    ASSERT_EQ(antop.getNextHopId(0x8081fffffffffff, 0x8035fffffffffff, 0, [](const H3Index idx) { return idx == 0x808bfffffffffff; }), 0x808bfffffffffff);
}
