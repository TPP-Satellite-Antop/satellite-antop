#include <iostream>

#include "antop.h"

int main() {
    try {
        Antop antop{};
        antop.init(100);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
