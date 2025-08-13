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

    ASSERT_NE(address1.data(), address2.data());
    ASSERT_EQ(address1.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
    ASSERT_EQ(address2.data(), std::vector<uint8_t>{IJ_AXES_DIGIT + (IJ_AXES_DIGIT << 3)});
}

TEST(AddressTest, PushOneCoordToEmptyAddress) {
    Address address(false);
    constexpr CoordIJK coord{1, 1, 0};

    address.push(&coord);

    ASSERT_EQ(address.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
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

TEST(AddressTest, PopFromEmptyAddress) {
    Address address{};
    address.pop();
    constexpr auto exp = std::vector<uint8_t>{};
    ASSERT_EQ(address.data(), exp);
}

TEST(AddressTest, PopFromSingleOutputAddress) {
    Address address{};
    constexpr CoordIJK coord{1, 1, 0};
    address.push(&coord);
    address.pop();
    constexpr auto exp = std::vector<uint8_t>{};
    ASSERT_EQ(address.data(), exp);
}

TEST(AddressTest, PopFromEvenSizeAddress) {
    Address address{};
    constexpr CoordIJK coord{1, 1, 0};
    address.push(&coord);
    address.push(&coord);
    address.pop();
    const auto exp = std::vector<uint8_t>{IJ_AXES_DIGIT};
    ASSERT_EQ(address.data(), exp);
}

TEST(AddressTest, PopFromOddSizeAddress) {
    Address address{};
    constexpr CoordIJK coord{1, 1, 0};
    address.push(&coord);
    address.push(&coord);
    address.push(&coord);
    address.pop();
    const auto exp = std::vector<uint8_t>{IJ_AXES_DIGIT + (IJ_AXES_DIGIT << 3)};
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
