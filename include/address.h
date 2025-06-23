#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <ostream>

#include "coord.h"

extern "C" {
    #include "h3lib/include/coordijk.h"
}

class Address {
    Coord _coord;
    bool _prime{};

public:
    Address();
    explicit Address(bool prime);

    [[nodiscard]] Address copy() const;
    [[nodiscard]] bool prime() const;
    [[nodiscard]] Coord coord() const;
    void push(const CoordIJK *coord);

    bool operator==(const Address &other) const;

    [[nodiscard]] size_t hash() const;
};

template<>
struct std::hash<Address> {
    size_t operator()(const Address& addr) const noexcept {
        return addr.hash();
    }
};

#endif //ADDRESS_H
