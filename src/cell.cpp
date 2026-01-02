#include "cell.h"
#include "limits"

constexpr int INFINITE = std::numeric_limits<int>::max();

void Cell::addAddress(const Address& address) {
    addresses.push_back(address);
}

int Cell::distanceTo(const Cell& cell) const {
    int dist = INFINITE;

    for (const Address& addr1 : this->addresses) {
        for (const Address& addr2 : cell.addresses) {
            dist = std::min(dist, addr1.distanceTo(addr2));
        }
    }

    return dist;
}
