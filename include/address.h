#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <ostream>

extern "C" {
    #include "h3lib/include/coordijk.h"
}

class Address {
    std::vector<uint8_t> _data;
    size_t _size = 0;
    size_t _len = 0;
    bool _prime{};

public:
    Address();
    explicit Address(bool prime);

    [[nodiscard]] Address copy() const;
    void push(const CoordIJK *coord);
    std::vector<uint8_t> data();
    bool prime() const;

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
