#ifndef COORD_H
#define COORD_H

extern "C" {
    #include "h3lib/include/coordijk.h"
}

class Coord {
    CoordIJK _coord = {0,0,0};

public:
    Coord();
    explicit Coord(const CoordIJK& coord);

    bool operator==(const Coord &c) const;
    Coord& operator+=(const Coord &c);
    Coord operator+(const Coord &c) const;

    [[nodiscard]] int i() const { return _coord.i; }
    [[nodiscard]] int j() const { return _coord.j; }
    [[nodiscard]] int k() const { return _coord.k; }

};

#endif //COORD_H
