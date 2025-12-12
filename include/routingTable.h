#ifndef ANTOPROUTINGTABLE_H
#define ANTOPROUTINGTABLE_H

#include <unordered_map>
#include <bitset>

#include "h3api.h"
#include "antop.h"
#include <bitset>

constexpr int NEIGHBORS = 6;

struct PairTableKey {
    H3Index source;
    H3Index destination;

    bool operator==(const PairTableKey& other) const {
        return source == other.source && destination == other.destination;
    }
};

// Hash: Allows to use PairTableKey as a key in hash-based containers
template <> struct std::hash<PairTableKey> {
    std::size_t operator()(const PairTableKey &k) const noexcept {
        const std::size_t h1 = std::hash<H3Index>()(k.source);
        const std::size_t h2 = std::hash<H3Index>()(k.destination);
        return h1 ^ (h2 << 1);
    }
};

struct RoutingInfo {
    H3Index nextHop = 0;
    int distance = 0;
    std::vector<H3Index> neighbors = {};
    std::bitset<NEIGHBORS> visitedBitmap{0}; // Each bit represents a neighbour. Since a pentagon has up to 5 neighbours, in that case, its last bit must never be used.
};

class RoutingTable {
    std::unordered_map<H3Index, RoutingInfo> routingTable;
    std::unordered_map<PairTableKey, int> pairTable; // Maps (src,dst) to distance
    Antop* antop;
    double ttl = 0.0;

    std::vector<H3Index> getNeighbors(H3Index src, H3Index dst);

public:
    explicit RoutingTable(Antop* antop);
    H3Index findNextHop(H3Index cur, H3Index src, H3Index dst, H3Index sender, int curDistance, double currentTime);
    H3Index findNewNeighbor(H3Index cur, H3Index dst, H3Index sender, double currentTime);
    void maybeClearRoutingTable(double currentTime);
    bool hasRoutingTableExpired();
    int getAntopResolution() const;
};

#endif // ANTOPROUTINGTABLE_H
