#include "routingTable.h"
#include <vector>
#include <iostream>
#include <ostream>

const int THRESHOLD_DISTANCE = 3;
const std::bitset<MAX_VISITED_NEIGHBORS> MSB_MASK = {0b100000};

RoutingTable::RoutingTable(int nodesNumber) {
    this->antop = new Antop();
    this->antop->init(nodesNumber);
}


// Returns true if the routing info to source should be updated.
bool shouldUpdateSrcInfo(RoutingInfo &routingInfoToSrc, int curDistance) {
    return routingInfoToSrc.nextHop == 0 || routingInfoToSrc.distance > curDistance;
}

bool isLoop(int storedDistance, int curDistance) {
    return storedDistance != 0 && storedDistance + THRESHOLD_DISTANCE < curDistance;
}

void markVisited(
    std::bitset<MAX_VISITED_NEIGHBORS>& bitmap,
    const std::vector<H3Index>& candidates,
    H3Index visited
) {
    auto curNeighbor = MSB_MASK;

    for (auto candidate : candidates) {
        if (candidate == visited) {
            bitmap |= curNeighbor;
            return;
        }
        curNeighbor >>= 1;
    }

    throw std::runtime_error("Sender not found in candidates");
}

H3Index findNextUnvisitedNeighbor(
    std::bitset<MAX_VISITED_NEIGHBORS>& bitmap,
    const std::vector<H3Index>& candidates,
    H3Index fallback
) {
    auto curNeighbor = MSB_MASK;

    for (auto candidate : candidates) {
        if ((bitmap & curNeighbor).none()) {
            bitmap |= curNeighbor;   // mark as visited
            return candidate;
        }
        curNeighbor >>= 1;
    }

    return fallback;
}

H3Index RoutingTable::findNextHop(H3Index cur, H3Index src, H3Index dst, H3Index sender, int curDistance) {
    PairTableKey key{src, dst};
    int storedDistance = pairTable[key];
    if (isLoop(storedDistance, curDistance)) {
        return sender;
    }
    
    pairTable[key] = storedDistance == 0 ? curDistance : std::min(storedDistance, curDistance);
    
    RoutingInfo routingInfoToSrc = routingTable[src];
    if (shouldUpdateSrcInfo(routingInfoToSrc, curDistance)) {
        std::vector<H3Index> candidates = antop->getHopCandidates(cur, dst, 0);
        __uint8_t senderIdxInCandidates = 128;
        
        for (auto candidate : candidates) {
            if (candidate == sender)
            break;
            senderIdxInCandidates = senderIdxInCandidates >> 1;
        }
        
        routingTable[src] = {sender, 0, curDistance, senderIdxInCandidates}; // ToDo: save actual TTL.
    }
    
    RoutingInfo routingInfoToDst = routingTable[dst];
    if (routingInfoToDst.nextHop != 0)
    return routingInfoToDst.nextHop;
    
    return findNewNeighbor(cur, dst, sender);
}

H3Index RoutingTable::findNewNeighbor(const H3Index cur, const H3Index dst, const H3Index sender) {
    auto bitmap = routingTable[dst].visitedNeighboursBitmap;
    std::vector<H3Index> candidates = antop->getHopCandidates(cur, dst, 0);

    // Flag sender as visited.
    markVisited(bitmap, candidates, sender);
    
    // Find next unvisited neighbor fallbacking to sender
    H3Index nextNeighbor = findNextUnvisitedNeighbor(bitmap, candidates, sender);

    routingTable[dst].nextHop = nextNeighbor;
    routingTable[dst].visitedNeighboursBitmap = bitmap;

    return nextNeighbor;
}

int RoutingTable::getAntopResolution() {
    return this->antop->getResolution();
}