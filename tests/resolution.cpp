#include <gtest/gtest.h>
#include "resolution.h"

TEST(ResolutionTest, GetOriginForValidResolutions) {
    EXPECT_EQ(getOriginForResolution(1), 0x81463ffffffffff);
    EXPECT_EQ(getOriginForResolution(2), 0x824607fffffffff);
    EXPECT_EQ(getOriginForResolution(3), 0x834600fffffffff);
    EXPECT_EQ(getOriginForResolution(4), 0x8446001ffffffff);
    EXPECT_EQ(getOriginForResolution(5), 0x85460003fffffff);
}

TEST(ResolutionTest, GetOriginForInvalidResolutionThrows) {
    EXPECT_THROW(getOriginForResolution(0), std::out_of_range);
    EXPECT_THROW(getOriginForResolution(6), std::out_of_range);
    EXPECT_THROW(getOriginForResolution(999), std::out_of_range);
}

TEST(ResolutionTest, GetResolutionValidSatellites) {
    EXPECT_EQ(getResolution(50), 1);
    EXPECT_EQ(getResolution(110), 1);
    EXPECT_EQ(getResolution(200), 2);
    EXPECT_EQ(getResolution(830), 2);
    EXPECT_EQ(getResolution(1000), 3);
    EXPECT_EQ(getResolution(5870), 3);
    EXPECT_EQ(getResolution(41150), 4);
    EXPECT_EQ(getResolution(288110), 5);
}

TEST(ResolutionTest, GetResolutionTooManySatellitesThrows) {
    EXPECT_THROW(getResolution(2016831), std::out_of_range);
    EXPECT_THROW(getResolution(99999999), std::out_of_range);
}