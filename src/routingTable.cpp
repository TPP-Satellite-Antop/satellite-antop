#include "routingTable.h"
#include <vector>
#include <iostream>
#include <ostream>

constexpr int THRESHOLD_DISTANCE = 3;
constexpr std::bitset<NEIGHBORS> MSB_MASK = {0b100000};

RoutingTable::RoutingTable(Antop* antop) {
    this->antop = antop;
}

// Returns true if the routing info to source should be updated.
bool shouldUpdateSrcInfo(const RoutingInfo &routingInfoToSrc, const int curDistance) {
    return routingInfoToSrc.nextHop == 0 || routingInfoToSrc.distance > curDistance;
}

bool isLoop(const int storedDistance, const int curDistance) {
    return storedDistance != 0 && storedDistance + THRESHOLD_DISTANCE < curDistance;
}

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

H3Index findNextUnvisitedNeighbor(std::bitset<NEIGHBORS>& bitmap, const std::vector<H3Index>& candidates, const H3Index fallback) {
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

H3Index RoutingTable::findNextHop(const H3Index cur, const H3Index src, const H3Index dst, const H3Index sender, const int curDistance) {
    const PairTableKey pairTableKey{src, dst};
    const int storedDistance = pairTable[pairTableKey];

    if (isLoop(storedDistance, curDistance)) {
        return sender;
    }
    
    pairTable[pairTableKey] = storedDistance == 0 ? curDistance : std::min(storedDistance, curDistance);

    if (const RoutingInfo routingInfoToSrc = routingTable[src]; shouldUpdateSrcInfo(routingInfoToSrc, curDistance)) {
        const std::vector<H3Index> candidates = antop->getHopCandidates(cur, dst, 0);
        auto senderIdxInCandidates = MSB_MASK;
        
        for (const auto candidate : candidates) {
            if (candidate == sender)
                break;
            senderIdxInCandidates >>= 1;
        }
        
        routingTable[src] = {sender, 0, curDistance, {}, senderIdxInCandidates}; // ToDo: save actual TTL.
    }

    if (const RoutingInfo routingInfoToDst = routingTable[dst]; routingInfoToDst.nextHop != 0)
        return routingInfoToDst.nextHop;
    return findNewNeighbor(cur, dst, sender);
}

H3Index RoutingTable::findNewNeighbor(const H3Index cur, const H3Index dst, const H3Index sender) {
    auto bitmap = routingTable[dst].visitedBitmap;
    const std::vector<H3Index> candidates = antop->getHopCandidates(cur, dst, 0);

    flagTargetAsVisited(bitmap, candidates, sender);
    
    const H3Index nextNeighbor = findNextUnvisitedNeighbor(bitmap, candidates, sender);

    routingTable[dst].nextHop = nextNeighbor;
    routingTable[dst].visitedBitmap = bitmap;

    return nextNeighbor;
}

int RoutingTable::getAntopResolution() const {
    return this->antop->getResolution();
}
