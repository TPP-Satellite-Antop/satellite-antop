#include <iostream>
#include <iomanip>

extern "C" {
 #include "constants.h"
}

int main() {
    std::cout << NUM_ICOSA_FACES << std::endl;
    return 0;
}