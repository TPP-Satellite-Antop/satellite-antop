#include <gtest/gtest.h>
#include "h3util.h"

TEST(ResolutionTest, GetResolutionValidSatellites) {
    EXPECT_EQ(findResolution(50), 0);
    EXPECT_EQ(findResolution(122), 0);
    EXPECT_EQ(findResolution(200), 0);
    EXPECT_EQ(findResolution(830), 1);
    EXPECT_EQ(findResolution(5883), 2);
}