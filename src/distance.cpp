#include <queue>
#include <unordered_map>
#include "distance.h"

int hammingDistance(const uint8_t origin, const uint8_t target) {
    uint8_t x = origin ^ target;
    uint8_t count = 0;

    while (x) {
        x &= x - 1;
        ++count;
    }

    return count;
}

// ToDo: cache the result of this since it's a major performance killer for high resolutions.
int h3Distance(const H3Index origin, const H3Index target) {
    if (origin == target) return 0;

    std::queue<H3Index> q;
    std::unordered_map<H3Index, int> dist;

    q.push(origin);
    dist[origin] = 0;

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();

        const int d = dist[cur];

        H3Index neighbors[7];
        gridDisk(cur, 1, neighbors);

        for (const auto n : neighbors) {
            if (n == 0) continue;

            if (!dist.contains(n)) {
                if (n == target) return d + 1;

                dist[n] = d + 1;
                q.push(n);
            }
        }
    }

    // Unreachable (shouldn't happen under normal conditions).
    throw std::runtime_error("Origin and target cells resolution do not match");
}
