#include "RoutingTable.h"
#include <vector>
#include <iostream>

constexpr int THRESHOLD_DISTANCE = 0;
constexpr std::bitset<NEIGHBORS> MSB_MASK = {0b100000};

RoutingTable::RoutingTable(Antop* antop) {
    this->antop = antop;
}

// Returns true if the routing information for a given destination should be upserted.
bool shouldUpdateSrcInfo(const H3Index sender, const RoutingInfo &routingInfoToSrc, const int curDistance, const std::vector<H3Index>& neighbors) {
    return sender != 0
        && (routingInfoToSrc.nextHop == 0 || routingInfoToSrc.distance > curDistance)
        && std::ranges::find(neighbors, sender) != neighbors.end();
}

bool isLoop(const int storedDistance, const int curDistance) {
    return storedDistance + THRESHOLD_DISTANCE < curDistance;
}

// Flags the target index as visited in the bitmap. Returns true if the routing information towards the packet's source
// should be updated to flag the provided sender as the next hop. It otherwise returns false if the sender is not a
// valid next hop.
void flagSenderAsVisited(std::bitset<NEIGHBORS>& bitmap, const std::vector<H3Index>& candidates, const H3Index sender) {
    auto curNeighbor = MSB_MASK;

    for (const auto candidate : candidates) {
        if (candidate == sender) {
            bitmap |= curNeighbor;
            break;
        }
        curNeighbor >>= 1;
    }
}

// Returns the best unvisited candidate. If none is found, findNextCandidate returns the provided fallback.
H3Index findNextCandidate(std::bitset<NEIGHBORS>& bitmap, const std::vector<H3Index>& candidates, const H3Index sender, const H3Index fallback) {
    auto senderVisited = true; // In case the sender is invalid and cannot be found in the candidates, return fallback.
    auto curNeighbor = MSB_MASK;

    for (const auto candidate : candidates) {
        if (candidate == sender) {
            senderVisited = (bitmap & curNeighbor).any();
        } else if ((bitmap & curNeighbor).none()) {
            bitmap |= curNeighbor;
            return candidate;
        }
        curNeighbor >>= 1;
    }

    return senderVisited ? fallback : sender;
}

// Attempts to fetch the candidate list from the cache, going from src to dst. If unable to, the candidate list is
// computed and stored into the cache.
std::vector<H3Index> RoutingTable::getNeighbors(const H3Index src, const H3Index dst) {
    if (auto candidates = routingTable[dst].neighbors; !candidates.empty())
        return candidates;
    const auto candidates = antop->getHopCandidates(src, dst);
    routingTable[dst].neighbors = candidates;
    return candidates;
}

// Retrieves a candidate for a packet's next hop.
H3Index RoutingTable::findNextHop(
    const H3Index cur,
    const H3Index src,
    const H3Index dst,
    const H3Index sender,
    int *curDistance,
    int *bundleLoopEpoch,
    const double nextPositionUpdate
) {
    if (curDistance == nullptr)
        throw std::invalid_argument("curDistance pointer cannot be null");

    if (this->expired(nextPositionUpdate))
        this->clear(nextPositionUpdate);

    const PairTableKey pairTableKey{src, dst};
    int storedDistance = *curDistance;
    if (const auto it = pairTable.find(pairTableKey); it != pairTable.end())
        storedDistance = it->second.distance;

    if (isLoop(storedDistance, *curDistance)) {
        *curDistance = storedDistance;
        return handleLoop(cur, src, dst, sender, bundleLoopEpoch, nextPositionUpdate);
    }

    pairTable[pairTableKey].distance = storedDistance == 0 ? *curDistance : std::min(storedDistance, *curDistance);

    if (src != 0) {
        const auto candidates = getNeighbors(cur, src);

        if (const RoutingInfo routingInfoToSrc = routingTable[src]; shouldUpdateSrcInfo(sender, routingInfoToSrc, *curDistance, candidates)) {
            std::bitset<NEIGHBORS> bitmap = routingInfoToSrc.visitedBitmap;

            flagSenderAsVisited(bitmap, candidates, sender);

            routingTable[src] = {sender, *curDistance, candidates, bitmap};
        }
    }

    if (const RoutingInfo routingInfoToDst = routingTable[dst]; routingInfoToDst.nextHop != 0)
        return routingInfoToDst.nextHop;
    return findNewNeighbor(cur, dst, sender, nextPositionUpdate);
}

H3Index RoutingTable::handleLoop(
    const H3Index cur,
    const H3Index src,
    const H3Index dst,
    const H3Index sender,
    int *bundleLoopEpoch,
    const double nextPositionUpdate
) {
    const PairTableKey key{src, dst};

    if (*bundleLoopEpoch < pairTable[key].loopEpoch) { // Loop has already been resolved locally.
        *bundleLoopEpoch = pairTable[key].loopEpoch;
        return routingTable[dst].nextHop;
    }

    (*bundleLoopEpoch)++;
    pairTable[key].loopEpoch++;

    if (*bundleLoopEpoch > pairTable[key].loopEpoch) // Loop has been resolved remotely. Update local state.
        pairTable[key].loopEpoch = *bundleLoopEpoch;

    return findNewNeighbor(cur, dst, sender, nextPositionUpdate);
}

// Retrieves a new candidate for a packet's next hop.
H3Index RoutingTable::findNewNeighbor(
    const H3Index cur,
    const H3Index dst,
    const H3Index sender,
    const double nextPositionUpdate
) {
    if (this->expired(nextPositionUpdate))
        this->clear(nextPositionUpdate);

    auto bitmap = routingTable[dst].visitedBitmap;
    const std::vector<H3Index> candidates = getNeighbors(cur, dst);

    const H3Index nextCandidate = findNextCandidate(bitmap, candidates, sender, cur);
    if (nextCandidate == sender)
        flagSenderAsVisited(bitmap, candidates, sender);

    routingTable[dst].nextHop = nextCandidate;
    routingTable[dst].visitedBitmap = bitmap;

    return nextCandidate;
}

int RoutingTable::getAntopResolution() const {
    return this->antop->getResolution();
}

bool RoutingTable::expired(const double nextPositionUpdate) const {
    return nextPositionUpdate > this->ttl;
}

void RoutingTable::clear(const double nextPositionUpdate) {
    this->routingTable.clear();
    this->pairTable.clear();
    this->ttl = nextPositionUpdate;
}
