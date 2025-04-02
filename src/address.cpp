/*
#include "address.h"

Address::Address() = default;

void Address::push(const CoordIJK *coord) {
    const uint8_t direction = _unitIjkToDigit(coord);
    if (direction >= INVALID_DIGIT) {
        return;
    }

    if (size % 2 == 0) {
        _data.push_back(direction);
        len++;
    } else {
        _data[len - 1] += direction << 3;
    }

    size++;
}
*/

std::vector<uint8_t> Address::data() {
    return _data;
}

