#ifndef CELL_H
#define CELL_H

#include "address.h"

class Cell {
    public:
        std::vector<Address> addresses;

        Cell() = default;
        void addAddress(const Address& address);
        [[nodiscard]] int distanceTo(const Cell& cell) const;
};

#endif //CELL_H
