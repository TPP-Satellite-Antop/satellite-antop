#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <cstdint>
extern "C" {
    #include "h3lib/include/coordijk.h"
}

class Address {
    std::vector<uint8_t> _data;
    size_t size = 0;
    size_t len = 0;

public:
    Address();

    void push(const CoordIJK *coord);
    std::vector<uint8_t> data();
};

#endif //ADDRESS_H
