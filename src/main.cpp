#include <iostream>
#include <iomanip>

#include "antop.h"
#include "address.h"

extern "C" {
    #include "h3lib/include/h3Index.h"
    #include "h3lib/include/localij.h"
}

void aux_func(H3Index origin, H3Index h3) {
    CoordIJK output;

    if (const H3Error e = cellToLocalIjk(origin, h3, &output); e != 0) {
        std::cerr << "Error converting coordinate to H3 index: " << e << std::endl;
    } else {
        std::cout << "Output " << origin << " to " << h3 << ": (" << output.i << ", " << output.j << ", " <<output.k << ")" << std::endl;
    }
}

int main() {
    constexpr LatLng coord = {0, 0};
    constexpr int resolution = 0;

    /*H3Index cell;
    // Call H3 function: latLngToCell
    if (latLngToCell(&coord, resolution, &cell) != 0) {
        std::cerr << "Error converting coordinate to H3 index!" << std::endl;
        return 1;
    }

    // Print the H3 index (hexadecimal)
    std::cout << "H3 Index: 0x" << std::hex << cell << std::endl;

    aux_func(0x8081fffffffffff, 0x807dfffffffffff);
    aux_func(0x8081fffffffffff, 0x8057fffffffffff);
    aux_func(0x8081fffffffffff, 0x805ffffffffffff);
    aux_func(0x8081fffffffffff, 0x808bfffffffffff);
    aux_func(0x8081fffffffffff, 0x80a9fffffffffff);
    aux_func(0x8081fffffffffff, 0x80a5fffffffffff);

    std::cout << "\n";

    aux_func(0x807dfffffffffff, 0x8081fffffffffff);
    aux_func(0x8057fffffffffff, 0x8081fffffffffff);
    aux_func(0x805ffffffffffff, 0x8081fffffffffff);
    aux_func(0x808bfffffffffff, 0x8081fffffffffff);
    aux_func(0x80a9fffffffffff, 0x8081fffffffffff);
    aux_func(0x80a5fffffffffff, 0x8081fffffffffff);*/

    init(coord, resolution);

    return 0;
}
