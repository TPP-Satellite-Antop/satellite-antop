#include <distance.h>
#include <gtest/gtest.h>

TEST(HammingDistance, Equal) {
    ASSERT_EQ(0, hammingDistance(0b10010110, 0b10010110));
}

TEST(HammingDistance, Max) {
    ASSERT_EQ(8, hammingDistance(0b00000000, 0b11111111));
}

TEST(HammingDistance, RandomOne) {
    ASSERT_EQ(4, hammingDistance(0b01001001, 0b10101101));
}

TEST(HammingDistance, RandomTwo) {
    ASSERT_EQ(3, hammingDistance(0b01111010, 0b11111111));
}

TEST(HammingDistance, RandomThree) {
    ASSERT_EQ(1, hammingDistance(0b01000011, 0b00000011));
}

TEST(HammingDistance, RandomFour) {
    ASSERT_EQ(5, hammingDistance(0b11001000, 0b00010010));
}

TEST(HammingDistance, RandomFive) {
    ASSERT_EQ(4, hammingDistance(0b00000000, 0b01100011));
}
