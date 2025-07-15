#ifndef ANTOP_H
#define ANTOP_H

#include <map>
#include "hex.h"

class Antop {
    std::map<H3Index, Hex> hexes;

public:
    Antop();
};

void init(int satellites);

/*TODO
std::string load(H3Index dir);
std::string getNextHopId(std::string src, std::string dst);
H3Index getDir(std::string id);
void turnOff(std::string id);
*/

#endif //ANTOP_H
