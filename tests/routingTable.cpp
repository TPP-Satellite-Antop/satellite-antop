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

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance, 1);
    curDistance = 10;
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8065fffffffffff, &curDistance, 1);

    ASSERT_EQ(nextHop, 0x8065fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, FindNewNeighborRotatesCandidates) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1.0), 0x8069fffffffffff);
}
