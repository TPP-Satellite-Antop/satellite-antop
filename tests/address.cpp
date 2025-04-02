#include <gtest/gtest.h>
#include "address.h"
extern "C" {
    #include "h3lib/include/coordijk.h"
}

TEST(AddressTest, PushOneCoordToEmptyAddress) {
    Address address;
    constexpr CoordIJK coord{1, 1, 0};

    address.push(&coord);

    ASSERT_EQ(address.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
}

TEST(AddressTest, PushSomeCoordToEmptyAddress) {
    Address address;
    constexpr CoordIJK coord1{1, 1, 0};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address.push(&coord1);
    address.push(&coord2);
    address.push(&coord3);

    const auto exp = std::vector<uint8_t>{IJ_AXES_DIGIT + (J_AXES_DIGIT << 3), IJ_AXES_DIGIT};

    ASSERT_EQ(address.data(), exp);
}
