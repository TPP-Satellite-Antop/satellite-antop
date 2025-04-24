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



namespace std {
    template<>
    struct hash<Address> {
        size_t operator()(Address const& a) const noexcept {
            // start with a seed
            size_t seed = 0;

            // helper to mix one value into the seed
            auto hash_combine = [&](auto const& v) {
                // from boost::hash_combine
                seed ^= std::hash<std::decay_t<decltype(v)>>{}(v)
                      + 0x9e3779b97f4a7c15ULL
                      + (seed << 6)
                      + (seed >> 2);
            };

            // mix the bytes
            std::vector<uint8_t> data = a.data();
            for (const uint8_t v : data) {
                hash_combine(v);
            }

            return seed;
        }
    };
}

