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

    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 1, 1.0);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
}

TEST(RoutingTableTest, SimpleCachedHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    const auto nextHop1 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 1, 1.0);
    const auto nextHop2 = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 1, 1.0);

    ASSERT_EQ(nextHop1, nextHop2);
}

TEST(RoutingTableTest, SimpleCachedReturnHop) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 1, 1.0);
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 1, 1.0);

    ASSERT_EQ(nextHop, 0x8069fffffffffff);
}

TEST(RoutingTableTest, ReturnToSenderUponLootDetection) {
    Antop antop{};
    antop.init(1);
    RoutingTable routingTable(&antop);

    routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8069fffffffffff, 1, 1.0);
    const auto nextHop = routingTable.findNextHop(0x8041fffffffffff, 0x8069fffffffffff, 0x8025fffffffffff, 0x8065fffffffffff, 10, 1.0);

    ASSERT_EQ(nextHop, 0x8065fffffffffff);
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
