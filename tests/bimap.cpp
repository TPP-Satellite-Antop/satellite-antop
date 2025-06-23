#include <gtest/gtest.h>
#include "address.h"
#include "addrIdxBiMap.h"

extern "C" {
    #include "h3lib/include/coordijk.h"
}

TEST(AddrIdxBiMapTest, InsertAndRetrieve) {
    AddrIdxBiMap bimap;
    constexpr H3Index idx = 0x807dfffffffffff;
    Address addr(false);
    constexpr CoordIJK coord{1, 1, 0};
    addr.push(&coord);
    bimap.insert({idx, addr});

    ASSERT_EQ(bimap.getIdx(addr), idx);
    ASSERT_EQ(bimap.getAddr(idx).coord(), addr.coord());
}

TEST(AddrIdxBiMapTest, TryRetrieve) {
    AddrIdxBiMap bimap;
    constexpr H3Index idx = 0x807dfffffffffff;
    Address addr(false);
    constexpr CoordIJK coord{1, 1, 0};
    addr.push(&coord);
    bimap.insert({idx, addr});

    const auto optIdx = bimap.tryGetIdx(addr);
    auto optAddr = bimap.tryGetAddr(idx);

    ASSERT_TRUE(optIdx.has_value());
    ASSERT_TRUE(optAddr.has_value());
    ASSERT_EQ(optIdx.value(), idx);
    ASSERT_EQ(optAddr.value().coord(), addr.coord());
}

TEST(AddrIdxBiMapTest, EmptyTryRetrieve) {
    const AddrIdxBiMap bimap;

    ASSERT_FALSE(bimap.tryGetIdx(Address(false)).has_value());
    ASSERT_FALSE(bimap.tryGetAddr(0x807dfffffffffff).has_value());
}
