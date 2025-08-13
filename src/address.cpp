#include "address.h"
#include <bitset>
#include <iostream>
#include <util.h>
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

/**
 * Adds a coordinate to the Address instance, encoding it as a direction
 * and updating the internal data structure to accommodate the new entry.
 *
 * Behavior:
 * - Converts the given coordinate into a direction value.
 * - If the direction value is invalid (>= `INVALID_DIGIT`), the method performs no operation.
 * - Encodes the direction into an internal compact array `_data`:
 *   - For even-sized data, the direction is appended as a new element.
 *   - For odd-sized data, the direction is packed into the last element of `_data`
 *     by shifting the direction value left by 3 bits and adding it to the element.
 * - Updates the internal `_size` and `_len` variables to reflect the added coordinate.
 *
 * @param coord A pointer to a `CoordIJK` structure representing the coordinate to be added to the Address.
 */
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

void Address::pop() {
    if (_size == 0) {
        return;
    }

    if (_size % 2 == 0) { // The last direction is in the high bits of the last byte
        _data[_len - 1] &= RIGHT_MASK;
    } else { // The last direction is alone in the last byte
        _data.pop_back();
        _len--;
    }

    _size--;
}

// ToDo: parallelize hamming distance calc.
int Address::distanceTo(const Address &addr) const {
    if (prime() != addr.prime()) {
        return std::numeric_limits<int>::max();
    }

    const size_t shortest = _len < addr._len ? _len : addr._len;
    const size_t longest = _len >= addr._len ? _len : addr._len;

    int dist = 0;
    for (size_t i = 0; i < shortest; i++) {
        dist += hammingDistance(_data[i], addr._data[i]);
    }

    const std::vector<uint8_t> data = _len == longest ? this->_data : addr._data;

    for (size_t i = shortest; i < longest; i++) {
        dist += hammingDistance(data[i], 0);
    }

    return dist;
}

std::vector<uint8_t> Address::data() {
    return _data;
}

bool Address::prime() const {
    return _prime;
}

bool Address::operator<(const Address& addr) const {
    if (prime() != addr.prime())
        return prime() < addr.prime();
    if (_size != addr._size)
        return _size < addr._size;
    if (_len != addr._len)
        return _len < addr._len;
    return _data < addr._data;
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
 * @param addr The other Address instance to compare with.
 * @return True if the two Address instances are considered equal, false otherwise.
 */
bool Address::operator==(const Address& addr) const {
    if (prime() != addr.prime()){
        return false;
    }

    const size_t shortest = _len < addr._len ? _len : addr._len;
    const size_t longest = _len >= addr._len ? _len : addr._len;

    for (size_t i = 0; i < shortest; i++) {
        if (_data[i] != addr._data[i]) {
            return false;
        }
    }

    const std::vector<uint8_t> data = _len == longest ? this->_data : addr._data;

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
    os << "{" << addr.prime() << ", [";

    for (const unsigned char i : addr._data) {
        os << std::bitset<8>(i);  // Cast to int to print numeric value instead of char
    }
    os << "]}";

    return os;
}
