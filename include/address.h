#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>

extern "C" {
    #include "h3lib/include/coordijk.h"
}

class Address {
    std::vector<uint8_t> _data;
    size_t _size = 0;
    size_t len = 0;
    bool prime{};

public:
    Address();
    explicit Address(bool prime);

    [[nodiscard]] Address copy() const;
    void push(const CoordIJK *coord);
    std::vector<uint8_t> data();

    bool operator<(const Address &other) const;

    bool operator==(const Address &other) const;
};

#endif //ADDRESS_H
