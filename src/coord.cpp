#include "coord.h"

Coord::Coord() = default;

Coord::Coord(const CoordIJK& coord) {
    _coord = coord;
}

Coord Coord::operator+(const Coord &c) const {
    Coord coord;
    coord._coord.i = _coord.i + c._coord.i;
    coord._coord.j = _coord.j + c._coord.j;
    coord._coord.k = _coord.k + c._coord.k;
    return coord;
}

bool Coord::operator==(const Coord &c) const {
    return _coord.i == c._coord.i &&
           _coord.j == c._coord.j &&
           _coord.k == c._coord.k;
}

Coord& Coord::operator+=(const Coord &c) {
    _coord.i += c._coord.i;
    _coord.j += c._coord.j;
    _coord.k += c._coord.k;
    return *this;
}

