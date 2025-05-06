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

/**
 * Compares the current Address instance with another Address instance to determine equality.
 * The equality is evaluated based on several conditions involving both prime properties and data contents.
 *
 * Conditions for equality:
 * - Both Address instances must have the same prime property.
 * - Each corresponding element in the data vector of both instances must be equal.
 * - If the lengths of the Address data differ, extra elements in the longer data must be 0.
 *
 * @param other The other Address instance to compare with.
 * @return True if the two Address instances are considered equal, false otherwise.
 */
bool Address::operator==(const Address& other) const {
    if (prime() != other.prime()){
        return false;
    }

    const size_t shortest = _len < other._len ? _len : other._len;
    const size_t longest = _len >= other._len ? _len : other._len;

    for (size_t i = 0; i < shortest; i++) {
        if (_data[i] != other._data[i]) {
            return false;
        }
    }

    const std::vector<uint8_t> data = _len == longest ? this->_data : other._data;

    for (size_t i = shortest; i < longest; i++) {
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
