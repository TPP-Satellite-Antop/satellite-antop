#ifndef ANTOPROUTINGTABLE_H
#define ANTOPROUTINGTABLE_H

#include <unordered_map>
#include <bitset>
#include <vector>

#include "Antop.h"
#include "h3api.h"

constexpr int NEIGHBORS = 6;

struct PairTableKey {
    H3Index source;
    H3Index destination;

    bool operator==(const PairTableKey& other) const {
        return source == other.source && destination == other.destination;
    }
};

// Hash: Allows using PairTableKey as a key in hash-based containers
template <> struct std::hash<PairTableKey> {
    std::size_t operator()(const PairTableKey &k) const noexcept {
        const std::size_t h1 = std::hash<H3Index>()(k.source);
        const std::size_t h2 = std::hash<H3Index>()(k.destination);
        return h1 ^ (h2 << 1);
    }
};

struct PairTableInfo {
    int distance = 0;
    int loopEpoch = 0;
};

struct RoutingInfo {
    H3Index nextHop = 0;
    int distance = 0;
    std::vector<H3Index> neighbors = {};
    std::bitset<NEIGHBORS> visitedBitmap{0}; // Each bit represents a neighbor. Since a pentagon has up to 5 neighbors, in that case, its last bit must never be used.
};

class RoutingTable {
    std::unordered_map<H3Index, RoutingInfo> routingTable;
    std::unordered_map<PairTableKey, PairTableInfo> pairTable;
    Antop* antop;
    double ttl = 0.0;

    std::vector<H3Index> getNeighbors(H3Index src, H3Index dst);
    void clear(double nextPositionUpdate);
    bool expired(double nextPositionUpdate) const;

public:
    explicit RoutingTable(Antop* antop);
    H3Index findNextHop(H3Index cur, H3Index src, H3Index dst, H3Index sender, int *curDistance, int *bundleLoopEpoch, double nextPositionUpdate);
    H3Index handleLoop(H3Index cur, H3Index src, H3Index dst, H3Index sender, int *bundleLoopEpoch, double nextPositionUpdate);
    H3Index findNewNeighbor(H3Index cur, H3Index dst, H3Index sender, double nextPositionUpdate);
    int getAntopResolution() const;
};

#endif // ANTOPROUTINGTABLE_H
