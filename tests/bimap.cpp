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
    addr.push(new CoordIJK{1, 1, 0});

    bimap.insert(idx, addr);

    auto retrievedIdx = bimap.retrieveIdx(addr);
    ASSERT_EQ(retrievedIdx, idx);

    // Check if the address can be retrieved from the index
    auto retrievedAddr = bimap.retrieveAddr(idx);
    ASSERT_EQ(retrievedAddr.data(), addr.data());
}