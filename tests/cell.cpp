#include <gtest/gtest.h>
#include "cell.h"

TEST(CellTest, EmptyCellsDistance) {
    const Cell cell1;
    const Cell cell2;

    ASSERT_EQ(cell1.distanceTo(&cell2), std::numeric_limits<int>::max());
}

TEST(CellTest, ZeroDistanceToSelf) {
    Cell cell;
    Address address(false);
    constexpr CoordIJK coord{0, 1, 1};
    address.push(&coord);
    cell.addAddress(address);

    ASSERT_EQ(cell.distanceTo(&cell), 0);
}

TEST(CellTest, DifferentDimensionsDistance) {
    Cell cell1;
    Cell cell2;

    Address address1(false);
    Address address2(true);  // Different dimension
    constexpr CoordIJK coord{0, 1, 1};
    address1.push(&coord);
    address2.push(&coord);

    cell1.addAddress(address1);
    cell2.addAddress(address2);

    ASSERT_EQ(cell1.distanceTo(&cell2), std::numeric_limits<int>::max());
}

TEST(CellTest, SingleAddressDistance) {
    Cell cell1;
    Cell cell2;

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

    cell1.addAddress(address1);
    cell2.addAddress(address2);

    ASSERT_EQ(cell1.distanceTo(&cell2), 4);
}

TEST(CellTest, MultipleAddressesMinDistance) {
    Cell cell1;
    Cell cell2;

    // First pair of addresses with distance 4
    Address address1A(false);
    Address address2A(false);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};
    constexpr CoordIJK coord3{1, 1, 0};

    address1A.push(&coord1);
    address1A.push(&coord2);
    address1A.push(&coord3);

    address2A.push(&coord2);
    address2A.push(&coord3);
    address2A.push(&coord1);

    // Second pair of addresses with same coordinates (distance 0)
    Address address1B(false);
    Address address2B(false);
    address1B.push(&coord1);
    address2B.push(&coord1);

    cell1.addAddress(address1A);
    cell1.addAddress(address1B);
    cell2.addAddress(address2A);
    cell2.addAddress(address2B);

    // Should return 0 as it's the minimum distance between any pair of addresses
    ASSERT_EQ(cell1.distanceTo(&cell2), 0);
}

TEST(CellTest, AsymmetricDistance) {
    Cell cell1;
    Cell cell2;

    Address address1(false);
    Address address2(false);
    constexpr CoordIJK coord1{0, 1, 1};
    constexpr CoordIJK coord2{0, 1, 0};

    address1.push(&coord1);
    address2.push(&coord2);

    cell1.addAddress(address1);
    cell2.addAddress(address2);

    // Distance should be the same in both directions
    ASSERT_EQ(cell1.distanceTo(&cell2), cell2.distanceTo(&cell1));
}