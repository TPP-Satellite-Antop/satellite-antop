#include <iostream>

#include "hypercube.h"

void a(int num) {
    hypercube antop{};
    antop.init(num);
}

int main() {
    try {
        a(100);
        a(800);
        a(5000);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
