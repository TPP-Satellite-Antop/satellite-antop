#include "routingTable.h"
#include <vector>
#include <iostream>
#include <ostream>

constexpr int THRESHOLD_DISTANCE = 3;
constexpr std::bitset<NEIGHBORS> MSB_MASK = {0b100000};

RoutingTable::RoutingTable(Antop* antop) {
    this->antop = antop;
}

// Returns true if the routing information for a given destination should be upserted.
bool shouldUpdateSrcInfo(const RoutingInfo &routingInfoToSrc, const int curDistance) {
    return routingInfoToSrc.nextHop == 0 || routingInfoToSrc.distance > curDistance;
}

bool isLoop(const int storedDistance, const int curDistance) {
    return storedDistance != 0 && storedDistance + THRESHOLD_DISTANCE < curDistance;
}

// Flags the target index as visited in the bitmap.
void flagTargetAsVisited(std::bitset<NEIGHBORS>& bitmap, const std::vector<H3Index>& candidates, const H3Index target) {
    auto curNeighbor = MSB_MASK;

    for (const auto candidate : candidates) {
        if (candidate == target) {
            bitmap |= curNeighbor;
            return;
        }
        curNeighbor >>= 1;
    }

    throw std::runtime_error("Sender not found in candidates");
}

// Returns the best unvisited candidate. If none is found, findNextCandidate returns the provided fallback.
H3Index findNextCandidate(std::bitset<NEIGHBORS>& bitmap, const std::vector<H3Index>& candidates, const H3Index fallback) {
    auto curNeighbor = MSB_MASK;

    for (const auto candidate : candidates) {
        if ((bitmap & curNeighbor).none()) {
            bitmap |= curNeighbor;
            return candidate;
        }
        curNeighbor >>= 1;
    }

    return fallback;
}

std::vector<H3Index> RoutingTable::getNeighbors(const H3Index src, const H3Index dst) {
    if (auto candidates = routingTable[dst].neighbors; !candidates.empty())
        return candidates;
    const auto candidates = antop->getHopCandidates(src, dst, 0);
    routingTable[dst].neighbors = candidates;
    return candidates;
}

// Retrieves a candidate for a packet's next hop.
H3Index RoutingTable::findNextHop(const H3Index cur, const H3Index src, const H3Index dst, const H3Index sender, const int curDistance) {
    const PairTableKey pairTableKey{src, dst};
    const int storedDistance = pairTable[pairTableKey];

    if (isLoop(storedDistance, curDistance)) {
        return sender;
    }
    
    pairTable[pairTableKey] = storedDistance == 0 ? curDistance : std::min(storedDistance, curDistance);

    if (const RoutingInfo routingInfoToSrc = routingTable[src]; shouldUpdateSrcInfo(routingInfoToSrc, curDistance)) {
        const auto candidates = getNeighbors(cur, dst);
        auto bitmap = MSB_MASK;
        
        for (const auto candidate : candidates) {
            if (candidate == sender)
                break;
            bitmap >>= 1;
        }
        
        routingTable[src] = {sender, 0, curDistance, candidates, bitmap}; // ToDo: save actual TTL.
    }

    if (const RoutingInfo routingInfoToDst = routingTable[dst]; routingInfoToDst.nextHop != 0)
        return routingInfoToDst.nextHop;
    return findNewNeighbor(cur, dst, sender);
}

// Retrieves a new candidate for a packet's next hop.
H3Index RoutingTable::findNewNeighbor(const H3Index cur, const H3Index dst, const H3Index sender) {
    auto bitmap = routingTable[dst].visitedBitmap;
    const std::vector<H3Index> candidates = getNeighbors(cur, dst);

    flagTargetAsVisited(bitmap, candidates, sender);
    
    const H3Index nextCandidate = findNextCandidate(bitmap, candidates, sender);

    routingTable[dst].nextHop = nextCandidate;
    routingTable[dst].visitedBitmap = bitmap;

    return nextCandidate;
}

int RoutingTable::getAntopResolution() const {
    return this->antop->getResolution();
}
