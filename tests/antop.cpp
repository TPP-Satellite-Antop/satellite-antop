#include <gtest/gtest.h>
#include "antop.h"

TEST(AntopTest, NeighborRouting) {
    Antop antop{};
    antop.init(1);

    ASSERT_EQ(antop.getHopCandidates(0x8081fffffffffff, 0x8057fffffffffff)[0], 0x8057fffffffffff);
}

/*
TEST(AntopTest, Aaa) {
    Antop antop{};
    antop.init(0);

    constexpr H3Index src = 0x8067fffffffffff;
    constexpr H3Index dst = 0x8021fffffffffff;

    std::cout << std::hex << 0x8067fffffffffff << ": " << antop.distance(src, dst) << std::endl;
    for (const auto candidate : antop.getHopCandidates(src, dst)) {
        std::cout << std::hex << candidate << ": " << std::dec << antop.distance(candidate, dst) << std::endl;
    }

    std::cout << std::endl;

    for (const auto candidate : antop.getHopCandidates(0x808ffffffffffff, dst)) {
        std::cout << std::hex << candidate << ": " << std::dec << antop.distance(candidate, dst) << std::endl;
    }

    std::cout << std::endl;

    for (const auto candidate : antop.getHopCandidates(0x8093fffffffffff, dst)) {
        std::cout << std::hex << candidate << ": " << std::dec << antop.distance(candidate, dst) << std::endl;
    }

    std::cout << std::endl;
}*/
