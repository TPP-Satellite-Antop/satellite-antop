#ifndef ADDR_IDX_BIMAP_H
#define ADDR_IDX_BIMAP_H

#include <address.h>
#include <map>
#include <optional>

#include "h3api.h"

struct AddrIdx {
    H3Index idx{};
    Address addr;
};

class AddrIdxBiMap {

public:
    AddrIdxBiMap();
    void insert(const H3Index& idx, const Address& addr);
    H3Index getIdx(const Address &addr);

    std::optional<H3Index> tryGetIdx(const Address &addr) const;

    Address getAddr(const H3Index &idx);
    [[nodiscard]] std::optional<Address> tryGetAddr(const H3Index &idx) const;

private:
    std::map<H3Index, Address> indexToAddress;
    std::map<Address, H3Index> addressToIndex;
};


#endif //ADDR_IDX_BIMAP_H
