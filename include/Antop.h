#ifndef ANTOP_H
#define ANTOP_H

#include <memory>
#include <vector>
#include "h3util.h"

class Antop {
public:
    explicit Antop(int satellites);
    ~Antop();

    std::vector<H3Index> getHopCandidates(H3Index src, H3Index dst) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif //ANTOP_H