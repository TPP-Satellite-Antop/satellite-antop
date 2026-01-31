#include <gtest/gtest.h>

#include "Antop.h"
#include "RoutingTable.h"

class RoutingTableTestWithAntop : public testing::Test {
public:
    static Antop* antop;
    
    static void SetUpTestSuite() {
        if (antop == nullptr) antop = new Antop(100);
    }

    static void TearDownTestSuite() {
        delete antop;
        antop = nullptr;
    }
};

Antop* RoutingTableTestWithAntop::antop = nullptr;

TEST_F(RoutingTableTestWithAntop, GetResolution) {
    const RoutingTable routingTable(antop);

    ASSERT_EQ(routingTable.getAntopResolution(), antop->getResolution());
}

TEST_F(RoutingTableTestWithAntop, ExpirationResetsTables) {
    auto routingTable = RoutingTable(antop);

    constexpr auto cur = 0x8041fffffffffff;
    constexpr auto src = 0x8025fffffffffff;
    constexpr auto dst = 0x8069fffffffffff;
    constexpr auto sender = src;
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto next = routingTable.findNextHop(cur, src, dst, sender, &curDistance, &loopEpoch, 1.0);
    const auto cached = routingTable.findNewNeighbor(cur, dst, sender, 1.0);
    const auto afterClear = routingTable.findNewNeighbor(cur, dst, sender, 2.0);

    ASSERT_TRUE(next != cached);
    ASSERT_EQ(next, afterClear);
}

TEST_F(RoutingTableTestWithAntop, SimpleUncachedHop) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST_F(RoutingTableTestWithAntop, DistanceDiscrepancyDoesNotGreatlyAffectRouting) {
    std::unordered_map<H3Index, RoutingTable> routingTables{};
    auto curDistance = 1;
    auto loopEpoch = 0;
    H3Index cur = 0x808dfffffffffff;
    H3Index sender = 0x8025fffffffffff;
    constexpr H3Index dst = 0x8033fffffffffff;

    // Although arbitrary, 10 hops should be more than enough to go from 8087fffffffffff to 8033fffffffffff (optimal #hops = 5)
    while (cur != dst && curDistance < 10) {
        auto [it, _] = routingTables.try_emplace(cur, antop);
        const auto nextHop = it->second.findNextHop(cur, 0x8087fffffffffff, dst, sender, &curDistance, &loopEpoch, 1);
        sender = cur;
        cur = nextHop;
        curDistance++;
    }

    ASSERT_TRUE(curDistance < 10);
}

TEST_F(RoutingTableTestWithAntop, SimpleCachedHop) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop1, nextHop2);
    ASSERT_EQ(curDistance, 1);
}

TEST_F(RoutingTableTestWithAntop, SimpleCachedReturnHop) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST_F(RoutingTableTestWithAntop, FindNewNeighborRotatesCandidates) {
    RoutingTable routingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8069fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x8069fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8041fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1.0), 0x8041fffffffffff);
}

TEST_F(RoutingTableTestWithAntop, PreviousUpdateBundleDoesNotCreateInvalidCacheRecord) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 2;
    auto loopEpoch = 0;

    // Bundle is initially routed to find if any paths are currently available.
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 10), 0x8025fffffffffff);
    // While the bundle awaited for its turn to be transmitted, a mobility update occurred.
    ASSERT_EQ(routingTable.findNextHop(0x8031fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 20), 0x8025fffffffffff);
    // Inverse route does not suggest routing through a now invalid route.
    ASSERT_NE(routingTable.findNextHop(0x8031fffffffffff, 0x8025fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 20), 0x8069fffffffffff);
}

TEST_F(RoutingTableTestWithAntop, NewNeighborSearchIsTriggeredUponLoopDetection) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8069fffffffffff, &curDistance, &loopEpoch, 1);
    curDistance = 10;
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop, 0x8031fffffffffff);
    ASSERT_EQ(curDistance, 1);
    ASSERT_EQ(loopEpoch, 1);
}

TEST_F(RoutingTableTestWithAntop, NewNeighborSearchIsNotTriggeredUponResolvedLoopDetection) {
    auto routingTable = RoutingTable(antop);
    auto curDistanceA = 1;
    auto curDistanceB = 1;
    auto loopEpochA = 0;
    auto loopEpochB = 0;

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8069fffffffffff, &curDistanceA, &loopEpochA, 1);
    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8069fffffffffff, &curDistanceB, &loopEpochB, 1);
    curDistanceA = 10;
    curDistanceB = 10;
    const auto nextHopA = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistanceA, &loopEpochA, 1);
    const auto nextHopB = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistanceB, &loopEpochB, 1);

    ASSERT_EQ(nextHopA, nextHopB);
    ASSERT_EQ(curDistanceA, 1);
    ASSERT_EQ(curDistanceB, 1);
    ASSERT_EQ(loopEpochA, 1);
    ASSERT_EQ(loopEpochB, 1);
}

TEST_F(RoutingTableTestWithAntop, LoopResolvedRemotelyDoesNotTriggerNewNeighborSearch) {
    auto routingTable = RoutingTable(antop);
    auto curDistanceA = 1;
    auto curDistanceB = 1;
    auto loopEpochA = 0;
    auto loopEpochB = 0;

    routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistanceA, &loopEpochA, 1);
    routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistanceB, &loopEpochB, 1);
    curDistanceA = 10;
    curDistanceB = 10;
    // Loop resolved remotely.
    loopEpochA++;
    loopEpochB++;
    const auto nextHopA = routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistanceA, &loopEpochA, 1);
    const auto nextHopB = routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistanceB, &loopEpochB, 1);

    ASSERT_EQ(nextHopA, nextHopB);
    ASSERT_EQ(curDistanceA, curDistanceB);
    ASSERT_EQ(loopEpochA, loopEpochB);
}

TEST_F(RoutingTableTestWithAntop, TwoLoopsTriggerTwoNewNeighborSearches) {
    auto routingTable = RoutingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto nextHopA = routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistance, &loopEpoch, 1);
    curDistance = 5;
    const auto nextHopB = routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistance, &loopEpoch, 1);
    curDistance++;
    const auto nextHopC = routingTable.findNextHop(0x8045fffffffffff, 0x80a5fffffffffff, 0x8095fffffffffff, 0x8067fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_NE(nextHopA, nextHopB);
    ASSERT_NE(nextHopA, nextHopC);
    ASSERT_NE(nextHopB, nextHopC);
}

TEST_F(RoutingTableTestWithAntop, LoopDetectionIsTriggeredWithSourceNodeDown) {
    auto routingTableA = RoutingTable(antop);
    auto routingTableB = RoutingTable(antop);
    auto routingTableC = RoutingTable(antop);
    auto curDistance = 5;
    auto loopEpoch = 0;

    const auto nextHopA = routingTableA.findNextHop(0x80d9fffffffffff, 0, 0x802dfffffffffff, 0x8095fffffffffff, &curDistance, &loopEpoch, 1);
    curDistance++;
    routingTableB.findNextHop(0x80bffffffffffff, 0, 0x802dfffffffffff, 0x80d9fffffffffff, &curDistance, &loopEpoch, 1);
    curDistance++;
    routingTableC.findNextHop(0x80dbfffffffffff, 0, 0x802dfffffffffff, 0x80bffffffffffff, &curDistance, &loopEpoch, 1);
    curDistance++;
    const auto nextHopB = routingTableA.findNextHop(0x80d9fffffffffff, 0, 0x802dfffffffffff, 0x80dbfffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_NE(nextHopA, nextHopB);
}
