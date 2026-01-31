#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include "h3api.h"

int hammingDistance(uint8_t origin, uint8_t target);
int h3Distance(H3Index origin, H3Index target);

#endif //UTIL_H