#include "addrIdxBiMap.h"

#include <optional>
#include <stdexcept>

AddrIdxBiMap::AddrIdxBiMap() = default;

void AddrIdxBiMap::insert(const AddrIdx& addrIdx) {
    indexToAddress.insert_or_assign(addrIdx.idx, addrIdx.addr);
    addressToIndex.insert_or_assign(addrIdx.addr, addrIdx.idx);
}

H3Index AddrIdxBiMap::getIdx(const Address& addr) {
    if (const auto it = addressToIndex.find(addr); it != addressToIndex.end()) {
        return it->second;
    }
    throw std::runtime_error("Address not found in AddrIdxBiMap");
}

std::optional<H3Index> AddrIdxBiMap::tryGetIdx(const Address& addr) const {
    if (const auto it = addressToIndex.find(addr); it != addressToIndex.end()) {
        return it->second;
    }
    return std::nullopt;
}

Address AddrIdxBiMap::getAddr(const H3Index& idx) {
    if (const auto it = indexToAddress.find(idx); it != indexToAddress.end()) {
        return it->second;
    }
    throw std::runtime_error("Index not found in AddrIdxBiMap");
}

std::optional<Address> AddrIdxBiMap::tryGetAddr(const H3Index& idx) const {
    if (const auto it = indexToAddress.find(idx); it != indexToAddress.end()) {
        return it->second;
    }
    return std::nullopt;
}
