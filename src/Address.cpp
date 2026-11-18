#include "Address.h"
#include <bitset>
#include <iostream>
#include <distance.h>
#include <bits/ostream.tcc>
#include <limits>

#define RIGHT_MASK 0b0000111
#define LEFT_MASK 0b0011100

/**
 * Returns a mask identifying the meaningful bits in a byte. Bits are stored MSB-first.
 */
static uint8_t validBitsMask(const size_t index, const size_t len, const uint8_t size) {
    if (index >= len) {
        return 0;
    }

    // All bytes except the last are completely filled.
    if (index < len - 1) {
        return 0xFF;
    }

    // A size of 0 means the last byte is completely filled.
    const uint8_t usedBits = size == 0 ? 8 : size;

    return static_cast<uint8_t>(0xFFu << (8 - usedBits));
}

Address::Address() = default;

Address::Address(const bool prime)
    : _prime(prime) {}

Address Address::copy() const {
    Address addr(prime());
    addr._data = this->_data;
    addr.bitOffset = this->bitOffset;
    return addr;
}

/**
 * Adds a coordinate to the Address instance, encoding it as a 3-bit direction.
 *
 * Directions are packed continuously into _data, starting from the most significant bit of each byte.
 *
 * - Invalid directions (>= INVALID_DIGIT) are ignored.
 * - bitOffset tracks the next available bit position in the last byte (0–7).
 * - Directions spanning two bytes are split across the byte boundary.
 *
 * @param coord A pointer to a CoordIJK structure representing the coordinate to be added to the Address.
 */
void Address::push(const CoordIJK *coord) {
    const uint8_t direction = _unitIjkToDigit(coord);
    if (direction >= INVALID_DIGIT) {
        return;
    }

    if (bitOffset == 0)
        _data.push_back(0);

    if (bitOffset <= 5)
        _data[_data.size() - 1] |= direction << (5 - bitOffset);
    else {
        _data[_data.size() - 1] |= direction >> (bitOffset - 5);
        _data.push_back(direction << (8 - (bitOffset - 5)));
    }

    bitOffset = (bitOffset + 3) % 8;
}

/**
 * Calculates hamming distance between this and addr, filling the shortest address with zeros.
 */
int Address::distanceTo(const Address &addr) const {
    if (prime() != addr.prime())
        return std::numeric_limits<int>::max();

    const size_t maxLen = std::max(_data.size(), addr._data.size());
    int dist = 0;

    for (size_t i = 0; i < maxLen; ++i) {
        const uint8_t a = i < _data.size() ? _data[i] : 0;
        const uint8_t b = i < addr._data.size() ? addr._data[i] : 0;

        const uint8_t maskedA = a & validBitsMask(i, _data.size(), bitOffset);
        const uint8_t maskedB = b & validBitsMask(i, addr._data.size(), addr.bitOffset);

        dist += hammingDistance(maskedA, maskedB);
    }

    return dist;
}

std::vector<uint8_t> Address::data() const {
    return _data;
}

bool Address::prime() const {
    return _prime;
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
    if (_prime != addr._prime) {
        return false;
    }

    const size_t maxLen = std::max(_data.size(), addr._data.size());

    for (size_t i = 0; i < maxLen; ++i) {
        uint8_t a = i < _data.size() ? _data[i] : 0;
        uint8_t b = i < addr._data.size() ? addr._data[i] : 0;

        if (i == _data.size() - 1 && bitOffset != 0)
            a &= static_cast<uint8_t>(0xFF << (8 - bitOffset));

        if (i == addr._data.size() - 1 && addr.bitOffset != 0)
            b &= static_cast<uint8_t>(0xFF << (8 - addr.bitOffset));

        if (a != b)
            return false;
    }

    return true;
}

size_t Address::hash() const {
    size_t result = std::hash<bool>{}(prime());
    for (const uint8_t byte : _data) {
        if (byte != 0) {
            result = result * 31 + std::hash<uint8_t>{}(byte);
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
