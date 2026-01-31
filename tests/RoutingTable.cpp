#include <gtest/gtest.h>

#include "Antop.h"
#include "RoutingTable.h"

class RoutingTableTestWithAntop : public testing::Test {
protected:
    static Antop* antop;
    
    static void SetUpTestSuite() {
        antop = new Antop(100);
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
    RoutingTable routingTable(antop);

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
    RoutingTable routingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST_F(RoutingTableTestWithAntop, SimpleCachedHop) {
    RoutingTable routingTable(antop);
    auto curDistance = 1;
    auto loopEpoch = 0;

    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, &loopEpoch, 1);

    ASSERT_EQ(nextHop1, nextHop2);
    ASSERT_EQ(curDistance, 1);
}

TEST_F(RoutingTableTestWithAntop, SimpleCachedReturnHop) {
    RoutingTable routingTable(antop);
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
    RoutingTable routingTable(antop);
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
    RoutingTable routingTable(antop);
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
    RoutingTable routingTable(antop);
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
    RoutingTable routingTable(antop);
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
    RoutingTable routingTable(antop);
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

TEST_F(RoutingTableTestWithAntop, Aaa) {
    RoutingTable routingTableA(antop);
    RoutingTable routingTableB(antop);
    RoutingTable routingTableC(antop);
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
