#include <gtest/gtest.h>
#include "address.h"
#include "addrIdxBiMap.h"

extern "C" {
    #include "h3lib/include/coordijk.h"
}

TEST(AddrIdxBiMapTest, InsertAndRetrieve) {
    AddrIdxBiMap bimap;
    H3Index idx = 0x807dfffffffffff;
    Address addr(false);
    CoordIJK coord{1, 1, 0};
    addr.push(&coord);
    bimap.insert(idx, addr);

    auto retrievedIdx = bimap.retrieveIdx(addr);
    ASSERT_EQ(retrievedIdx, idx);

    auto retrievedAddr = bimap.retrieveAddr(idx);
    ASSERT_EQ(retrievedAddr.data(), addr.data());
}