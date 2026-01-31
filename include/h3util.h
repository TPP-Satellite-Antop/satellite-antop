#ifndef H3UTIL_H
#define H3UTIL_H

#include <array>
#include <span>

#include "h3api.h"

constexpr int maxRes = 3;
constexpr int pentagonsPerRes = 12;
constexpr std::array cellsPerRes = { 122, 1, 1 }; // ToDo: update to { 122, 842, 5882 }

struct CellInfo {
    std::array<H3Index, pentagonsPerRes> pentagons;
    std::span<const H3Index> cells;
};

constexpr std::array<H3Index, pentagonsPerRes> pentagonsRes0 = {
    0x8091fffffffffff, 0x807ffffffffffff, 0x80a7fffffffffff, 0x80d7fffffffffff, 0x80c3fffffffffff, 0x8031fffffffffff,
    0x8063fffffffffff, 0x804dfffffffffff, 0x8075fffffffffff, 0x80ebfffffffffff, 0x801dfffffffffff, 0x8009fffffffffff,
};

constexpr std::array<H3Index, pentagonsPerRes> pentagonsRes1 = {
    // ToDo: populate
};

constexpr std::array<H3Index, pentagonsPerRes> pentagonsRes2 = {
    // ToDo: populate
};

constexpr std::array<H3Index, cellsPerRes[0]> cellsRes0 = {
    0x8063fffffffffff, 0x8061fffffffffff, 0x8085fffffffffff, 0x8043fffffffffff, 0x807bfffffffffff, 0x8053fffffffffff,
    0x80abfffffffffff, 0x8021fffffffffff, 0x8087fffffffffff, 0x803dfffffffffff, 0x8065fffffffffff, 0x8097fffffffffff,
    0x80a3fffffffffff, 0x802dfffffffffff, 0x806bfffffffffff, 0x803ffffffffffff, 0x80cbfffffffffff, 0x80cdfffffffffff,
    0x80affffffffffff, 0x8025fffffffffff, 0x8011fffffffffff, 0x800bfffffffffff, 0x808dfffffffffff, 0x8041fffffffffff,
    0x8069fffffffffff, 0x8083fffffffffff, 0x80adfffffffffff, 0x80bdfffffffffff, 0x801ffffffffffff, 0x8059fffffffffff,
    0x8039fffffffffff, 0x80d7fffffffffff, 0x80e1fffffffffff, 0x8009fffffffffff, 0x80e5fffffffffff, 0x80a7fffffffffff,
    0x80c9fffffffffff, 0x8031fffffffffff, 0x8015fffffffffff, 0x8001fffffffffff, 0x8005fffffffffff, 0x8095fffffffffff,
    0x8073fffffffffff, 0x804bfffffffffff, 0x8075fffffffffff, 0x8099fffffffffff, 0x80c1fffffffffff, 0x80d1fffffffffff,
    0x8019fffffffffff, 0x8055fffffffffff, 0x8035fffffffffff, 0x80e7fffffffffff, 0x80f1fffffffffff, 0x8007fffffffffff,
    0x80edfffffffffff, 0x80d9fffffffffff, 0x80b9fffffffffff, 0x802ffffffffffff, 0x8017fffffffffff, 0x8003fffffffffff,
    0x800dfffffffffff, 0x809dfffffffffff, 0x8077fffffffffff, 0x80c7fffffffffff, 0x8051fffffffffff, 0x80bbfffffffffff,
    0x8045fffffffffff, 0x8047fffffffffff, 0x8029fffffffffff, 0x809ffffffffffff, 0x8027fffffffffff, 0x80d5fffffffffff,
    0x805ffffffffffff, 0x809bfffffffffff, 0x8037fffffffffff, 0x8071fffffffffff, 0x80cffffffffffff, 0x80d3fffffffffff,
    0x805dfffffffffff, 0x8049fffffffffff, 0x80bffffffffffff, 0x8091fffffffffff, 0x801bfffffffffff, 0x8093fffffffffff,
    0x801dfffffffffff, 0x80b3fffffffffff, 0x8079fffffffffff, 0x80effffffffffff, 0x8067fffffffffff, 0x80ddfffffffffff,
    0x80a1fffffffffff, 0x802bfffffffffff, 0x806dfffffffffff, 0x80e3fffffffffff, 0x806ffffffffffff, 0x8089fffffffffff,
    0x8013fffffffffff, 0x80b1fffffffffff, 0x803bfffffffffff, 0x80b7fffffffffff, 0x808ffffffffffff, 0x808bfffffffffff,
    0x80b5fffffffffff, 0x80e9fffffffffff, 0x804dfffffffffff, 0x80c3fffffffffff, 0x80dffffffffffff, 0x80a9fffffffffff,
    0x8033fffffffffff, 0x8081fffffffffff, 0x805bfffffffffff, 0x807ffffffffffff, 0x8023fffffffffff, 0x80dbfffffffffff,
    0x80ebfffffffffff, 0x800ffffffffffff, 0x80f3fffffffffff, 0x807dfffffffffff, 0x8057fffffffffff, 0x80c5fffffffffff,
    0x804ffffffffffff, 0x80a5fffffffffff,
};

constexpr std::array<H3Index, cellsPerRes[1]> cellsRes1 = {
    // ToDo: populate
};

constexpr std::array<H3Index, cellsPerRes[2]> cellsRes2 = {
    // ToDo: populate
};

constexpr std::array<CellInfo, maxRes> cellInfoByResolution = {{
    { pentagonsRes0, cellsRes0 },
    { pentagonsRes1, cellsRes1 },
    { pentagonsRes2, cellsRes2 },
}};

int getResolution(int satellites);


#endif //H3UTIL_H




