#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <ostream>

extern "C" {
    #include "coordijk.h"
}

class Address {
    size_t bitOffset = 0;
    bool _prime{};

public:
    std::vector<uint8_t> _data;

    Address();
    explicit Address(bool prime);

    [[nodiscard]] Address copy() const;
    void push(const CoordIJK *coord);

    [[nodiscard]] int distanceTo(const Address &addr) const;

    [[nodiscard]] std::vector<uint8_t> data() const;
    [[nodiscard]] bool prime() const;

    bool operator<(const Address &addr) const;

    bool operator==(const Address &addr) const;

    friend std::ostream& operator<<(std::ostream& os, const Address& addr);

    [[nodiscard]] size_t hash() const;
};

template<>
struct std::hash<Address> {
    size_t operator()(const Address& addr) const noexcept {
        return addr.hash();
    }
};

#endif //ADDRESS_H
