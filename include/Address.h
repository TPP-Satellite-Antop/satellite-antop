#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <ostream>

extern "C" {
    #include "coordijk.h"
}

class Address {
    size_t _size = 0;
    size_t _len = 0;
    bool _prime{};

public:
    std::vector<uint8_t> _data;

    Address();
    explicit Address(bool prime);

    [[nodiscard]] Address copy() const;
    void push(const CoordIJK *coord);

    [[nodiscard]] int distanceTo(const Address &addr) const;

    std::vector<uint8_t> data();
    [[nodiscard]] bool prime() const;

    bool operator<(const Address &other) const;

    bool operator==(const Address &other) const;

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
