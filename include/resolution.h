#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <map>
#include <h3api.h>

const std::map<int, int> CELLS_BY_RESOLUTION = {
    {0, 122},
    {1, 842},
    {2, 5882},
};

H3Index getOriginForResolution(int res);
int getResolution(int satellites);

#endif //RESOLUTION_H
