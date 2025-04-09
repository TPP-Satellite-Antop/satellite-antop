#ifndef BIMAP_H
#define BIMAP_H

#include <address.h>
#include <map>

#include "h3api.h"

class AddrIdxBiMap {
public:
    AddrIdxBiMap();
    void insert(const H3Index& idx, const Address& addr);

private:
    std::map<H3Index, Address> indexToAddress;
    std::map<Address, H3Index> addressToIndex;
};


#endif //BIMAP_H
