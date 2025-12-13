#include <gtest/gtest.h>
#include "routingTable.h"
#include "antop.h"

TEST(RoutingTableTest, GetResolution) {
    Antop antop{};
    antop.init(1);
    const RoutingTable routingTable(&antop);

    ASSERT_EQ(routingTable.getAntopResolution(), antop.getResolution());
}

TEST(RoutingTableTest, SimpleUncachedHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, SimpleCachedHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance);

    ASSERT_EQ(nextHop1, nextHop2);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, SimpleCachedReturnHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    auto curDistance = 1;
    routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance);
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, &curDistance);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
    ASSERT_EQ(curDistance, 1);
}

TEST(RoutingTableTest, ReturnToSenderUponLoopDetection) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);
    auto curDistance = 1;

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, &curDistance);
    curDistance = 10;
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8065fffffffffff, &curDistance);

    ASSERT_EQ(nextHop, 0x8065fffffffffff);
    ASSERT_EQ(curDistance, 0);
}

TEST(RoutingTableTest, FindNewNeighborRotatesCandidates) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x8025fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x803dfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x8031fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x8065fffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x804bfffffffffff);
    ASSERT_EQ(routingTable.findNewNeighbor(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff), 0x8069fffffffffff);
}
