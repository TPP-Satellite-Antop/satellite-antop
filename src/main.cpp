#include <iostream>
#include <iomanip>

#include "antop.h"
#include "address.h"

extern "C" {
    #include "h3lib/include/h3Index.h"
    #include "h3lib/include/localij.h"
}

int main() {
    constexpr LatLng coord = {0, 0};
    constexpr int resolution = 0;

    init(coord, resolution);

    return 0;
}
