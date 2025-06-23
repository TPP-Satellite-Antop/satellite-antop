#include <gtest/gtest.h>
#include "address.h"

extern "C" {
    #include "h3lib/include/coordijk.h"
}

TEST(AddressTest, DeepCopyAddress) {
    constexpr CoordIJK coord{1, 1, 0};

    Address address1(false);
    address1.push(&coord);

    Address address2 = address1.copy();
    address2.push(&coord);

    ASSERT_NE(address1.coord(), address2.coord());
    ASSERT_EQ(address1.coord(), Coord(CoordIJK(1,1,0)));
    ASSERT_EQ(address2.coord(), Coord(CoordIJK(2,2,0)));
}

TEST(AddressTest, PushOneCoordToEmptyAddress) {
    Address address(false);
    constexpr CoordIJK coord{1, 1, 0};

    address.push(&coord);

    ASSERT_EQ(address.coord(), Coord(CoordIJK(1,1,0)));
}

TEST(AddressTest, PushSomeCoordToEmptyAddress) {
    Address address(false);
    constexpr CoordIJK coord1{1, 1, 0};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    ASSERT_EQ(address.coord(), Coord(CoordIJK(2,3,0)));
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
