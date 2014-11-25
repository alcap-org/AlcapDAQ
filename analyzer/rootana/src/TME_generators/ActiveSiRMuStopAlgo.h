#ifndef TME_GENERATORS_ACTIVESIRMUSTOPALGO_H
#define TME_GENERATORS_ACTIVESIRMUSTOPALGO_H

class TMuonEvent;
namespace IDs{class source;}
#include "TDetectorPulse.h"

namespace TMEAlgorithm{
    class ActiveSiRStop;
}

class TMEAlgorithm::ActiveSiRStop{
    public:
     ActiveSiRStop(const IDs::source& sir2, double muSc_min, double muSc_max, double SiR2_min, double SiR2_max);

     bool operator()(const TMuonEvent* tme, TDetectorPulse::ParentChannel_t)const;

     private:
     double fMuScMax, fMuScMin, fSiR2Max, fSiR2Min;
     IDs::source fSiR2source;
     
};

#endif // TME_GENERATORS_ACTIVESIRMUSTOPALGO_H
