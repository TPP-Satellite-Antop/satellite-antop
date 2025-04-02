#ifndef ADDRESS_H
#define ADDRESS_H

#include <vector>
#include <cstdint>
extern "C" {
    #include "h3lib/include/coordijk.h"
}

typedef enum {
    EVEN_K_AXES_DIGIT = K_AXES_DIGIT, /* 00 000 001 */
    EVEN_J_AXES_DIGIT = J_AXES_DIGIT, /* 00 000 010 */
    EVEN_JK_AXES_DIGIT = JK_AXES_DIGIT, /* 00 000 011 */
    EVEN_I_AXES_DIGIT = I_AXES_DIGIT, /* 00 000 100 */
    EVEN_IK_AXES_DIGIT = I_AXES_DIGIT, /* 00 000 100 */
    EVEN_IJ_AXES_DIGIT = IJ_AXES_DIGIT, /* 00 000 110 */
    ODD_K_AXES_DIGIT = EVEN_K_AXES_DIGIT << 3, /* 00 001 000 */
    ODD_J_AXES_DIGIT = EVEN_J_AXES_DIGIT << 3, /* 00 010 000 */
    ODD_JK_AXES_DIGIT = EVEN_JK_AXES_DIGIT << 3, /* 00 000 000 */
    ODD_I_AXES_DIGIT = EVEN_I_AXES_DIGIT << 3,
    ODD_IK_AXES_DIGIT = EVEN_IK_AXES_DIGIT << 3, /* 5 */
    ODD_IJ_AXES_DIGIT = EVEN_IJ_AXES_DIGIT << 3, /* 6 */
} DirectionValue;

class Address {
    std::vector<uint8_t> _data;
    size_t size = 0;
    size_t len = 0;

public:
    Address();

    void push(const CoordIJK *coord);
    std::vector<uint8_t> data();
};



#endif //ADDRESS_H
