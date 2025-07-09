#ifndef CELL_H
#define CELL_H

#include "address.h"

class Cell {
    std::vector<Address> addresses;

    public:
        Cell();
        void addAddress(const Address& address);
        int distanceTo(const Cell* cell);

};

#endif //CELL_H
