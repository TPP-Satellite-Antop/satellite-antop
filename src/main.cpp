#include <iostream>
#include <iomanip>

extern "C" {
    #include "latLng.h"
    #include "h3lib/include/h3Index.h"
}

int main() {
    // Example: Convert a latitude/longitude to an H3 cell index
    LatLng coord = {37.7749, -122.4194};  // San Francisco
    int resolution = 0;  // H3 resolution (0-15, 0 coarsest, 15 finest)

    H3Index cell;
    // Call H3 function: latLngToCell
    if (latLngToCell(&coord, 0, &cell) != 0) {
        std::cerr << "Error converting coordinate to H3 index!" << std::endl;
        return 1;
    }

    // Print the H3 index (hexadecimal)
    std::cout << "H3 Index: 0x" << std::hex << cell << std::endl;

    return 0;
}
