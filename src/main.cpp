#include <iostream>
#include <iomanip>

#include "antop.h"
#include "address.h"

extern "C" {
    #include "h3lib/include/h3Index.h"
}

int main() {
    constexpr LatLng coord = {-10, 0};
    //constexpr int resolution = 0;

    for (int i = 0; i <= 0; i++) {
        std::cout << "Resolution: " << i << std::endl;
        init(coord, i);
        std::cout << std::endl;
    }
    return 0;
}
