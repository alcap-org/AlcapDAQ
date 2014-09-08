#ifndef UTILS_INTERPOLATEPULSE_H
#define UTILS_INTERPOLATEPULSE_H

class TH1D;
class TPulseIsland;
#include <string>

// Creates a histogram with sub-bin resolution
TH1D* InterpolatePulse(
    const TPulseIsland* pulse, std::string histname,
     std::string histtitle, bool interpolate, int refine);


#endif //UTILS_INTERPOLATEPULSE_H
