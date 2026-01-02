#include <gtest/gtest.h>
#include "address.h"

extern "C" {
    #include "algos.h"
    #include "coordijk.h"
}

#include <vector>
#include <tuple>
#include <h3api.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

Direction coordIJKToDirection(const CoordIJK& d) {
    CoordIJK n = d;

    for (int dir = 0; dir <= 6; ++dir) {
        if (n.i == UNIT_VECS[dir].i &&
            n.j == UNIT_VECS[dir].j &&
            n.k == UNIT_VECS[dir].k) {
            return static_cast<Direction>(dir);
        }
    }

    throw std::runtime_error("CoordIJK does not map to a unit H3 direction");
}

std::vector<H3Index> traverseIJK(
    H3Index origin,
    const std::vector<CoordIJK>& path
) {
    std::vector<H3Index> result;
    H3Index current = origin;

    result.push_back(current);

    for (const auto p : path) {
        H3Index next;
        int rotations = 0;
        h3NeighborRotations(current, coordIJKToDirection(p), &rotations, &next);
        current = next;
        result.push_back(current);
    }

    return result;
}

TEST(AddressTest, DeepCopyAddress) {
    constexpr CoordIJK coord{1, 1, 0};

    Address address1(false);
    address1.push(&coord); // 00000110

    Address address2 = address1.copy();
    address2.push(&coord); // 00110110

    ASSERT_NE(address1.data(), address2.data());
    ASSERT_EQ(address1.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
    ASSERT_EQ(address2.data(), std::vector<uint8_t>{IJ_AXES_DIGIT + (IJ_AXES_DIGIT << 3)});
}


TEST(AddressTest, IterateIjkAddressReachesIdx) {
    H3Index origin = 0x8047fffffffffff;
    // current = 0x080dbfffffffffff;
    std::vector<H3Index> neighbors = {
        0x80BFFFFFFFFFFFF,
        0x80BBFFFFFFFFFFF,
        0x80D5FFFFFFFFFFF,
        0x80EBFFFFFFFFFFF,
        0x80EDFFFFFFFFFFF,
        0x80D9FFFFFFFFFFF
    };

    std::vector path = {
        CoordIJK(0,1,1),
        CoordIJK(0,0,1),
        CoordIJK(1,0,0),
        CoordIJK(1,0,0),
        CoordIJK(1,0,0),
        CoordIJK(0,1,0),
    };

    Address address(false);
    for (const auto p : path) {
        address.push(&p);
    }

    auto hexes = traverseIJK(origin, path);
    ASSERT_TRUE(
        std::ranges::any_of(neighbors, [&](H3Index n) {
            return std::ranges::find(hexes, n) != hexes.end();
        })
    );
}

TEST(AddressTest, PushSomeCoordToEmptyAddress) {
    Address address(false);
    constexpr CoordIJK coord1{1, 1, 0};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    const auto exp = std::vector<uint8_t>{IJ_AXES_DIGIT + (J_AXES_DIGIT << 3), IJ_AXES_DIGIT};

    ASSERT_EQ(address.data(), exp);
}

TEST(AddressTest, EmptyHardEqualOperator) {
    ASSERT_TRUE(Address(false) == Address(false));
}

TEST(AddressTest, HardEqualOperator) {
    Address address(false);
    constexpr CoordIJK coord1{1, 1, 0};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    ASSERT_TRUE(address == address);
}

TEST(AddressTest, SoftEqualOperator) {
    Address address(false);
    constexpr CoordIJK coord{0, 0, 0};
    address.push(&coord);

    ASSERT_TRUE(Address(false) == address);
    ASSERT_TRUE(address == Address(false));
}

TEST(AddressTest, NotEqualOperatorOne) {
    Address address(false);
    constexpr CoordIJK coord{0, 1, 1};
    address.push(&coord);

    ASSERT_FALSE(Address(false) == address);
    ASSERT_FALSE(address == Address(false));
}

TEST(AddressTest, NotEqualOperatorTwo) {
    Address address1(false);
    Address address2(false);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};
    address1.push(&coord1);
    address2.push(&coord1);
    address2.push(&coord2);
    address2.push(&coord3);

    ASSERT_FALSE(address1 == address2);
    ASSERT_FALSE(address2 == address1);
}

TEST(AddressTest, ZeroDistanceTo) {
    Address address(false);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};
    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    ASSERT_EQ(address.distanceTo(address), 0);
}

TEST(AddressTest, DiffDimensionDistanceTo) {
    Address address1(false);
    Address address2(true);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};
    address1.push(&coord1);
    address1.push(&coord2);
    address1.push(&coord3);
    address2.push(&coord1);
    address2.push(&coord2);
    address2.push(&coord3);

    ASSERT_EQ(address1.distanceTo(address2), std::numeric_limits<int>::max());
}

TEST(AddressTest, DistanceTo) {
    Address address1(false);
    Address address2(false);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};
    address1.push(&coord1);
    address1.push(&coord2);
    address1.push(&coord3);
    address2.push(&coord2);
    address2.push(&coord3);
    address2.push(&coord1);

    ASSERT_EQ(address1.distanceTo(address2), 4);
}
