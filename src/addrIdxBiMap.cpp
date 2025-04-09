#include "addrIdxBiMap.h"

AddrIdxBiMap::AddrIdxBiMap() = default;

void AddrIdxBiMap::insert(const H3Index& idx, const Address& addr) {
    indexToAddress.insert_or_assign(idx, addr);
    addressToIndex.insert_or_assign(addr, idx);
}

