#include "address.h"
#include <bitset>
#include <iostream>

#define RIGHT_MASK 0b0000111
#define LEFT_MASK 0b0011100

Address::Address() = default;

Address::Address(const bool prime)
    : _prime(prime) {}

Address Address::copy() const {
    Address addr(prime());
    addr._coord = this->_coord;
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
    if (_unitIjkToDigit(coord) >= INVALID_DIGIT) {
        return;
    }

    _coord += Coord(*coord);
}

bool Address::prime() const {
    return _prime;
}

Coord Address::coord() const {
    return _coord;
}

/*
bool Address::operator<(const Address& other) const {
    if (prime() != other.prime())
        return prime() < other.prime();
    if (_size != other._size)
        return _size < other._size;
    if (_len != other._len)
        return _len < other._len;
    return _data < other._data;
}
*/
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
    return prime() == addr.prime() && _coord == addr.coord();
}

size_t Address::hash() const {
    size_t result = std::hash<bool>{}(prime());
    result = (result * 31) + std::hash<uint8_t>{}(_coord.i());
    result = (result * 31) + std::hash<uint8_t>{}(_coord.j());
    result = (result * 31) + std::hash<uint8_t>{}(_coord.k());
    return result;
}
