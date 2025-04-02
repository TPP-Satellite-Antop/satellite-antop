/*
#include "address.h"

Address::Address() {

}

void Address::push(const CoordIJK *coord) {
    uint8_t direction = _unitIjkToDigit(coord);
    if (direction >= INVALID_DIGIT) {
        return;
    }

    if (size % 2 == 0) {
        data.push_back(direction);
        len++;
    } else {
        data[len - 1] += direction << 3;
    }

    size++;
}
*/

