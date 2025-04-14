#ifndef ADDR_IDX_BIMAP_H
#define ADDR_IDX_BIMAP_H

#include <address.h>
#include <map>

#include "h3api.h"

class AddrIdxBiMap {

public:
    AddrIdxBiMap();
    void insert(const H3Index& idx, const Address& addr);
    H3Index retrieveIdx(const Address &addr);
    Address retrieveAddr(const H3Index &idx);

private:
    std::map<H3Index, Address> indexToAddress;
    std::map<Address, H3Index> addressToIndex;
};


#endif //ADDR_IDX_BIMAP_H
