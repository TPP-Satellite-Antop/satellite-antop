#include "address.h"
#include <bitset>
#include <iostream>
#include <bits/ostream.tcc>

#define RIGHT_MASK 0b0000111
#define LEFT_MASK 0b0011100

Address::Address() = default;

Address::Address(const bool prime)
    : _prime(prime) {}

Address Address::copy() const {
    Address addr(prime());
    addr._data = this->_data;
    addr._size = this->_size;
    addr._len = this->_len;
    return addr;
}

void Address::push(const CoordIJK *coord) {
    const uint8_t direction = _unitIjkToDigit(coord);
    if (direction >= INVALID_DIGIT) {
        return;
    }

    if (_size % 2 == 0) {
        _data.push_back(direction);
        _len++;
    } else {
        _data[_len - 1] += direction << 3;
    }

    _size++;
}

std::vector<uint8_t> Address::data() {
    return _data;
}

bool Address::prime() const {
    return _prime;
}

bool Address::operator<(const Address& other) const {
    if (prime() != other.prime())
        return prime() < other.prime();
    if (_size != other._size)
        return _size < other._size;
    if (_len != other._len)
        return _len < other._len;
    return _data < other._data;
}

bool Address::operator==(const Address& other) const {
    if (prime() != other.prime()){
        return false;
    }

    size_t size = _size;
    size_t shortestLen = _len;
    size_t longestLen = other._len;
    Address longest = other.copy();
    if (size > other._size) {
        size = other._size;
        shortestLen = other._len;
        longestLen = _len;
        longest = this->copy();
    }

    for (size_t i = 0; i < shortestLen; i++) {
        if ((i % 2 == 1 && (_data[i] & RIGHT_MASK) != (other._data[i] & RIGHT_MASK)) ||
            (i % 2 == 0 && (_data[i] & LEFT_MASK) != (other._data[i] & LEFT_MASK))) {
            return false;
        }
    }

    const std::vector<uint8_t> data = longest.data();
    if (shortestLen != longestLen && size % 2 == 1 && (data[shortestLen - 1] & LEFT_MASK) != 0) {
        return false;
    }

    for (size_t i = shortestLen; i < longestLen; i++) {
        if (data[i] != 0) {
            return false;
        }
    }

    return true;
}

size_t Address::hash() const {
    size_t result = std::hash<bool>{}(prime());
    for (const uint8_t byte : _data) {
        if (byte != 0) {
            result = (result * 31) + std::hash<uint8_t>{}(byte);
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const Address& addr) {
    os << "[";

    for (const unsigned char i : addr._data) {
        os << std::bitset<8>(i);  // Cast to int to print numeric value instead of char
    }
    os << "])";

    return os;
}
