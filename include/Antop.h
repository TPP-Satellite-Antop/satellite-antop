#ifndef ANTOP_H
#define ANTOP_H

#include <unordered_map>

#include "Hypercube.h"
#include "Cell.h"
#include "h3util.h"

class Antop {
    uint8_t hypercubeLookup[122][122]; // Matches two H3 cells to the hypercube ID recommended to calculate distance between them.
    std::array<Hypercube, pentagonsPerRes> hypercubes;






public:
    std::unordered_map<H3Index, std::vector<H3Index>> neighborsByIdx{};
    std::unordered_map<H3Index, Cell> cellByIdx;
    std::unordered_map<Address, H3Index> addresses;

    int distance(H3Index idx1, H3Index idx2);

    Antop() = default; // ToDo: replace default to custom constructor. This constructor should reserve memory for all three maps to avoid unnecessary rehashing.
    void init(int satellites);
    int getResolution() const;
    std::vector<H3Index> getHopCandidates(H3Index src, H3Index dst);
};

#endif //ANTOP_H