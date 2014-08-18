#ifndef MAXTIMEDIFFMEGENERATOR_H_
#define MAXTIMEDIFFMEGENERATOR_H_

#include "TVMuonEventGenerator.h"

class MaxTimeDiffMEGenerator:public TVMuonEventGenerator{

 public:
  MaxTimeDiffMEGenerator(TMEGeneratorOptions* opts): TVMuonEventGenerator("MaxTimeDiff",opts){};
  virtual ~MaxTimeDiffMEGenerator(){};

 public:
  virtual int ProcessPulses(MuonEventList& muonEventsOut,const SourceDetPulseMap& detectorPulsesIn);
};
#endif //MAXTIMEDIFFMEGENERATOR_H_
