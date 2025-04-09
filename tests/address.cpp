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
    Address address(false);    constexpr CoordIJK coord1{1, 1, 0};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    const auto exp = std::vector<uint8_t>{IJ_AXES_DIGIT + (J_AXES_DIGIT << 3), IJ_AXES_DIGIT};

    ASSERT_EQ(address.data(), exp);
}
