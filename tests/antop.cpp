#include <gtest/gtest.h>
#include "antop.h"

void bbb(Antop* antop, const H3Index idx1, const H3Index idx2) {
    std::cout << std::hex << idx1 << " to " << idx2 << ": " << std::dec << antop->distance(idx1, idx2) << std::endl;
}

TEST(AntopTest, NeighborRouting) {
    Antop antop{};
    antop.init(1);

    bbb(&antop, 0x8015fffffffffff, 0x8061fffffffffff);
    bbb(&antop, 0x8005fffffffffff, 0x8061fffffffffff);
    bbb(&antop, 0x8031fffffffffff, 0x8061fffffffffff);
    bbb(&antop, 0x8041fffffffffff, 0x8061fffffffffff);
    bbb(&antop, 0x8061fffffffffff, 0x8031fffffffffff);
    bbb(&antop, 0x8061fffffffffff, 0x8041fffffffffff);

    ASSERT_EQ(antop.getHopCandidates(0x8081fffffffffff, 0x8057fffffffffff, 0)[0], 0x8057fffffffffff);
}
