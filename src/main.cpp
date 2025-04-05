#include <iostream>
#include <iomanip>

extern "C" {
    #include "h3lib/include/h3Index.h"
    #include "h3lib/include/localij.h"
}

int main() {
    // Example: Convert a latitude/longitude to an H3 cell index
    LatLng coord = {0, 0};  // San Francisco
    int resolution = 0;  // H3 resolution (0-15, 0 coarsest, 15 finest)

    H3Index cell;
    // Call H3 function: latLngToCell
    if (latLngToCell(&coord, resolution, &cell) != 0) {
        std::cerr << "Error converting coordinate to H3 index!" << std::endl;
        return 1;
    }

    // Print the H3 index (hexadecimal)
    std::cout << "H3 Index: 0x" << std::hex << cell << std::endl;

    CoordIJK output;
    H3Error _ = cellToLocalIjk(cell, 0x8055fffffffffff, &output);
    std::cout << "Output: (" << output.i << ", " << output.j << ", " <<output.k << ")" << std::endl;

    H3Error _b = cellToLocalIjk(cell, 0x8057fffffffffff, &output);
    std::cout << "Output: (" << output.i << ", " << output.j << ", " <<output.k << ")" << std::endl;

    H3Error _c = cellToLocalIjk(0x8055fffffffffff, 0x8057fffffffffff, &output);
    std::cout << "Output: (" << output.i << ", " << output.j << ", " <<output.k << ")" << std::endl;

    return 0;
}
