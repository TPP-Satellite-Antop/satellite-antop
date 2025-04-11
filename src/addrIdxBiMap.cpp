#include "addrIdxBiMap.h"

#include <stdexcept>

AddrIdxBiMap::AddrIdxBiMap() = default;

void AddrIdxBiMap::insert(const H3Index& idx, const Address& addr) {
    indexToAddress.insert_or_assign(idx, addr);
    addressToIndex.insert_or_assign(addr, idx);
}

H3Index AddrIdxBiMap::retrieveIdx(const Address& addr) {
    if (const auto it = addressToIndex.find(addr); it != addressToIndex.end()) {
        return it->second;
    }

    throw std::runtime_error("Address not found in AddrIdxBiMap");
}

Address AddrIdxBiMap::retrieveAddr(const H3Index& idx) {
    if (const auto it = indexToAddress.find(idx); it != indexToAddress.end()) {
        return it->second;
    }

    throw std::runtime_error("Index not found in AddrIdxBiMap");
}

