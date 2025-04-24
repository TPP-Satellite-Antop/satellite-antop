#include "address.h"

Address::Address() = default;

Address::Address(const bool prime)
    : prime(prime) {}

Address Address::copy() const {
    Address addr(this->prime);
    addr._data = this->_data;
    addr._size = this->_size;
    addr.len = this->len;
    return addr;
}

void Address::push(const CoordIJK *coord) {
    const uint8_t direction = _unitIjkToDigit(coord);
    if (direction >= INVALID_DIGIT) {
        return;
    }

    if (_size % 2 == 0) {
        _data.push_back(direction);
        len++;
    } else {
        _data[len - 1] += direction << 3;
    }

    _size++;
}

std::vector<uint8_t> Address::data() {
    return _data;
}

bool Address::operator<(const Address& other) const {
    if (prime != other.prime)
        return prime < other.prime;
    if (_size != other._size)
        return _size < other._size;
    if (len != other.len)
        return len < other.len;
    return _data < other._data;
}

bool Address::operator==(const Address& other) const {
    if (prime != other.prime || _size != other._size || len != other.len){
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (_data[i] != other._data[i]) {
            return false;
        }
    }
    return true;
}

size_t Address::hash() const {
    size_t result = std::hash<bool>{}(prime);
    for (const uint8_t byte : _data) {
        result = (result * 31) + std::hash<uint8_t>{}(byte);
    }
    result = (result * 31) + std::hash<size_t>{}(_size);
    result = (result * 31) + std::hash<size_t>{}(len);
    return result;
}
