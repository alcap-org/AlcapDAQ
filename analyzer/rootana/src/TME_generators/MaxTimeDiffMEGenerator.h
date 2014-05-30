#ifndef MAXTIMEDIFFMEGENERATOR_H_
#define MAXTIMEDIFFMEGENERATOR_H_

#include "TVMuonEventGenerator.h"

class MaxTimeDiffMEGenerator:public TVMuonEventGenerator{

 public:
  MaxTimeDiffMEGenerator(TVMuonEventGenerator::options* opts): TVMuonEventGenerator(opts){};
  virtual ~MaxTimeDiffMEGenerator(){};

 public:
  virtual int ProcessPulses(MuonEventList& muonEventsOut,const StringDetPulseMap& detectorPulsesIn);
  virtual bool ChecksForPileUp()const {return false;};
};
#endif //MAXTIMEDIFFMEGENERATOR_H_
