#include <gtest/gtest.h>
#include "resolution.h"

TEST(ResolutionTest, GetOriginForValidResolutions) {
    EXPECT_EQ(getOriginForResolution(0), 0x8047fffffffffff);
    EXPECT_EQ(getOriginForResolution(1), 0x81463ffffffffff);
    EXPECT_EQ(getOriginForResolution(2), 0x824607fffffffff);
}

TEST(ResolutionTest, GetOriginForInvalidResolutionThrows) {
    EXPECT_THROW(getOriginForResolution(6), std::out_of_range);
    EXPECT_THROW(getOriginForResolution(999), std::out_of_range);
}

TEST(ResolutionTest, GetResolutionValidSatellites) {
    EXPECT_EQ(findResolution(50), 0);
    EXPECT_EQ(findResolution(122), 0);
    EXPECT_EQ(findResolution(200), 0);
    EXPECT_EQ(findResolution(830), 1);
    EXPECT_EQ(findResolution(5883), 2);
}