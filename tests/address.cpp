#include <gtest/gtest.h>
#include "address.h"
extern "C" {
    #include "h3lib/include/coordijk.h"
}

TEST(AddressTest, PushOneCoordToEmptyAddress) {
    Address address;
    const CoordIJK coord{1, 1, 0};

    address.push(&coord);

    ASSERT_EQ(address.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
}

TEST(AddressTest, PushManyCoordToEmptyAddress) {
    Address address;
    const CoordIJK coord{1, 1, 0};

    address.push(&coord);

    ASSERT_EQ(address.data(), std::vector<uint8_t>{IJ_AXES_DIGIT});
}