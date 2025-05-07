#include <iostream>
#include <iomanip>

#include "antop.h"
#include "address.h"

extern "C" {
    #include "h3lib/include/h3Index.h"
    #include "h3lib/include/localij.h"
}

int main() {
    constexpr LatLng coord = {-10, 0};
    constexpr int resolution = 3;

    /*H3Index idx;
    if (latLngToCell(&coord, resolution, &idx) != E_SUCCESS) {
        std::cerr << "Mierdaaaaaa" << std::endl;
        return 1;
    }

    H3Index out[7];
    if (gridDisk(idx, 1, out) != E_SUCCESS) {
        std::cerr << "caca" << std::endl;
        return 1;
    }

    CoordIJ output;
    for (const unsigned long h3 : out) {
        if (const H3Error e = cellToLocalIj(idx, h3, 0, &output); e != 0) {
            std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
            continue;
        }

        std::cout << idx << "-->" << h3 << ": [" << output.i << ", " << output.j << "]" << std::endl;
    }*/



    init(coord, resolution);

    return 0;
}
