#ifndef ANTOP_H
#define ANTOP_H

#include <memory>
#include <vector>
#include "h3util.h"

class Antop {
public:
    struct Impl;
    explicit Antop(int satellites);
    ~Antop();

    [[nodiscard]] std::vector<H3Index> getHopCandidates(H3Index src, H3Index dst) const;
    [[nodiscard]] int getResolution() const;

private:
    std::unique_ptr<Impl> impl;
};

#endif //ANTOP_H