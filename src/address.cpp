#include "address.h"

Address::Address() = default;

Address::Address(const bool prime)
    : prime(prime) {}

Address Address::copy() const {
    Address addr(this->prime);
    addr._data = this->_data;
    addr.size = this->size;
    addr.len = this->len;
    return addr;
}

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

std::vector<uint8_t> Address::data() {
    return _data;
}

bool Address::operator<(const Address& other) const { //TODO check
    if (prime != other.prime)
        return prime < other.prime;
    if (size != other.size)
        return size < other.size;
    if (len != other.len)
        return len < other.len;
    return _data < other._data;  // std::vector already defines operator<
}

