#include "antop.h"

Address::Address(LatLng ref) {
    H3Index h3Idx;

    latLngToCell(&ref, 0, &h3Idx);
}
