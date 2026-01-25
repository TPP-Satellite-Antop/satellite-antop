#include <gtest/gtest.h>
#include "routingTable.h"
#include "antop.h"

TEST(RoutingTableTest, GetResolution) {
    Antop antop{};
    antop.init(1);
    const RoutingTable routingTable(&antop);

    ASSERT_EQ(routingTable.getAntopResolution(), antop.getResolution());
}

TEST(RoutingTableTest, ExpirationResetsTables) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    const auto cur = 0x8041fffffffffff;
    const auto src = 0x8025fffffffffff;
    const auto dst = 0x8069fffffffffff;
    const auto sender = src;
    auto curDistance = 1;

    const auto next = routingTable.findNextHop(cur, src, dst, sender, &curDistance, 1.0);
    const auto cached = routingTable.findNewNeighbor(cur, dst, sender, 1.0);
    const auto afterClear = routingTable.findNewNeighbor(cur, dst, sender, 2.0);

    ASSERT_TRUE(next != cached);
    ASSERT_EQ(next, afterClear);
}

TEST(RoutingTableTest, SimpleUncachedHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, 1);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, SimpleCachedHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, 1);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, 1);

    ASSERT_EQ(nextHop1, nextHop2);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, SimpleCachedReturnHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    auto curDistance = 1;
    routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, 1);
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance, 1);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, ReturnToSenderUponLoopDetection) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8069fffffffffff, &curDistance, 1);
    curDistance = 10;
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistance, 1);

    ASSERT_EQ(nextHop, 0x8065fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, FindNewNeighborRotatesCandidates) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8069fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x8069fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8041fffffffffff);
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1.0), 0x8041fffffffffff);
}

TEST(RoutingTableTest, PreviousUpdateBundleDoesNotCreateInvalidCacheRecord) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 2;

    // Bundle is initially routed to find if any paths are currently available.
    ASSERT_EQ(routingTable.findNextHop(0x8041fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 10), 0x8025fffffffffff);
    // While the bundle awaited for its turn to be transmitted, a mobility update occurred.
    ASSERT_EQ(routingTable.findNextHop(0x8031fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 20), 0x8025fffffffffff);
    // Inverse route does not suggest routing through a now invalid route.
    ASSERT_NE(routingTable.findNextHop(0x8031fffffffffff, 0x8025fffffffffff, 0x8095fffffffffff, 0x8025fffffffffff, &curDistance, 20), 0x8069fffffffffff);
}

TEST(RoutingTableTest, BundleSpreeGetRoutedToSameNextHopUponLoopDetection) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;
    auto curDistance1 = 10;
    auto curDistance2 = 10;

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8069fffffffffff, &curDistance, 1);
    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistance1, 1);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x800bfffffffffff, 0x8065fffffffffff, &curDistance2, 1);

    ASSERT_EQ(curDistance1, 1);
    ASSERT_EQ(curDistance2, 1);
    ASSERT_EQ(nextHop1, nextHop2);
}
/*
TEST(RoutingTableTest, CacheGetsUpdatedUponLoopDetection) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 2;
    auto curDistanceLoop = 5;

    routingTable.findNextHop(0x8069fffffffffff, 0x80b9fffffffffff, 0x800bfffffffffff, 0x8095fffffffffff, &curDistance, 1);
    // Loop 1
    const auto nextHop1 = routingTable.findNextHop(0x8069fffffffffff, 0x80b9fffffffffff, 0x800bfffffffffff, 0x804bfffffffffff, &curDistanceLoop, 1);
    // Non-loop bundle (same route as the original one)
    const auto nextHop2 = routingTable.findNextHop(0x8069fffffffffff, 0x80b9fffffffffff, 0x800bfffffffffff, 0x8095fffffffffff, &curDistance, 1);

    // After the loop detection, both bundles should be routed to the same address, which must be different from the already tried ones.
    ASSERT_EQ(nextHop1, nextHop2);
    ASSERT_NE(nextHop1, 0x804bfffffffffff);
    ASSERT_NE(nextHop2, 0x804bffffffffff);
    ASSERT_NE(nextHop1, 0x8041fffffffffff);
    ASSERT_NE(nextHop2, 0x8041fffffffffff);
}*/
